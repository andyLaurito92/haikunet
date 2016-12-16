//CPP:ATLAS-TDAQ/OpenTsdb/TelnetOpenTsdbClient.cpp
//CPP:ATLAS-TDAQ/OpenTsdb/IOpenTsdbClient.cpp
#ifndef __TELNET_TSDB_CLIENT_H__
#define __TELNET_TSDB_CLIENT_H__

#include "pdevslib.h"
#include "ATLAS-TDAQ/globals.h"
#include "IOpenTsdbClient.h"

#include <string>
#include <deque>
#include <map>
#include <string>
#include <sstream>
#include <atomic>

#include "sinks/ParameterReader.h"

#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/scoped_ptr.hpp>
#include <boost/lambda/lambda.hpp>

using boost::asio::ip::tcp;

class TelnetOpenTsdbClient : public IOpenTsdbClient {
public:
    typedef std::map<std::string, std::string> TagsType;

    TelnetOpenTsdbClient(const std::string & host, unsigned short port);
    ~TelnetOpenTsdbClient();

    /***
     * run() should be called from a separate thread
     */
    void run();
    void add_point(
        const std::string & metric,
        unsigned long timestamp,
        const double & value,
        const TagsType * const ptags);

    void close();
    bool isConnected() { return this->m_connected.load(); };
    bool isClosed() { return !this->m_connected.load(); };

private:
    void write(const std::string data);

    /***
     * timestamp - time in openTSDB has second precision. Millisecond can be used, but read documentation: http://opentsdb.net/docs/build/html/user_guide/writing.html#timestamps
     */
    std::string format(
        const std::string & metric,
        unsigned long timestamp,
        const double & value,
        const TagsType * const ptags);

    void start_connect();
    void handle_write(std::string writtenPoint, const boost::system::error_code & error);
    void handle_write_error(std::string error);
    void enqueueWrite(std::string addPointCommand);
    void do_close();
    void forceClose();
    bool readFromSocketIfAvailable(char* data, unsigned int size);
    std::string getVersion(boost::posix_time::time_duration timeout);
    void check_deadline();

    /* methods for async connect. Currently not used. See start_connect */
//    void restart_connect(const boost::system::error_code & error);
//    void handle_resolve(const boost::system::error_code & error,tcp::resolver::iterator ep_it);
//    void handle_connect(const boost::system::error_code & error);

private:
    // parameters
    const std::string m_host;
    const unsigned short m_port;

    boost::scoped_ptr<boost::asio::io_service::work> m_work;
    boost::asio::io_service m_io_service;
    boost::asio::io_service::strand m_strand; // to guarantee jobs are run sequentially in the give post() order.
    boost::shared_ptr<tcp::socket> m_socket;
    boost::asio::deadline_timer m_reconnect_timer;

    tcp::resolver m_resolver;

    std::atomic<bool> closeRequested;
    std::atomic<bool> m_connected;

    std::atomic<unsigned int> pendingWrites;
    std::atomic<unsigned int> writeErrors;
    std::atomic<unsigned int> writeSuccess;
    std::atomic<unsigned int> maxQueued;

    unsigned short int formatWarningMessages = 0;

public:

};

#endif

