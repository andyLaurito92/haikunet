#include "ATLAS-TDAQ/OpenTsdb/TelnetOpenTsdbClient.h"

TelnetOpenTsdbClient::TelnetOpenTsdbClient(const std::string & host, unsigned short port)
        : m_host(host),
          m_port(port),
		  m_io_service(),
		  m_strand(m_io_service),
          m_reconnect_timer(m_io_service),
          m_resolver(m_io_service),
		  closeRequested(false),
		  m_connected(false),
		  pendingWrites(0),
		  writeErrors(0),
		  writeSuccess(0),
		  maxQueued(0)
    {
		// set the deadline to positive infinity so that the actor takes no action until a specific deadline is set.
		m_reconnect_timer.expires_at(boost::posix_time::pos_infin);

	    // Start the persistent actor that checks for deadline expiry.
	    this->check_deadline();

		printLog(LOG_LEVEL_ERROR, "[TelnetOpenTSDBClient] start_connect \n");
        start_connect();
    }

TelnetOpenTsdbClient::~TelnetOpenTsdbClient() {
	this->forceClose();
}

// run() should be called from a separate thread
void TelnetOpenTsdbClient::run()
{
	printLog(LOG_LEVEL_ERROR, "[TelnetOpenTSDBClient] Running thread\n");
	m_work.reset(new boost::asio::io_service::work(this->m_io_service));
	m_io_service.run();

}

void TelnetOpenTsdbClient::add_point(
	const std::string & metric,
	unsigned long timestamp,
	const double & value,
	const TagsType * const ptags)
{
	if(this->closeRequested.load()){
		printLog(LOG_LEVEL_ERROR, "[TelnetOpenTSDBClient] warning: attempted to add_point after closing the connection. Skipping point: %s %f %f (modelName=%s)\n", metric.data(), timestamp, value, ptags->at("modelName").data());
		return;
	}

	if(this->isClosed()){
		printLog(LOG_LEVEL_ERROR, "[TelnetOpenTSDBClient] warning: attempted to add_point when the connection is closed. Skipping point: %s %f %f (modelName=%s)\n", metric.data(), timestamp, value, ptags->at("modelName").data());
		//throw std::runtime_error("[TelnetOpenTSDBClient] warning: attempted to add_point when the connection is closed. Skipping point.");
	}

	if(this->pendingWrites.load() >= 1000){
		// sleep the simulation thread to slow down the amount of points logged
		printLog(LOG_LEVEL_ERROR, "[TelnetOpenTSDBClient] Big load detected. Sleeping for 1 second. (PendingWrites=%u ; Successful writes: %u) \n", this->pendingWrites.load(), this->writeSuccess.load());

		//boost::this_thread::sleep(boost::posix_time::milliseconds(500));
		sleep(1); // TODO: one second might be too much... problems compiling using boost::thread :-(

		printLog(LOG_LEVEL_ERROR, "[TelnetOpenTSDBClient] Waking up. (PendingWrites=%u; Successful writes: %u) \n", this->pendingWrites.load(), this->writeSuccess.load());
//		std::string version = this->getVersion(boost::posix_time::seconds(5));
//		if(version.find("2.1") == std::string::npos){
//			printLog(LOG_LEVEL_ERROR, "[TelnetOpenTSDBClient] WARNING: Wrong OpenTsdb version. Expecting '2.1' but server returned: %s \n", version.data());
//			throw "WARNING: Wrong OpenTsdb version";
//		}
	}

	// get Telnet format
	std::string addCommand = format(metric, timestamp, value, ptags);

	// Enqueue command to be sent
	this->pendingWrites++;

	//printLog("%s", addCommand.data()); // log to the -1.log file so that it can be used for batch update
//	printLog(LOG_LEVEL_IMPORTANT, "[TelnetOpenTSDBClient] adding new point: %s. Pending Writes: %u \n", addCommand.data(), this->pendingWrites.load());

	this->m_strand.post(boost::bind(&TelnetOpenTsdbClient::enqueueWrite, this, addCommand));
}

void TelnetOpenTsdbClient::close()
{
	this->closeRequested = true;

	printLog(LOG_LEVEL_IMPORTANT, "[TelnetOpenTSDBClient] Close Requested. Successful writes:%u. Write errors: %u. Max Queued: %u ; Pending Writes: %u\n", this->writeSuccess.load(), this->writeErrors.load(), this->maxQueued.load(), this->pendingWrites.load());

	this->m_strand.post(boost::bind(&TelnetOpenTsdbClient::do_close, this));
}

void TelnetOpenTsdbClient::enqueueWrite(std::string addPointCommand)
{
//		printLog(LOG_LEVEL_ERROR, "[TelnetOpenTSDBClient] writting point: %s. Pending Writes: %u\n", addPointCommand.data(), this->pendingWrites.load());
		boost::system::error_code error;
		boost::asio::write(*this->m_socket.get(), boost::asio::buffer(addPointCommand, addPointCommand.length()),
						   boost::asio::transfer_all(),
						   error);

		this->handle_write(addPointCommand, error);
//		printLog(LOG_LEVEL_ERROR, "[TelnetOpenTSDBClient] Point written: %s. Pending Writes: %i\n", addPointCommand.data(), this->pendingWrites.load());
}

/***
 * timestamp - time in openTSDB has second precision. Millisecond can be used, but read documentation: http://opentsdb.net/docs/build/html/user_guide/writing.html#timestamps
 */
std::string TelnetOpenTsdbClient::format(
	const std::string & metric,
	unsigned long timestamp,
	const double & value,
	const TagsType * const ptags)
{
	// validate that timestamp is not over the max permitted by OpenTsdb
	if (timestamp >= 4294967296){
			printLog(LOG_LEVEL_ERROR, "[TelnetOpenTSDBClient] ERROR: Max timestamp exceeded. Trying to log %lu and the maximum timestamp allowed by OpenTsdb is 4294967296. \n", timestamp);
					throw std::runtime_error("[TelnetOpenTSDBClient] Max timestamp exceeded");
	} else if(timestamp >= 2147483648){
		// print only a few of these messages
		if(formatWarningMessages++ % 1000 == 0) printLog(LOG_LEVEL_ERROR, "[TelnetOpenTSDBClient] WARNING: Big timestamp detected %lu (>2147483648), which required 64bit representation. Some applications/OpenTsdb interfaces might not support it.. \n", timestamp);
	}

	// create the command format: put <metricName> <timestamp> <value> { tags }
	std::ostringstream oss;
	oss.precision(10);
	oss << "put " << metric << " " << std::fixed << timestamp << " " << value << " ";
	if(ptags)
	{
		const TagsType & tags = *ptags;
		for(TagsType::const_iterator it = tags.begin(); it != tags.end(); ++it)		{
			oss << it->first << "=" << it->second << " ";
		}
	}
	oss << "\n";

//	printLog(LOG_LEVEL_IMPORTANT, "[TelnetOpenTSDBClient] logging metric: %s \n", oss.str().data());
	std::string ret = oss.str();
	return ret;
}

/**
 * Connects to the OpenTSDB host.
 * This operation is blocking. It could be asynchronous, but we decided to keep it synchronous to avoid running simulations if the connection fails.
 */
void TelnetOpenTsdbClient::start_connect()
{
	printLog(LOG_LEVEL_IMPORTANT, "[TelnetOpenTSDBClient] Opening connection with OpenTSDB on %s:%u \n", this->m_host.data(), this->m_port);

	try{
		// resolve host&port to an endpoint
		tcp::resolver::query query(this->m_host, boost::lexical_cast<std::string>(this->m_port));
		tcp::resolver::iterator endpoint_iterator = this->m_resolver.resolve(query);

		// create & connect to the socket.
		this->m_socket.reset(new tcp::socket(this->m_io_service));
		boost::asio::connect(*this->m_socket.get(), endpoint_iterator);

		printLog(LOG_LEVEL_DEBUG, "[TelnetOpenTSDBClient] Socket to OpenTSDB on %s:%i OPEN \n", this->m_host.data(), this->m_port);

		// test if connection is responding properly
		std::string version = this->getVersion(boost::posix_time::seconds(5));
		if(version.find("2.1") == std::string::npos){
			printLog(LOG_LEVEL_ERROR, "[TelnetOpenTSDBClient] WARNING: Wrong OpenTsdb version. Expecting '2.1' but server returned: %s \n", version.data());
		}

		printLog(LOG_LEVEL_IMPORTANT, "[TelnetOpenTSDBClient] Connection to OpenTSDB on %s:%i SUCCESSFUL \n", this->m_host.data(), this->m_port);

		this->m_connected = true;

	}  catch (std::exception& e)	{
		printLog(LOG_LEVEL_ERROR, "[TelnetOpenTSDBClient] ERROR attempting to connect to %s:%i. Check that OpenTsdb server is up and running in the web browser: http://%s:%i \n %s", this->m_host.data(), this->m_port, this->m_host.data(), this->m_port, e.what());
		throw e;
	}
}

void TelnetOpenTsdbClient::handle_write(std::string writtenPoint, const boost::system::error_code & error)
{
	if(this->isClosed()) return;

	if(!error)
	{
		// Check if the previous write returned something
		char data[1024]; // should be enough to read all the messages
		if(this->readFromSocketIfAvailable(data, 1024)){
			std::string errorStr = "ERROR writing point: " + writtenPoint + "\n. Telnet connection returned: " + std::string(data);

			// handle the error
			this->handle_write_error(errorStr);
			return;
		}

		// Previous write was successful. Dequeue last element
		this->writeSuccess++;
		this->pendingWrites--;
		//printLog(LOG_LEVEL_IMPORTANT,"Successfully wrote point: %s (Pending Writes: %u ; Successful writes: %u; )\n", writtenPoint.data(), this->pendingWrites.load(), this->writeSuccess.load());

		this->maxQueued = std::max((unsigned int)this->maxQueued, this->pendingWrites.load());
	} else { // handle write error
		this->handle_write_error("write failed: " + std::string(error.message()) + ". Not sent: "+  writtenPoint +  "\n");
		return;
	}
}

void TelnetOpenTsdbClient::handle_write_error(std::string error){
	this->writeErrors++;
	printLog(LOG_LEVEL_ERROR, "[TelnetOpenTsdbClient] %s",error.data() );
	//this->do_close();

	// Forced close.
	this->forceClose();

	// TODO: fail fast to stop the simulation. No reconnection
	//	 m_reconnect_timer.expires_from_now(boost::posix_time::seconds(2));
	//	 m_reconnect_timer.async_wait(
	//	 					boost::bind(&TelnetOpenTsdbClient::start_connect, this));
}

void TelnetOpenTsdbClient::do_close()
{
	if(this->pendingWrites.load() != 0){
		printLog(LOG_LEVEL_IMPORTANT, "[TelnetOpenTSDBClient] There are still pending points to send. Re-queing close. Pending Writes: %u; Successful writes:%u. Write errors: %u. Max Queued: %u ; \n", this->pendingWrites.load(), this->writeSuccess.load(), this->writeErrors.load(), this->maxQueued.load());
		this->m_strand.post(boost::bind(&TelnetOpenTsdbClient::do_close, this));
		return;
	}

	if(!this->isClosed()){
		this->forceClose();
	}
}

void TelnetOpenTsdbClient::forceClose()
{
	boost::system::error_code ignored_ec;
	printLog(LOG_LEVEL_IMPORTANT, "[TelnetOpenTSDBClient] closing socket. Successful writes:%u. Write errors: %u. Max Queued: %u\n", this->writeSuccess.load(), this->writeErrors.load(), this->maxQueued.load());

	this->m_connected = false;
	this->m_socket->close(ignored_ec);
	this->m_reconnect_timer.cancel();
	this->m_work.reset();
	this->m_io_service.stop();
}

/**
 * This is a blocking operation. If the connection does not respond in 3 seconds, it retries once and then throws exception.
 */
std::string TelnetOpenTsdbClient::getVersion(boost::posix_time::time_duration timeout){
	this->m_reconnect_timer.expires_from_now(timeout);

	// request for version
	std::string request = "version\n";
	boost::asio::write(*this->m_socket.get(), boost::asio::buffer(request, request.length()));

	// read response
	printLog(LOG_LEVEL_DEBUG, "[TelnetOpenTSDBClient] reading version response .... \n");
	char response[248];
	boost::system::error_code ec = boost::asio::error::would_block; // ASIO sets the error_code, so any other value different to would_block indicates completion
	//this->m_socket->read_some(boost::asio::buffer(response), error);

	this->m_socket->async_read_some(boost::asio::buffer(response), boost::lambda::var(ec) = boost::lambda::_1);

	// Block until the asynchronous operation has completed.
	do this->m_io_service.run_one(); while (ec == boost::asio::error::would_block && this->m_reconnect_timer.expires_at() != boost::posix_time::pos_infin);

	// Check if we finished successfully.
	if (ec) {
		printLog(LOG_LEVEL_ERROR, "[TelnetOpenTSDBClient] reading version TIMEOUT. Check OpentTSDB server is responsive. Can you issue a 'version' command from telnet %s %i ?  \n", this->m_host.data(), this->m_port);
		this->forceClose();
		throw boost::system::system_error(ec);
	}

	// set timer back to INF
	this->m_reconnect_timer.expires_at(boost::posix_time::pos_infin);

	return std::string(response);
}

void TelnetOpenTsdbClient::check_deadline()
{
	// Check whether the deadline has passed. We compare the deadline against
	// the current time since a new asynchronous operation may have moved the
	// deadline before this actor had a chance to run.
	//printLog(LOG_LEVEL_ERROR, "[TelnetOpenTSDBClient] Checking timer \n");
	if (this->m_reconnect_timer.expires_at() <= boost::asio::deadline_timer::traits_type::now())
	{
		printLog(LOG_LEVEL_ERROR, "[TelnetOpenTSDBClient] Timer Expired, closing connections \n");
		// The deadline has passed. The socket is closed so that any outstanding
		// asynchronous operations are cancelled. This allows the blocked async_read_some functions to return.
		boost::system::error_code ignored_ec;
		this->m_socket->close(ignored_ec);
		this->forceClose();

		// There is no longer an active deadline. The expiry is set to positive
		// infinity so that the actor takes no action until a new deadline is set.
		this->m_reconnect_timer.expires_at(boost::posix_time::pos_infin);
	}

	// Put the actor back to sleep.
	this->m_reconnect_timer.async_wait(boost::bind(&TelnetOpenTsdbClient::check_deadline, this));
}

bool TelnetOpenTsdbClient::readFromSocketIfAvailable(char* data, unsigned int size){
	// Check if there is something to read. TODO: might be possible to use this->m_socket->available() too.
	boost::asio::socket_base::bytes_readable command(true);
	this->m_socket->io_control(command);
	std::size_t readableBytes = command.get();

	//printLog(LOG_LEVEL_DEBUG, "[TelnetOpenTSDBClient] ReadableBytes=%i \n", readableBytes);
	if(readableBytes > 0){
		boost::system::error_code errorCode;

		// read the error from the socket
		auto buff = boost::asio::buffer(data, size);
		this->m_socket->read_some(buff, errorCode);

		return true;
	}

	return false;
}
