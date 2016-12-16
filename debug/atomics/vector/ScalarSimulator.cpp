#include "ScalarSimulator.h"

ScalarSimulator::ScalarSimulator(const char *n) : scalarIndex(-1) {
	this->scalarName = n;
	this->scalarName.erase(this->scalarName.begin() + this->scalarName.rfind("_"), this->scalarName.end()); // delete the last part of the name that contains the "_<index>". see VectorialModel.h::85
};

int ScalarSimulator::getScalarIndex() {
	return this->scalarIndex;
}

std::string ScalarSimulator::getScalarName(){
	return this->scalarName;
};

