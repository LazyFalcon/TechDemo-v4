#include "ui-text.hpp"
#include "Assets.hpp"
#include "Utils.hpp"
#include "font.hpp"
#include "font.hpp"
#include "Logging.hpp"

float LastTextLength;
float LastTextHeight;


float Text::getLen(const std::string &text){
    float len = 0.f;
    u8 letter;
    for (u32 i = 0; i < text.size(); i++){
        letter = text[i];
        // len += fontInfo->m_letters[letter].size.x; // TODO:
    }
    return len;
}
float Text::getLen(const std::u16string &text){
    float len = 0.f;
    u8 letter;
    for (u32 i = 0; i < text.size(); i++){
        letter = text[i];
        // len += fontInfo->m_letters[letter].size.x; // TODO:
    }
    return len;
}

// this function is not doing text breaking,
void Text::renderTo(RenderedText& container, const std::string& text){
    auto& fontData = assets::getFont(font);

    glm::vec2 start = bounds.xy();

    // align horizontally
    if(formatting == Left){}
    else if(formatting == Centered){
        float textLength = fontData.calculateTextLength(text);
        start.x += std::max(0.f, bounds.z/2.f - std::min(textLength/2.f, bounds.z));
    }
    else if(formatting == Right){
        float textLength = fontData.calculateTextLength(text);
        start.x += std::max(0.f, bounds.z - textLength);
    }
    else if(formatting == Justify){
        float textLength = fontData.calculateTextLength(text);
        start.x += std::max(0.f, bounds.z/2.f - std::min(textLength/2.f, bounds.z));
    }

    // center vertically
    {
        start.y += (bounds[3] - fontData.base)*0.5f;
    }

    start.x = floor(start.x);
    start.y = ceil(start.y);

    for(u32 i = 0; i < text.size(); i++){
        int character = text[i];
        const auto &symbol = fontData.symbols[character];

        if (i > 0){ // kerning
            start.x -= fontData.kerning[int(text[i - 1])<<16 & character];
        }

        // check if text fits in bounds, cut otherwise
        // if(start.x + symbol.pxAdvance + fontData.dotLen*2 >= bounds.x+bounds.z){
        //     // TODO: go to end and check if has \n
        //     start.x -= fontData.kerning[int('.')<<16 & character];
        //     for(int j=0; j<2; j++){

        //         const auto &dot = fontData.symbols['.'];

        //         container.push_back(Rendered{
        //             {start + symbol.pxOffset, symbol.pxSize},
        //             symbol.uv,
        //             symbol.uvSize,
        //             depth,
        //             color
        //         });
        //         start.x += dot.pxAdvance;
        //     }
        //     break;
        // }
        // log("", glm::vec4(start + symbol.pxOffset, symbol.pxSize));
        container.push_back(Rendered{
            {start + symbol.pxOffset, symbol.pxSize},
            symbol.uv,
            symbol.uvSize,
            depth,
            color
        });
        start.x += symbol.pxAdvance;
    }

    m_alreadyRendered = true;
}

void Text::renderTo(RenderedText& container, const std::u16string& text){
    auto& fontData = assets::getFont(font);

    glm::vec2 start = bounds.xy();

    // align horizontally
    if(formatting == Left){}
    else if(formatting == Centered){
        float textLength = fontData.calculateTextLength(text);
        start.x += std::max(0.f, bounds.z/2.f - std::min(textLength/2.f, bounds.z));
    }
    else if(formatting == Right){
        float textLength = fontData.calculateTextLength(text);
        start.x += std::max(0.f, bounds.z - textLength);
    }
    else if(formatting == Justify){
        float textLength = fontData.calculateTextLength(text);
        start.x += std::max(0.f, bounds.z/2.f - std::min(textLength/2.f, bounds.z));
    }

    // center vertically
    {
        start.y += (bounds[3] - fontData.base)*0.5f;
    }

    start.x = floor(start.x);
    start.y = ceil(start.y);

    clog("size", text.size());

    for(u32 i = 0; i < text.size(); i++){
        char16_t character = text[i];
        clog("c", int(character));
        const auto &symbol = fontData.symbols[character];

        // if (i > 0){ // kerning
        //     start.x -= fontData.kerning[int(text[i - 1])<<16 & character];
        // }

        container.push_back(Rendered{
            {start + symbol.pxOffset, symbol.pxSize},
            symbol.uv,
            symbol.uvSize,
            depth,
            color
        });
        start.x += symbol.pxAdvance;
    }

    m_alreadyRendered = true;
}
