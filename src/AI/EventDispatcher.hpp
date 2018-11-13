#pragma once
#include <any>
#include <map>

enum class AiEvent
{
    Activate, Deactivate, Destroy, Select, Unselect
};

using EventHnadler = std::function<void()>;
using EventHnadlerWithPayload = std::function<void(std::any&)>;

class EventDispatcher
{
private:
    std::multimap<int, EventHnadler> m_handlers;
    std::multimap<int, EventHnadlerWithPayload> m_handlersWithPayload;
public:
    void handleEvent(AiEvent event){
        auto range = m_handlers.equal_range(static_cast<int>(event));
        for(auto &it : range) (*it)();
    }
    void handleEvent(AiEvent event, std::any&& payload){
        auto range = m_handlersWithPayload.equal_range(static_cast<int>(event));
        for(auto &it : range) (*it)(payload);
    }
    template<typename Func>
    void onId(AiEvent id, Func&& func){
        m_handlers.emplace(id, func);
    }

};
