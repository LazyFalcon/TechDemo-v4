#include "Events.hpp"
#include "EventProcessor.hpp"

std::list<std::shared_ptr<Event>> EventProcessor::listOfEvents;

void EventProcessor::push(std::shared_ptr<Event> &e){
    listOfEvents.emplace_back(e);
}

void EventProcessor::process(){
    listOfEvents.remove_if([this](const std::shared_ptr<Event> &ev){
        return ev->handle(this->app);
    });
}
