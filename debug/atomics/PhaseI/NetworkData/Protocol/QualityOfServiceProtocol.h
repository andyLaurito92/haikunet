#if !defined QualityOfServiceProtocol_H
#define QualityOfServiceProtocol_H

#include <stdlib.h>
#include <string>

#include "IProtocol.h"

class QualityOfServiceProtocol : public IProtocol {
	int typeOfService;

public:

	QualityOfServiceProtocol(int typeOfService) : typeOfService(typeOfService) {

	}

	int getSize_bits() { return 0; } ;
	int getLayer() { return LAYER_ROUTING;};
	int getId() { return PROTOCOL_ID_QUALITY_OF_SERVICE; };

	int getTypeOfService() { return this->typeOfService; };
};


#endif
