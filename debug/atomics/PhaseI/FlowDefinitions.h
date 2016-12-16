//CPP:PhaseI/FlowDefinitions.cpp

#ifndef FLOWDEFINITIONS_H_
#define FLOWDEFINITIONS_H_

#include <memory>
#include <stdlib.h>
#include <deque>

#include "NetworkData/Flow.h"
#include "NetworkData/Route.h"

#include "NetworkData/GlobalFlowBasedRoutingTable.h"

class FlowDefinitions {

private:
	static std::multimap<std::string, std::shared_ptr<Flow>> Flows; // TODO: use a multimap to allow for several flow from a single server

	static void defineFlows(); // add here your flows
public:

	static void registerFlowSourceNode(std::shared_ptr<Flow> flow, std::string startingNode){
		Flows.insert({startingNode, flow});
	}

	static void registerFlowRoute(std::shared_ptr<Flow> flow, std::shared_ptr<Route> route ){
		GlobalFlowBasedRoutingTable::addFlowRouteEntry(flow->getFlowId(), route); // Add the route to the routing table
	}

	static void emptyFlowsDefinition(){
		Flows.clear();
	}

	static std::vector<std::shared_ptr<Flow>> getSourceFlows(std::string sourceName){
		if(Flows.empty()) {// call just once (unless it does not define any flows...)
			defineFlows();
		}

		std::vector<std::shared_ptr<Flow>> ret; // TODO: is there a way to avoid creating the vector and coping it around?

		auto range =  Flows.equal_range(sourceName);
		for (auto it = range.first; it != range.second; ++it){ // TODO: is there a way to avoid iteration (returning a multimap iterator is not nice)
			ret.push_back(it->second);
		}

		return ret;
	}
};



#endif /* FLOWDEFINITIONS_H_ */
