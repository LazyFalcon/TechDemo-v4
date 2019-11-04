#pragma once
#include "BaseStructs.hpp"

class App;
struct Event;

struct Event
{
    virtual ~Event() = default;
    virtual bool handle(App& app) = 0;
};
void addEvent(std::shared_ptr<Event> e);

template<typename EventType, typename... Args>
std::shared_ptr<EventType> event(const Args&... args) {
    auto out = std::make_shared<EventType>(args...);
    addEvent(out);
    return out;
}
