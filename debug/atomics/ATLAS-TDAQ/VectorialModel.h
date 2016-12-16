//CPP:ATLAS-TDAQ/VectorialModel.cpp


/*
 * VectorialModel.h
 *
 *  Created on: Jul 1, 2015
 *      Author: mbonaven
 */

#ifndef ATOMICS_ATLAS_TDAQ_IVECTORIALMODEL_H_
#define ATOMICS_ATLAS_TDAQ_IVECTORIALMODEL_H_

#include "simulator.h"
#include "event.h"
#include "stdarg.h"
#include "engine.h"

#include "VectorialEvent.h"
#include "ScalarSimulator.h"
#include "VectorialModel.h"

#include "sinks/ParameterReader.h"

#include <boost/static_assert.hpp>

template<typename T>
class VectorialModel : public Simulator{
	BOOST_STATIC_ASSERT((std::is_base_of<Simulator, T>::value));

protected:
	Coupling* D0;
	std::shared_ptr<VectorialEvent> vec; // reference to last sent vectorial event. TODO: use unique_prt
	Event y;
	int scalarInstances;

	virtual int getParameterCount() =0;

	T* createScalarInstance(char* scalarModelName){
		return new T(scalarModelName);
	}

public:
	VectorialModel(const char *n): Simulator(n) {};
	virtual ~VectorialModel() { }


	/************ Implementation of Simulator methods  ************/
	void init(double t,...) {
		//The 'parameters' variable contains the parameters transferred from the editor.
		va_list parameters;
		va_start(parameters,t);

	    printLog(LOG_LEVEL_FULL_LOGGING, "[%f] %s_init: Model accept the following %u parameters:  \n", t, this->getName(), this->getParameterCount());

		// Get all scalar parameters names
		std::vector<std::string> scalarParamNames;
		for (int i = 0; i < this->getParameterCount(); i++) {
			scalarParamNames.push_back(va_arg(parameters, char*));
		}

		// read from the params the amount of scalar instances to be created
		char *fvar = va_arg(parameters,char*);
		scalarInstances = readDefaultParameterValue<int>(fvar);/*getScilabVar(fvar, true);*/

		printLog(LOG_LEVEL_INIT, "[%f] %s_init: Number of instances %i  \n", t, this->getName(), scalarInstances);


        // read scalar parameters, each as a vector (where each element of the vector will be the parameter for scalar model i)
		std::vector<std::vector<double>> scalarParameters;
		for (int i = 0; i < this->getParameterCount(); i++) {
			scalarParameters.push_back(readDefaultParameterValue<std::vector<double> >(scalarParamNames[i].data()));
			printLog(LOG_LEVEL_INIT, "[%f] %s_init : Parameter %i=%s is a vector with %i values  \n", t, this->getName(), i, scalarParamNames[i].data(), scalarParameters[i].size());
		}

		// create a coupled model that will contain all scalar model
		D0 = new Coupling("CoupledHyst");
		Simulator **D1 = new Simulator*[scalarInstances];
		Connection **EIC1 = new Connection*[0];
		Connection **EOC1 = new Connection*[0];
		Connection **IC1 = new Connection*[0];

		// create scalar models
		char* childi;
		char* pars[this->getParameterCount()];
		for (int i = 0; i < scalarInstances; i++) {
			childi = new char[24];
			sprintf(childi, "%s_%i", this->getName(), i);
			D1[i] = this->createScalarInstance(childi); //new DcmApp(childi);

			// If is inherits from ScalarSimulator, set its scalarIndex
			if (ScalarSimulator* scalarModel = dynamic_cast<ScalarSimulator*>(D1[i])) {
				scalarModel->scalarIndex = i;
			}

			// get the parameters to pass to the scalar model
			for (uint j = 0; j < this->getParameterCount(); j++) {
				if (i < scalarParameters[j].size()) {
					pars[j] = new char[24]; // save memory for the parameter name.
					sprintf(pars[j], "%1.16g", scalarParameters.at(j).at(i)); // NOTE: we are doing this convertion because it is not supported to read parameters as strings from scilab
					printLog(LOG_LEVEL_FULL_LOGGING, "[%f] %s_init : Parameter %i for model %i =%s \n", t, this->getName(), j, i, pars[j]);
				}
			}

			// init the scalar model with corresponding params
			D1[i]->init(t, pars[0], pars[1], pars[2], pars[3]);
		}

		// setup and init coupled model
		D0->setup(D1, scalarInstances, IC1, 0, EIC1, 0, EOC1, 0);
		D0->init(t);
	}

	double ta(double t) {
		//This function returns a double.
		return D0->ta(t);
	}

	void dint(double t) {
		D0->dint(t);
	}

	void dext(Event x, double t) {
		auto vec1 = std::static_pointer_cast<VectorialEvent> (x.valuePtr);
		int index = vec1->index;

		printLog(LOG_LEVEL_FULL_LOGGING, "[%f] %s: received event with index %i\n", t, this->getName(), index);

		// Set the scalar value to the event before forwarding
		x.valuePtr = vec1->valuePtr;

		if ((index>-1)&&(index<scalarInstances)){
			D0->D[index]->dextmessage(x,t);
			D0->heap.update(index);
		} else if (index==-1) {
			for (int ind=0;ind<scalarInstances;ind++){
				D0->D[ind]->dextmessage(x,t);
				D0->heap.update(ind);
			}
		}
	}

	Event lambda(double t) {
		//This function returns an Event:
		//     Event(%&Value%, %NroPort%)
		//where:
		//     %&Value% points to the variable which contains the value.
		//     %NroPort% is the port number (from 0 to n-1)

		y= D0->D[D0->transitionChild]->lambdamessage(t);

		vec = std::make_shared<VectorialEvent>();
		vec->valuePtr = y.valuePtr;
		vec->index=D0->transitionChild;

		y.valuePtr = vec;

		printLog(LOG_LEVEL_FULL_LOGGING, "[%f] %s: Forwarding Vectorial Event message using index %u and port %u \n", t, this->getName(), vec->index, y.port);
		return y;
	}

	void exit(double t) {
		D0->exit(t);
	}
};

#endif /* ATOMICS_ATLAS_TDAQ_IVECTORIALMODEL_H_ */
