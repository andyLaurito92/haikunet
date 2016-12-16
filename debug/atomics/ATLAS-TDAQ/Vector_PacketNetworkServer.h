//CPP:ATLAS-TDAQ/Vector_PacketNetworkServer.cpp
//CPP:ATLAS-TDAQ/VectorialModel.cpp
//CPP:ATLAS-TDAQ/PacketNetworkServer.cpp
#if !defined Vector_PacketNetworkServer_h
#define Vector_PacketNetworkServer_h

#include "simulator.h"
#include "event.h"
#include "stdarg.h"

#include "engine.h"
#include "qss/qsstools.h"
#include "VectorialEvent.h"
#include "PacketNetworkServer.h"
#include "VectorialModel.h"

class Vector_PacketNetworkServer: public VectorialModel<PacketNetworkServer> {
public:
	Vector_PacketNetworkServer(const char *n): VectorialModel<PacketNetworkServer>(n) {};
	~Vector_PacketNetworkServer() { }

	int getParameterCount() { return 2; };
};

#endif
