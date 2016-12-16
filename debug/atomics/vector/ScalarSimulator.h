//CPP:vector/ScalarSimulator.cpp
#if !defined ScalarSimulator_h
#define ScalarSimulator_h

#include "simulator.h"
#include "event.h"
#include "stdarg.h"

class ScalarSimulator {
public:
		int scalarIndex;
		std::string scalarName;

		ScalarSimulator(const char *n);
		int getScalarIndex();
		std::string getScalarName();
};
#endif
