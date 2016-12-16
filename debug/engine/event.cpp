/****************************************************************************
**
**  Copyright (C) 2009 Facultad de Ciencia Exactas Ingeniería y Agrimensura
**		       Universidad Nacional de Rosario - Argentina.
**  Contact: PowerDEVS Information (kofman@fceia.unr.edu.ar, fbergero@fceia.unr.edu.ar)
**
**  This file is part of PowerDEVS.
**
**  PowerDEVS is free software: you can redistribute it and/or modify
**  it under the terms of the GNU General Public License as published by
**  the Free Software Foundation, either version 3 of the License, or
**  (at your option) any later version.
**
**  PowerDEVS is distributed in the hope that it will be useful,
**  but WITHOUT ANY WARRANTY; without even the implied warranty of
**  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
**  GNU General Public License for more details.
**
**  You should have received a copy of the GNU General Public License
**  along with PowerDEVS.  If not, see <http://www.gnu.org/licenses/>.
**
****************************************************************************/

#include "types.h"
#include "event.h"
#include "pdevslib.h"

Event::Event()
{
	value = 0;
	mode = NOREALTIME;
	interrupted = 0;
	port = 0;
}

Event::~Event()
{

}

Event::Event( void* val, Port p) : Event(std::shared_ptr<void>(val, [](void*){}), p){
	// a shared_ptr is created but does not manage memory de-allocation. This is because as a pointer is received as parameter, so the caller already reserved memory and should manage it. But the value of valuePtr is consistent with value
};

Event::Event( std::shared_ptr<void> valPtr, Port p){
	valuePtr=valPtr;
	value=valPtr.get();
	mode = NOREALTIME;
	interrupted = 0;
	port=p;

};

//Event::Event( void* val, Port p){
//	value=val;
//	mode = NOREALTIME;
//	interrupted = 0;
//	port=p;
//	printLog(LOG_LEVEL_DEBUG, "Even(void, int) \n");
//};
//
//Event::Event( std::shared_ptr<void> valPtr, Port p) : Event(valPtr.get(), p){
//	valuePtr=valPtr;
//	printLog(LOG_LEVEL_DEBUG, "Even(sharedPtr, int) \n");
//};

void Event::setNullEvent(){
	value=0;
	port=0;
};

bool Event::isNotNull(){
  bool state;
  if (value==0) {state=false;} else {state=true;};
  return state;
};
