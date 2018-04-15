#include "Events.hpp"
#include "EventProcessor.hpp"
#include "App.hpp"
#include "Window.hpp"
#include "PerfTimers.hpp"

void addEvent(std::shared_ptr<Event> e){
    EventProcessor::push(e);
}
