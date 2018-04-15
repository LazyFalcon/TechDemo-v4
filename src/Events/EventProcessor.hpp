#pragma once
#include <list>
#include <memory>

struct App;
struct Event;

class EventProcessor
{
private:
    static std::list<std::shared_ptr<Event>> listOfEvents;
    App &app;
public:
    EventProcessor(App &app) : app(app){}
    static void push(std::shared_ptr<Event> &e);
    void process();
};
