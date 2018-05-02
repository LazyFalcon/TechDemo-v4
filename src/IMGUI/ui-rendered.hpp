#pragma once
#include "common.hpp"
#include "ui-text.hpp"
#include <vector>
#include <tuple>


/*
DOC:
How it will be rendered:
- depth pre pass to exclude lower layers from being visible
- render sorted backgrounds of Panles
- render all in one pass with depth test enabled
- probably crop rects also could be implemented here, i.e as equal depth test

For Panel backgrouds there are prepared few textures of screen size, layer 0 of this atlas is emty texture, only o allow color
Button backgrounds can be more specialized, and allow custom images, images with color, blured part and color as they don't need to be sorted

*/

struct RenderedUIItems
{
    struct Background
    {
        glm::vec4 box; // tex coords are extracted from box screen position, layer 0 is black
        float texture; // layer from atlas
        float depth;
        u32 color;
    };

    struct Image
    {
        glm::vec4 box; // tex coords are extracted from box screen position, layer 0 is black
        float texture; // layer from atlas
        float depth;
    };

    struct ColoredImage
    {
        glm::vec4 box; // tex coords are extracted from box screen position, layer 0 is black
        float texture; // layer from atlas
        float depth;
        u32 color;
    };

    struct ColoredBox
    {
        glm::vec4 box;
        float depth;
        u32 color;
    };

    struct RenderedFont
    {
        glm::vec4 position;
        float depth;
        float layerId;
        u32 color;
    };

    struct Line
    {
        glm::vec2 from;
        glm::vec2 to;
        float depth;
        u32 color;
    };

    struct LineStrip
    {
        float depth;
        u32 color;
        std::vector<glm::vec2> points;
    };

    // for better accesibility and names for containers are not needed
    std::tuple<
        // these are render as opaque elements in depth-pre pass
        std::vector<RenderedUIItems::Background>, // this also needs to be sorted
        std::vector<RenderedUIItems::Image>,
        std::vector<RenderedUIItems::ColoredImage>,
        std::vector<RenderedUIItems::ColoredBox>,

        // these not
        std::vector<RenderedUIItems::Line>,
        std::vector<RenderedUIItems::LineStrip>,
        std::vector<Text::Rendered>
    > prepared;

    template<typename T>
    std::vector<T>& get(){
        return std::get<std::vector<T>>(prepared);
    }
    template<typename T>
    void put(const T& t){
        std::get<std::vector<T>>(prepared).push_back(t);
    }
    void reset(){

    }
};
