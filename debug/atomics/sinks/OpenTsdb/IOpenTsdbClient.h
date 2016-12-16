/*
 * IOpenTsdbClient.h
 *
 *  Created on: Jun 3, 2015
 *      Author: mbonaven
 */

#ifndef OPENTSDB_IOPENTSDBCLIENT_H_
#define OPENTSDB_IOPENTSDBCLIENT_H_


/**
 * Abstract class for openTSDB clients
 */
class IOpenTsdbClient {
public:
	typedef std::map<std::string, std::string> TagsType;

	virtual void run()=0;

	virtual void add_point(const std::string & metric, unsigned long timestamp, const double & value, const TagsType * const ptags)=0;

	virtual void close()=0;
	virtual bool isClosed() =0;

	virtual ~IOpenTsdbClient() { }
};

#endif /* MATIAS_IOPENTSDBCLIENT_H_ */
