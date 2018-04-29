#pragma once
#include "common.hpp"

class Text
{
private:
    bool m_alreadyRendered {false};
public:
    struct Rendered
    {
        // glm::vec2 pxPosition;
        // glm::vec2 pxSize;
        glm::vec4 polygon;
        glm::vec3 uv;
        glm::vec2 uvSize;
        float depth;
        HexColor color;
    };

    enum Formatting {
        Left, Right, Centered, Justify
    };

    using RenderedText = std::vector<Rendered>;

    void renderTo(RenderedText& container,  const std::string& text);


    // float render(const std::string &text, const std::string &text, const glm::vec4 box, const HexColor color=0, const int caretPosition = -2);
    // float render(const std::u16string &text, const int fontId, const glm::vec4 box, const font::TextPosition flag=font::Left, const HexColor color=0, const int caretPosition = -2);
    float getLen(const std::string &text);

    std::string font;
    u32 color;
    Formatting formatting;
    glm::vec4 bounds;
    float depth;
};
