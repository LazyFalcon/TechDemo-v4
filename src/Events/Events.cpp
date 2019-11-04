#include "core.hpp"
#include "Events.hpp"
#include "App.hpp"
#include "EventProcessor.hpp"
#include "PerfTimers.hpp"
#include "Window.hpp"

void addEvent(std::shared_ptr<Event> e) {
    EventProcessor::push(e);
}
