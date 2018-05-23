#pragma once
#include "common.hpp"

struct KeyActionModifier
{
    int key;
    int action;
    int modifier;
};

u32 hashInput(int k, int a, int m);
u32 hashInput(KeyActionModifier keys);
std::pair<std::string, std::string> splitToFunctionAndKeys(const std::string& str);
std::vector<std::string> splitToKeys(std::string str);
KeyActionModifier parseKeyBinding(const std::string& str);
std::string convertKeyToString(int k, int a, int m);

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
