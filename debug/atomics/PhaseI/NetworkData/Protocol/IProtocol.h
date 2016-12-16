#if !defined IProtocol_H
#define IProtocol_H

#include <stdlib.h>
#include <string>

#include "sinks/ParameterReader.h"

// Known implemented Protocol IDs (classes that inherit from Protocol)
#define PROTOCOL_ID_UNDEFINED -1
#define PROTOCOL_ID_FELIX_APP 100        	// FelixAppProtocol.h
#define PROTOCOL_ID_ABSTRACT_ROUTING 200 	// RoutingProtocol.h
#define PROTOCOL_ID_QUALITY_OF_SERVICE 300 		// QualityOfServiceProtocol.h
#define PROTOCOL_ID_FLOW 400 	// FlowProtocol.h

// Layer IDS
#define LAYER_APPLICATION 7
#define LAYER_ROUTING 3

class IProtocol {
public:
 virtual ~IProtocol() {}

 virtual int getSize_bits()=0;
 virtual int getLayer()=0;
 virtual int getId()=0;


 void printInfo(int level){
 	printLog(level, "\t Protocol ID= %u \n", this->getId());
 	printLog(level, "\t Protocol layer= %u \n", this->getLayer());
 	printLog(level, "\t Protocol size= %u (bits) \n", this->getSize_bits());
 }


};


#endif
