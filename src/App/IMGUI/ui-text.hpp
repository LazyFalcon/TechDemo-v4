#pragma once

// responsible for generating glyphs that will be rendered from given string
class Text
{
private:
    bool m_alreadyRendered {false};

public:
    struct Rendered
    {
        glm::vec4 polygon;
        glm::vec3 uv;
        glm::vec2 uvSize;
        float depth;
        HexColor color;
    };

    enum Formatting
    {
        Left,
        Right,
        Centered,
        Justify
    };

    using RenderedText = std::vector<Rendered>;

    void renderTo(RenderedText& container, const std::string& text);
    void renderTo(RenderedText& container, const std::u16string& text);

    float getLen(const std::string& text);
    float getLen(const std::u16string& text);

    std::string font;
    u32 color;
    Formatting formatting;
    glm::vec4 bounds;
    float depth;
};
