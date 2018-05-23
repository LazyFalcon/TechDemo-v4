#include <GLFW/glfw3.h>
#include "input-utils.hpp"
#include "common.hpp"
#include "Logging.hpp"

std::map<std::string, int> stringToKey = {
    { "space", GLFW_KEY_SPACE },
    { "esc", GLFW_KEY_ESCAPE },
    { "enter", GLFW_KEY_ENTER },
    { "tab", GLFW_KEY_TAB },
    { "backspace", GLFW_KEY_BACKSPACE },
    { "insert", GLFW_KEY_INSERT },
    { "delete", GLFW_KEY_DELETE },
    { "right", GLFW_KEY_RIGHT },
    { "left", GLFW_KEY_LEFT },
    { "down", GLFW_KEY_DOWN },
    { "up", GLFW_KEY_UP },
    { "pageUp", GLFW_KEY_PAGE_UP },
    { "pageDown", GLFW_KEY_PAGE_DOWN },
    { "home", GLFW_KEY_HOME },
    { "end", GLFW_KEY_END },
    { "capsLock", GLFW_KEY_CAPS_LOCK },
    { "scrollLock", GLFW_KEY_SCROLL_LOCK },
    { "printScreen", GLFW_KEY_PRINT_SCREEN },
    { "pause", GLFW_KEY_PAUSE },
    { "f1", GLFW_KEY_F1 },
    { "f2", GLFW_KEY_F2 },
    { "f3", GLFW_KEY_F3 },
    { "f4", GLFW_KEY_F4 },
    { "f5", GLFW_KEY_F5 },
    { "f6", GLFW_KEY_F6 },
    { "f7", GLFW_KEY_F7 },
    { "f8", GLFW_KEY_F8 },
    { "f9", GLFW_KEY_F9 },
    { "f10", GLFW_KEY_F10 },
    { "f11", GLFW_KEY_F11 },
    { "f12", GLFW_KEY_F12 },
    { "menu", GLFW_KEY_MENU },
    { "shift", GLFW_KEY_LEFT_SHIFT },
    { "ctrl", GLFW_KEY_LEFT_CONTROL },
    { "alt", GLFW_KEY_LEFT_ALT },
    { "super", GLFW_KEY_LEFT_SUPER },
    { "hold", HOLD_KEY},
    { "scrollUp", SCROLL_UP},
    { "scrollDown", SCROLL_DOWN},
    { "LMB", LMB},
    { "MMB", MMB},
    { "RMB", RMB},
    { "minus", int('-')},
    { "LT", LT},
    { "RT", RT},
    { "MousePosition", MousePosition},
    { "MouseMove", MouseMove},
};

std::map<int, std::string> keyToString  = {
    { GLFW_KEY_SPACE, "space" },
    { GLFW_KEY_ESCAPE, "esc" },
    { GLFW_KEY_ENTER, "enter" },
    { GLFW_KEY_TAB, "tab" },
    { GLFW_KEY_BACKSPACE, "backspace" },
    { GLFW_KEY_INSERT, "insert" },
    { GLFW_KEY_DELETE, "delete" },
    { GLFW_KEY_RIGHT, "right" },
    { GLFW_KEY_LEFT, "left" },
    { GLFW_KEY_DOWN, "down" },
    { GLFW_KEY_UP, "up" },
    { GLFW_KEY_PAGE_UP, "pageUp" },
    { GLFW_KEY_PAGE_DOWN, "pageDown" },
    { GLFW_KEY_HOME, "home" },
    { GLFW_KEY_END, "end" },
    { GLFW_KEY_CAPS_LOCK, "capsLock" },
    { GLFW_KEY_SCROLL_LOCK, "scrollLock" },
    { GLFW_KEY_PRINT_SCREEN, "printScreen" },
    { GLFW_KEY_PAUSE, "pause" },
    { GLFW_KEY_F1, "f1" },
    { GLFW_KEY_F2, "f2" },
    { GLFW_KEY_F3, "f3" },
    { GLFW_KEY_F4, "f4" },
    { GLFW_KEY_F5, "f5" },
    { GLFW_KEY_F6, "f6" },
    { GLFW_KEY_F7, "f7" },
    { GLFW_KEY_F8, "f8" },
    { GLFW_KEY_F9, "f9" },
    { GLFW_KEY_F10, "f10" },
    { GLFW_KEY_F11, "f11" },
    { GLFW_KEY_F12, "f12" },
    { GLFW_KEY_MENU, "menu" },
    { GLFW_KEY_LEFT_SHIFT, "shift" },
    { GLFW_KEY_LEFT_CONTROL, "ctrl" },
    { GLFW_KEY_LEFT_ALT, "alt" },
    { GLFW_KEY_LEFT_SUPER, "super" },
    { HOLD_KEY, "hold"},
    { SCROLL_UP, "scrollUp"},
    { SCROLL_DOWN, "scrollDown"},
    { LMB, "LMB"},
    { MMB, "MMB"},
    { RMB, "RMB"},
    { LT, "LT"},
    { RT, "RT"},
    { MousePosition, "MousePosition"},
    { MouseMove, "MouseMove"},
    { GLFW_MOD_SHIFT, "shift" },
    { GLFW_MOD_CONTROL, "ctrl" },
    { GLFW_MOD_ALT, "alt" },
    { GLFW_MOD_SUPER, "super" },
};

std::map<std::string, int> stringToMod = {
    { "shift", GLFW_MOD_SHIFT },
    { "ctrl", GLFW_MOD_CONTROL },
    { "alt", GLFW_MOD_ALT },
    { "super", GLFW_MOD_SUPER },
};

u32 hashInput(int k, int a, int m){
    if(k > 256){
        switch(k){
            case GLFW_KEY_KP_ENTER: { k = GLFW_KEY_ENTER ; break; }
            case GLFW_KEY_KP_DIVIDE : { k = '/'; break; }
            case GLFW_KEY_KP_MULTIPLY: { k = '*'; break; }
            case GLFW_KEY_KP_SUBTRACT : { k = '-'; break; }
            case GLFW_KEY_KP_ADD : { k = '+'; break; }
            case GLFW_KEY_KP_EQUAL : { k = '='; break; }
            case GLFW_KEY_RIGHT_SHIFT : { k = GLFW_KEY_LEFT_SHIFT; break; }
            case GLFW_KEY_RIGHT_CONTROL : { k = GLFW_KEY_LEFT_CONTROL; break; }
            case GLFW_KEY_RIGHT_ALT : { k = GLFW_KEY_LEFT_ALT; break; }
            case GLFW_KEY_RIGHT_SUPER : { k = GLFW_KEY_LEFT_SUPER; break; }
            default:{
                if(k >= GLFW_KEY_KP_0 and k <= GLFW_KEY_KP_9) k -= GLFW_KEY_KP_0 + '0';
            }
        }
        /// in case we want only shift pressed and behave as key not mod
        if(k == GLFW_KEY_LEFT_SHIFT and m & GLFW_MOD_SHIFT) m = 0;
        if(k == GLFW_KEY_LEFT_CONTROL and m & GLFW_MOD_CONTROL) m = 0;
        if(k == GLFW_KEY_LEFT_ALT and m & GLFW_MOD_ALT) m = 0;
        if(k == GLFW_KEY_LEFT_SUPER and m & GLFW_MOD_SUPER) m = 0;
    }

    /// m is 4bits, a is 2bits, k is at least 9bits
    return u32( k<<6 | a <<4 | m );
}
u32 hashInput(KeyActionModifier keys){
    return hashInput(keys.key, keys.action, keys.modifier);
}
/// keys:function
std::pair<std::string, std::string> splitToFunctionAndKeys(const std::string &str){
    // str.erase( remove(str.begin(), str.end(),' '), str.end() );
    int a=0;
    for(int i=0; i<str.size(); i++){
        if(str[i] == ':'){
            return std::make_pair(str.substr(i+1), str.substr(0, i));
        }
    }
    return {};
}
std::vector<std::string> splitToKeys(std::string str){
    if(str.size() == 1) return {str};
    if(str.back() == '-') str.replace(str.size()-1, 1, "minus");

    int a=0;
    std::vector<std::string> values;
    for(int i=0; i<str.size(); i++){
        if(str[i] == '-'){
            values.push_back(str.substr(a, i-a));
            a = i+1;
        }
    }
    values.push_back(str.substr(a));
    return values;
}
KeyActionModifier parseKeyBinding(const std::string &str){
    KeyActionModifier out {};
    std::vector<std::string> values = splitToKeys(str);
    out.action = GLFW_PRESS;
    // everything before last key is a modifier key
    for(int i=0; i<values.size()-1; i++){
        if(stringToMod.count(values[i])){
            out.modifier |= stringToMod.at(values[i]);
        }
        else {
            error("No modifier key:", str);
        }
    }
    if(auto k = stringToKey.find(values.back()); k!=stringToKey.end())
        out.key = k->second;
    else {
        out.key = int(values.back()[0]);
    }
    if(out.key >= 'a' and out.key <= 'z') out.key -= 'a' - 'A';
    // log("\t", out.key);
    return out;
}

std::string convertKeyToString(int k, int a, int m){
    std::string out;
    switch(a){
        case GLFW_PRESS: out = "on-"; break;
        case GLFW_REPEAT: out = "hold-"; break;
        case GLFW_RELEASE: out = "off-"; break;
    }

    if(m and m & GLFW_MOD_SHIFT) out += "shift-";
    if(m and m & GLFW_MOD_CONTROL) out += "ctrl-";
    if(m and m & GLFW_MOD_ALT) out += "alt-";
    if(m and m & GLFW_MOD_SUPER) out += "super-";

    if(k >= 'a' and k <= 'z') k -= 'a' - 'A';

    if(auto found=keyToString.find(k); found!=keyToString.end()){
        out += found->second;
    }
    else out += char(k);

    return out;
}
