#pragma once
#include "common.hpp"

struct KeyActionMode
{
    int key;
    int action;
    int modifier;
};

u32 hashInput(int k, int a, int m);
u32 hashInput(KeyActionMode keys);
std::pair<std::string, std::string> splitToFunctionAndKeys(const std::string &str);
std::vector<std::string> splitToKeys(std::string str);
KeyActionMode parseKeyBinding(const std::string &str);

const int LMB = 501;
const int MMB = 502;
const int RMB = 503;
const int SCROLL_UP = 504;
const int SCROLL_DOWN = 505;
const int MousePosition = 507;
const int MouseMove = 508;
const int HOLD_KEY = 666;
const int LT = 666;
const int RT = 666;

/// TODO: better container?
extern std::map<std::string, int> stringToKey;

extern std::map<std::string, int> stringToMod;
