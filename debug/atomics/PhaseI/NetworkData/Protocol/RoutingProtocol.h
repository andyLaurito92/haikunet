#if !defined RoutingProtocol_H
#define RoutingProtocol_H

#include <stdlib.h>
#include <string>

#include "IProtocol.h"
#include "PhaseI/NetworkData/Route.h"

class RoutingProtocol : public IProtocol {
	std::shared_ptr<Route> route;

public:

	RoutingProtocol(std::shared_ptr<Route> route) : route(route){
	}

	RoutingProtocol(std::deque<Route::Node> route) : route(std::make_shared<Route>(std::move(route))){
	}

	int getSize_bits() { return 0; }
	int getLayer() { return LAYER_ROUTING;}
	int getId() { return PROTOCOL_ID_ABSTRACT_ROUTING; }

	std::shared_ptr<Route> getRoute() { return this->route; }

	void printInfo(int level){
		IProtocol::printInfo(level);

		printLog(level, "\t Route has %u jumps \n", this->route->jumpCount());
	}

};


#endif
