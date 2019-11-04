#include "core.hpp"
#include "TimeEvents.hpp"

namespace
{
std::list<int> initList() {
    std::list<int> out;

    for(int i = 0; i < 5000; i++) { out.push_back(i); }

    return out;
}
std::list<int> freeIdList = initList();
int nextFreeId() {
    auto out = freeIdList.front();
    freeIdList.pop_front();
    return out;
}

struct TimeEventHandler
{
    u64 expiryTime;
    u64 timeStep;
    int id;
    timeFuctionSignature callback;
};

std::list<TimeEventHandler> intervalList;
std::list<TimeEventHandler> timeoutList;

}
void updateTimers(u64 msTime) {
    intervalList.remove_if([msTime](TimeEventHandler& it) {
        if(it.expiryTime <= msTime) {
            it.expiryTime += it.timeStep;
            it.callback();
        }
        return false;
    });

    timeoutList.remove_if([msTime](TimeEventHandler& it) {
        if(it.expiryTime <= msTime) {
            it.callback();
            freeIdList.push_back(it.id);
            return true;
        }
        return false;
    });
}

int setInterval(u64 ms, timeFuctionSignature callback) {
    int id = nextFreeId();

    // intervalList.push_back({
    //         getCurrentTime() + ms,
    //         ms,
    //         id,
    //         callback
    //     });
    return id;
}
int setTimeout(u64 ms, timeFuctionSignature callback) {
    int id = nextFreeId();

    // timeoutList.push_back({
    //         getCurrentTime() + ms,
    //         0,
    //         id,
    //         callback
    //     });

    return id;
}

void cancelTimer(int id) {
    intervalList.remove_if([id](TimeEventHandler& t) { return id == t.id; });
    timeoutList.remove_if([id](TimeEventHandler& t) { return id == t.id; });
}
