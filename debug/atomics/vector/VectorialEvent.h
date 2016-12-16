#ifndef VECTORIAL_EVENT_H
#define VECTORIAL_EVENT_H

class VectorialEvent
{
public:
	VectorialEvent() : index(-1) {

	}

	VectorialEvent(void* val, int index) : VectorialEvent(std::shared_ptr<void>(val, [](void*){}), index) {

	}

	VectorialEvent(std::shared_ptr<void> value, int index) : valuePtr(value), index(index){

	}



	std::shared_ptr<void> valuePtr;
	int index;

	//	VectorialEvent() {
	//		printLog(LOG_LEVEL_FULL_LOGGING, "Vectorial Event created (#%i) \n", this);
	//	}
	//
	//	~VectorialEvent() {
	//			printLog(LOG_LEVEL_FULL_LOGGING, "Vectorial Event destroyed (#%i) \n", this);
	//	}
};

#endif //VECTORIAL_EVENT_H
