#include "FunctionalTesterModel.h"

void FunctionalTesterModel::init(double, ...) {
	this->sigma = std::numeric_limits<double>::infinity(); // it is changed if events are set
}

void FunctionalTesterModel::setEventsToGenerate(std::vector<std::pair<double, Event>> events){
	this->eventsToGenerate = events;
	if(this->eventsToGenerate.size() <=0) return;

	// sort events by time
	std::sort(this->eventsToGenerate.begin(), this->eventsToGenerate.end(),
		    [](const std::pair<double, Event> & a, const std::pair<double, Event> & b) -> bool
		{
		    return a.first < b.first;
		});

	this->sigma = this->eventsToGenerate[0].first;
}

std::vector<std::pair<double, Event>> FunctionalTesterModel::getEventsReceived(){
	return this->eventsReceived;
}

double FunctionalTesterModel::ta(double t) {
	return this->sigma;
}

void FunctionalTesterModel::dint(double t) {
	// an event was just sent, remove it
	assert(!this->eventsToGenerate.empty());
	this->eventsToGenerate.erase(this->eventsToGenerate.begin());

	// set sigma
	this->sigma = std::numeric_limits<double>::infinity();
	if(!this->eventsToGenerate.empty()){
		this->sigma = this->eventsToGenerate[0].first - t;
	}
}

void FunctionalTesterModel::dext(Event ev, double t){
	this->eventsReceived.push_back({t, ev });
}

Event FunctionalTesterModel::lambda(double){
	return this->eventsToGenerate[0].second;
}

void FunctionalTesterModel::exit(double t) {

}
