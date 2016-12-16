#if !defined FelixAppProtocol_H
#define FelixAppProtocol_H

#include <stdlib.h>
#include <string>

#include "IProtocol.h"

class FelixAppProtocol : public IProtocol {
	int size_bits = 0;

public:

	FelixAppProtocol(int size_bits) : size_bits(size_bits){

	}

	int getSize_bits() { return size_bits; } ;
	int getLayer() { return LAYER_APPLICATION;};
	int getId() { return PROTOCOL_ID_FELIX_APP; };
};


#endif
