//CPP:ATLAS-TDAQ/OpenTsdb/HttpOpenTsdbClient.cpp
//CPP:ATLAS-TDAQ/OpenTsdb/IOpenTsdbClient.cpp
#ifndef __HTTP_TSDB_CLIENT_H__
#define __HTTP_TSDB_CLIENT_H__

#include "pdevslib.h"

#include <string>
#include <deque>
#include <map>
#include <string>
#include <sstream>
#include "IOpenTsdbClient.h"

#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/scoped_ptr.hpp>

//JSON
#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"


using boost::asio::ip::tcp;

/**
 * Client for OpenTSDB using the HTTP API for sending metrics (PUT endpoint) to the TSD.
 * TSD must be running at the given <host>:<port>
 *
 * HTTP API: http://opentsdb.net/docs/build/html/api_http/index.html
 * PUT endpoint: http://opentsdb.net/docs/build/html/api_http/put.html
 * To check if TSD is running try to access the web interface at: http://<host>:<port>/
 *
 * this is just a Proof of Concept. Some missing things:
 * TODO: - Test with adding several points (test queue and performance)
 *       - Improve error handling
 *       - Parse http response using a library. Ej:cpp-net: http://cpp-netlib.org/0.11.1/examples/http/http_client.html
 *       - Queue point values (currently queuing the string).
 *       - Enable compression (if there are more than X points in the queue)
 *       - All values are sent as double
 *       - timestamps are doubles, but OpenTSDB only records with second precision so it removes the decimal part.
 */
class HttpOpenTsdbClient : public IOpenTsdbClient
{
// OpenTSDB Http API endpoint. '?details' makes the response more verbose. See endpoint documentation.
#define HTTP_API_ENDPOINT_DEBUG "/api/put?details"
#define HTTP_API_ENDPOINT "/api/put"

public:
    typedef std::map<std::string, std::string> TagsType;

    HttpOpenTsdbClient(const std::string & host, unsigned short port)
        : m_io_service(),
          m_connected(false),
          m_host(host),
          m_port(port),
          m_reconnect_timer(m_io_service),
          m_resolver(m_io_service)
    {
        start_connect();
    }

    ~HttpOpenTsdbClient() { }

    // run() should be called from a separate thread
    void run()
    {
    	m_work.reset(new boost::asio::io_service::work(m_io_service));
        m_io_service.run();
    }

    void add_point(
        const std::string & metric,
        double timestamp,
        const double & value,
        const TagsType * const ptags)
    {
    	if(this->closeRequested){
    		printLog(1, "[HttpOpenTsdbClient] warning: attempted to add_point after closing the connection. Skipping point.\n");
    		return;
    	}

        write(format(metric, timestamp, value, ptags));
    }

    void close()
    {
    	this->closeRequested = true;

    	m_io_service.post(boost::bind(&HttpOpenTsdbClient::do_close, this));
    }

    bool isConnected() { return this->m_connected; };
    bool isClosed() { return false; };

private:

    int getResponse(){
		boost::asio::streambuf response;
		boost::asio::read_until(*this->m_socket, response, "\r\n");

		// Check that response is OK.
		std::istream response_stream(&response);
		std::string http_version;
		response_stream >> http_version;
		unsigned int status_code;
		response_stream >> status_code;
		std::string status_message;
		std::getline(response_stream, status_message);
		if (!response_stream || http_version.substr(0, 5) != "HTTP/")
		{
			printLog(1,"[HttpOpenTsdbClient] warning: Invalid response\n");
		    return 0;
		}
		if (status_code != 200)
		{
			// TODO: handle error correctly. Error 405 for example might give more details.
			printLog(1,"[HttpOpenTsdbClient]warning: Response returned with status code %u. \n", status_code);
			return 0;
		}else {
			printLog(LOG_LEVEL_FULL_LOGGING,"status_code=200\n");
		}

		// Read the response headers, which are terminated by a blank line.
		boost::asio::read_until(*this->m_socket, response, "\r\n\r\n");

		// Process the response headers.
		std::string header;
		while (std::getline(response_stream, header) && header != "\r")
		  ;//std::cout << header << "\n";
		//std::cout << "\n";

		// Write whatever content we already have to output.
		if (response.size() <= 0){
			printLog(LOG_LEVEL_ERROR,"No content in response");
			return 0;
		}

		std::istream response_stream_json(&response);
		std::string jsonResponse;
		response_stream_json >> jsonResponse;
		rapidjson::Document d;
		d.Parse(jsonResponse.c_str());

		//printLog(1,"errors: %i. failed: %i. success: %i \n",d["errors"].Size() , d["failed"].GetInt(), d["success"].GetInt());

		return d["success"].GetInt();
    }

    void write(const std::string & data)
    {
    	m_io_service.post(boost::bind(&HttpOpenTsdbClient::do_write, this, data));
    }

    std::string format(
        const std::string & metric,
        double timestamp,
        const double & value,
        const TagsType * const ptags)
    {
        std::ostringstream oss;

        // Create JSON representation of the metric
        rapidjson::StringBuffer message;
		rapidjson::Writer<rapidjson::StringBuffer> writer(message);
		writer.StartObject();
		writer.String("metric");
		writer.String(metric.data());
		writer.String("timestamp");
		writer.Double(timestamp);
		writer.String("value");
		writer.Double(value); // TODOL if the value is INT, we are using extra space declaring it as double
		writer.String("tags");
		writer.StartObject(); // start tags object
		for (const auto& kv : *ptags) {
			writer.String(kv.first.data());
			writer.String(kv.second.data());
		}
		writer.EndObject(); // end tags
		writer.EndObject(); // end StartObject

		oss << "POST " << HTTP_API_ENDPOINT_DEBUG << " HTTP/1.1\r\n";
		oss << "Host: " << this->m_host << ":" << boost::lexical_cast<std::string>(this->m_port) << "\r\n";
		oss << "Accept: */*\r\n";
		oss << "Content-Type: " << "application/json" << "\r\n";
		oss << "Content-Length: " << message.GetSize() << "\r\n";
		oss << "\r\n";
		oss << message.GetString() ;

       // std::cout << "The message to send is: " << oss.str();

        return oss.str();
    }

    void start_connect()
    {
        tcp::resolver::query query(m_host, boost::lexical_cast<std::string>(m_port));
        m_resolver.async_resolve(query,
            boost::bind(&HttpOpenTsdbClient::handle_resolve, this,
            boost::asio::placeholders::error,
            boost::asio::placeholders::iterator));
    }

    void restart_connect(const boost::system::error_code & error)
    {
        if(!error)
        {
            // not interrupted during an async_wait
            start_connect();
        }
        else
        {
        	printLog(1, "[HttpOpenTsdbClient] warning: restart_connect was interrupted!\n");
        }
    }

    void handle_resolve(
        const boost::system::error_code & error,
        tcp::resolver::iterator ep_it)
    {
        if(!error)
        {
        	// attempt to connect to each endpoint in iterator until we fail
            m_socket.reset(new tcp::socket(m_io_service));
            boost::asio::async_connect(*m_socket, ep_it,
                boost::bind(&HttpOpenTsdbClient::handle_connect, this,
                    boost::asio::placeholders::error));
        }
        else
        {
        	printLog(1, "[HttpOpenTsdbClient] warning: handle_resolve error --> RESTART \n");
            // restart in 2 seconds
            m_reconnect_timer.expires_from_now(boost::posix_time::seconds(2));
            m_reconnect_timer.async_wait(
                boost::bind(&HttpOpenTsdbClient::restart_connect, this,
                    boost::asio::placeholders::error));
        }
    }

    void handle_connect(const boost::system::error_code & error)
    {
        if(!error)
        {
        	m_connected = true;

            if(!m_pending.empty())
            {
                // send first message from the queue now
            	printLog(1,"[HttpOpenTsdbClient]: Now connected. Pending %i messages. Sending first message \n", m_pending.size());
            	m_io_service.post(boost::bind(&HttpOpenTsdbClient::handle_write, this));
            }
        }
        else
        {
        	printLog(1,"[HttpOpenTsdbClient]: warning: not connected: \n");
        	std::cout<< error;
            m_reconnect_timer.expires_from_now(boost::posix_time::seconds(2));
            m_reconnect_timer.async_wait(
                boost::bind(&HttpOpenTsdbClient::restart_connect, this,
                    boost::asio::placeholders::error));
        }
    }

    void handle_write()
    {
    	//printLog(1, "[handle_write]called: \n");
    	this->maxQueued = std::max((unsigned int)this->maxQueued, (unsigned int) m_pending.size());

		if(!m_pending.empty())
		{
			//printLog(1,"[HttpOpenTsdbClient] Pending %i messages (max %u). Sending next: %s \n", m_pending.size(), this->maxQueued, m_pending.front().data());
			try	{
				// Do the write
				boost::system::error_code error;
				boost::asio::write(*this->m_socket, boost::asio::buffer(m_pending.front())); //this->m_socket->write_some()

				// wait for the response
				int success = this->getResponse();

				if(success > 0){
					//printLog(1,"[HttpOpenTsdbClient] Successfully wrote point:  \n"/*, m_pending.front().data()*/);
					m_pending.pop_front();
					this->writeSuccess++;

					m_io_service.post(boost::bind(&HttpOpenTsdbClient::handle_write, this));
				} else {
					this->writeErrors++;
					this->handle_write_error();
				}

			}
			catch (std::exception& e)
			{
				this->writeErrors++;
				std::cout << "Exception: " << e.what() << "\n";
				printLog(1,"[HttpOpenTsdbClient] WARNING: Exception: %s \n",e.what());
				this->handle_write_error();
			}
		}else if(this->closeRequested){ // if nothing else to send and request to close
			m_work.reset();
			m_io_service.post(boost::bind(&HttpOpenTsdbClient::do_close, this));
		}
    }

    void handle_write_error(){
    	printLog(1,"[HttpOpenTsdbClient] WARNING: Not sent:%s  \n", m_pending.front().data() );
    	m_connected = false;
    	this->close();
    	throw "[HttpOpenTsdbClient] Message error. Check Log";

    	// Use the following code to attempt reconections when there is an error.
//		m_reconnect_timer.expires_from_now(boost::posix_time::seconds(2));
//		m_reconnect_timer.async_wait(
//		boost::bind(&HttpOpenTsdbClient::restart_connect, this, boost::asio::placeholders::error));
    }

    void do_write(std::string data)
    {
    	bool write_in_progress = !m_pending.empty();
        m_pending.push_back(data);

        if(!m_connected)       	return;

        if(!write_in_progress)
        {
        	printLog(1, "sending first message: %s ", m_pending.front().data());
        	m_io_service.post(boost::bind(&HttpOpenTsdbClient::handle_write, this));
        }
    }

    void do_close()
    {
    	if(!this->m_pending.empty()){
    		printLog(1, "[HttpOpenTsdbClient] requested to close with %u pending (NOT closing now). Successful writes:%u. Write errors: %u. Max Queued: %u \n", this->m_pending.size(), this->writeSuccess, this->writeErrors, this->maxQueued);
    		return;
    	}

   		m_work.reset();
    	m_socket->close();
    	printLog(1, "[HttpOpenTsdbClient] closing socket. Successful writes:%u. Write errors: %u. Max Queued: %u\n", this->writeSuccess, this->writeErrors, this->maxQueued);
    }

private:
    boost::asio::io_service m_io_service;
    boost::shared_ptr<tcp::socket> m_socket;
    std::deque<std::string> m_pending;

    bool m_connected = false;

    std::string m_host;
    unsigned short m_port;
    boost::asio::deadline_timer m_reconnect_timer;
    tcp::resolver m_resolver;
    boost::scoped_ptr<boost::asio::io_service::work> m_work;

    bool closeRequested = false;

public:
    unsigned int writeErrors = 0;
    unsigned int writeSuccess = 0;
    unsigned int maxQueued = 0;
};

#endif

