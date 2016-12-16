#if !defined Route_H
#define Route_H

#include <stdlib.h>
#include <string>
#include <list>
#include <map>

#include "sinks/ParameterReader.h"

/**
 * Represents a path that packets take in order to go from a source node (eJ: a Server) to a destination node (ej: aPacketSink), traversing other network nodes (Ej: Routers).
 * This is class is used by the Router model to decide which outgoing port to send incoming packets.
 */
class Route {
public:
	struct Node{
		int outgoingPort;
		std::string nodeName;// this is now just for verification. TODO: can create routes only with names and automatically fill outgoing ports
	};

private:
	std::deque<Node> routePath;

public:

	Route(std::deque<Node> routePath) : routePath(routePath) {
	}

	~Route() {
	}

	Node getNextJump(){
		if(this->routePath.empty()){ // do not fail, but issue a faked node
			return { -1, "Empty route, no more jumps"};
		}

		return this->routePath.front();
    }

	void popJump(){
		if(this->routePath.empty()){
			printLog(LOG_LEVEL_ERROR, "Route::popJump: Attempted to remove a jump from an empty route. \n");
			throw std::runtime_error("Route::popJump: Attempted to remove a jump from an empty route. \n");
		}

		this->routePath.pop_front();
	}

	int jumpCount(){
		return this->routePath.size();
	}

	std::deque<Node> getCompletePath() { return this->routePath; }

};


#endif
