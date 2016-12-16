#ifndef GlobalFlowBasedRoutingTable_H_
#define GlobalFlowBasedRoutingTable_H_

#include <stdlib.h>
#include <string>

#include "Route.h"

#include "sinks/ParameterReader.h"


/**
 *  GlobalFlowBasedRoutingTable defines the routing tables for all devices in the network (global). This routing table is flow-based (not port-based): based on the flowId the outgoing port is decided.
 *  A given flow (identified by ID) and device (deviceName), it can be associated to multiple outports, which allows a single flow to traverse the network through different paths. It is up to the routing algorithm (Router.cpp atomic model) to decide which outport to use (this RoutingTable just returns all the possible outports)
 *  Entries can be added one by one, or by specifying the complete flow path.
 *
 */
class GlobalFlowBasedRoutingTable {
private:
	struct TableKey {
			std::string flowId;
			std::string nodeName;
			std::string compareValue; // to avoid concatenating strings in every comparison

			TableKey(std::string flowId, std::string nodeName) : flowId(flowId), nodeName(nodeName), compareValue(flowId + nodeName){

			}

	};

	struct TableKeyComparator {
	public:
	    bool operator()(const TableKey& a, const TableKey& b) const {
	    	bool result = a.compareValue.compare(b.compareValue) > 0;

//	    	if(result)
//	    		printLog(0, " '%s'(%u) goes before '%s'(%u) \n", a.compareValue.data(), &a,  b.compareValue.data(), &b);
//	    	else
//	    		printLog(0, " '%s'(%u) goes before '%s'(%u) \n", b.compareValue.data(), &b,  a.compareValue.data(), &a);

	        return result;
	    }
	};

	struct TableValue {
			int outPort;
	};

	// TODO: Multimaps are typically implemented as binary search trees. ¿Is it slow indexing with 2 strings?
	static std::multimap<TableKey, TableValue, TableKeyComparator> routingTable; // multimap allows multiple outports per flow

public:

	/* Used mainly in tests */
	static void emptyTable(){
		routingTable.clear();
	}

	static void addEntry(std::string flowId, std::string nodeName, int outPort){
		TableKey key = TableKey {flowId, nodeName};
		TableValue value = TableValue { outPort };

		// check that it does not already exist
		auto it = routingTable.find(key);
		if(it != routingTable.end() && it->second.outPort == outPort) return; // <key, value> already exists so we don't add it again

		// add to the table
		routingTable.insert({key, value});

		printTable(LOG_LEVEL_DEBUG);
	}

	static void addFlowRouteEntry(std::string flowId, std::shared_ptr<Route> route){
		// add each entry of the path
		for(auto node : route->getCompletePath()){
			addEntry(flowId, node.nodeName, node.outgoingPort);
		}
	}

	static std::vector<int> getOutPorts(std::string flowId, std::string nodeName){
		TableKey key = TableKey {flowId, nodeName};

		auto outPorts = std::vector<int>();

		printLog(LOG_LEVEL_DEBUG, "Trying to find key '%s'(%u) \n", key.compareValue.data(), &key);
		printTable(LOG_LEVEL_DEBUG);

		auto it_range = routingTable.equal_range(key);
		for (auto it = it_range.first; it != it_range.second; ++it){
			outPorts.push_back(it->second.outPort);
		}

		printLog(LOG_LEVEL_DEBUG, "Found %u keys \n", outPorts.size());
		return outPorts;
	}

	static void printTable(int logLevel){
		int size = routingTable.size();

		printLog(logLevel, "Table entries %d asdfasdf \n", size);
		for(auto it : routingTable){
			printLog(logLevel, "Key: '%s' => %u \n", it.first.compareValue.data(), it.second.outPort );
		}
	}
};


#endif
