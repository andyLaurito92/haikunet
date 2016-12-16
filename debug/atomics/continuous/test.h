#ifndef CLASSA
#define CLASSA
#include <stdio.h>
#include "simulator.h"
#include "event.h"
#include "memory"

class A {
  public:
  A(int tt):t(tt) { printLog("Creando un A %d\n",tt); }
  ~A() { printLog("Liberando un A %d\n",t); }
  int t;
};

class EventPtr : public Event {
public:
	std::shared_ptr<void> valuePtr;

	EventPtr(std::shared_ptr<void> val, int port) : Event(val.get(), port){
		valuePtr = val;
		printLog("valuePtr asignado \n");

	}

	~EventPtr(){ printLog("Liberando un EventPtr \n"); }


};

#endif
