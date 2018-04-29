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
        auto character = text[i];
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




// Rendering
// float Text::render(const std::string &text, const std::string &font, const glm::vec4 pxBox, const HexColor color, const int caretPosition){
//     auto &font = assets::getFont(font);
//     empty = false;
//     LastTextHeight = 0;
//     LastTextLength = 0;
//     lastTextSize = text.size();
//     glm::vec2 currentPosition = pxBox.xy();
//     u8 character;

//     // if(flag == font::Left){}
//     // else if(flag == font::Center){
//     //     float textLength = font.calculateTextLength(text);
//     //     currentPosition.x += std::max(0.f, pxBox.z/2.f - std::min(textLength/2.f, pxBox.z));
//     // }
//     // else if(flag == font::Right){
//     //     float textLength = font.calculateTextLength(text);
//     //     currentPosition.x += std::max(0.f, pxBox.z - textLength);
//     // }

//     currentPosition.y += pxBox.w/2.f - font.base/2.f;

//     currentPosition.x = ceil(currentPosition.x);
//     currentPosition.y = ceil(currentPosition.y);

//     for(u32 i = 0; i < text.size(); i++){
//         character = text[i];
//         const auto &symbol = font.symbols[character];

//         if (i > 0){ // kerning
//             currentPosition.x -= font.kerning[int(text[i - 1])<<16 & character];
//         }

//         if(currentPosition.x + symbol.pxAdvance + font.dotLen*2 >= pxBox.x+pxBox.z){
//             // TODO: go to end and check if has \n
//             currentPosition.x -= font.kerning[int('.')<<16 & character];
//             for(int i=0; i<2; i++){

//                 const auto &dot = font.symbols['.'];
//                 renderedSymbols.push_back(RenderedSymbol{
//                     currentPosition + dot.pxOffset,
//                     dot.pxSize,
//                     dot.uv,
//                     dot.uvSize,
//                     color
//                 });
//                 currentPosition.x += dot.pxAdvance;
//             }
//             break;
//         }

//         if(character == '\n'){ // new line
//             LastTextLength = std::max(LastTextLength, currentPosition[0] - pxBox.x);
//             LastTextHeight += font.lineHeight;
//             currentPosition = pxBox.xy() - glm::vec2(0, LastTextHeight);
//             continue;
//         }

//         renderedSymbols.push_back(RenderedSymbol{
//             currentPosition + symbol.pxOffset,
//             symbol.pxSize,
//             symbol.uv,
//             symbol.uvSize,
//             color
//         });
//         currentPosition.x += symbol.pxAdvance;
//     }

//     if (lastTextSize > 0) // compute len
//         LastTextLength = currentPosition[0] - pxBox.x;

//     placeCaret(fontId, currentPosition, color, caretPosition);
//     return LastTextLength + 1.f;
// }
// void Text::placeCaret(const int fontId, glm::vec2 position, const HexColor color, const int caretPosition){
//     auto &font = assets::getFont(fontId);

//     if(caretPosition == -2) return;
//     if(caretPosition == -1 and renderedSymbols.size() > 0){
//         position.x = renderedSymbols[renderedSymbols.size() - lastTextSize].pxPosition.x;
//     }
//     else if(caretPosition == -1 and renderedSymbols.empty() == 0){
//         // position = renderedSymbols[lastTextSize-1].pxPosition;
//     }
//     else {
//         int cpos = std::min(renderedSymbols.size()-1, renderedSymbols.size() - lastTextSize + caretPosition);
//         position.x = renderedSymbols[cpos].pxPosition.x + renderedSymbols[cpos].pxSize.x;
//     };

//     const auto &symbol = font.symbols['|'];

//     renderedSymbols.push_back(RenderedSymbol{
//         .pxPosition = position + symbol.pxOffset*glm::vec2(0,1),
//         .pxSize = symbol.pxSize,
//         .uv = symbol.uv,
//         .uvSize = symbol.uvSize,
//         .color = color
//     });
// }
