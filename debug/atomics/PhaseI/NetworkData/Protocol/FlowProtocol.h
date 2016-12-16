#if !defined FlowProtocol_H
#define FlowProtocol_H

#include <stdlib.h>
#include <string>

#include "IProtocol.h"

class FlowProtocol : public IProtocol {
	std::string flowId;

public:

	FlowProtocol(std::string flowId) : flowId(flowId){
	}

	std::string getFlowId() { return this->flowId; }

	int getSize_bits() { return 0; }
	int getLayer() { return LAYER_ROUTING;}
	int getId() { return PROTOCOL_ID_FLOW; }
};


#endif
