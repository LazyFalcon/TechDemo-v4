#pragma once
#include "common.hpp"

struct UnicodeSymbolInfo {
    glm::vec3 uv; // [0-1]
    glm::vec2 uvSize; // [0-1]
    glm::vec2 pxSize; // px
    glm::vec2 pxOffset; // px
    float pxAdvance;// px
};

class Font
{
private:
    void loadCharacter(const std::string &word, float U, float V, int pageOffset);
    void loadKerning(const std::string &word);
public:
    std::unordered_map<char16_t, UnicodeSymbolInfo> symbols;
    std::unordered_map<u32, float> kerning;// a.id<<16 & b.id

    void load(const std::string &name, std::vector<std::string> &imagesToLoad);
    float calculateTextLength(const std::string &text);
    float calculateTextLength(const std::u16string &text);

    float lineHeight;
    float base;
    float dotLen;
};

int convertFontFilenameToId(const std::string&);
