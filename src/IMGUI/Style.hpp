#include "ImguiCore.hpp"
#include <vector>

class Style
{
private:

public:
    void renderPanel(iBox& box){
        renderedElements.push_back({box, 0x1e1d1dff, 0});
    }
    void reset(){
        renderedElements.clear();
    }

    std::vector<ImguiRenderElement> renderedElements;
};
