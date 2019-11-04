#pragma once
#include "Types.hpp"

using timeFuctionSignature = std::function<void(void)>;
int setInterval(u64 ms, timeFuctionSignature callback);
int setTimeout(u64 ms, timeFuctionSignature callback);
void updateTimers(u64 msTime);
void cancelTimer(int id);
