#pragma once
#include "ui-core.hpp"
#include "ui-panel.hpp"
#include "ui-styler.hpp"
#include <vector>

class Panel;
class RenderedUIItems;

// Main class of this miracle
class Imgui
{
private:
    Panel m_defaultPanel;
    int m_width, m_height;
    std::unique_ptr<RenderedUIItems> m_renderedUIItems;
public:
    // handling mouse actions for proper actions
    struct {
        enum ItemAction {
            None, Hover, Default, Alternate
        };
        struct Action {
            std::optional<glm::vec2> position;
            bool on;
            bool off;
            float layer;
            // checks against position from initial click
            bool pressedOn(const glm::vec4& poly, float depth){
                return on and position and (position->x>=poly.x and position->x <=poly.x+poly.z) and (position->y >=poly.y and position->y <=poly.y+poly.w);
            }
            bool pressedOff(const glm::vec4& poly, float depth){
                return off and position and (position->x>=poly.x and position->x <=poly.x+poly.z) and (position->y >=poly.y and position->y <=poly.y+poly.w);
            }
            bool pressed(const glm::vec4& poly, float depth){
                return pressedOff(poly, depth);
            }
        } lmb, rmb;

        glm::vec2 mousePos;
        glm::vec2 mouseTranslation;
        float cursorDepthInThisFrame;
        float cursorDepthInLastFrame; // to cover scenario in which items/panels are overlapping

        // checks agains layer and current mouse position
        bool hover(const glm::vec4& poly, float depth){
            bool hasHover = (mousePos.x>=poly.x and mousePos.x <=poly.x+poly.z) and (mousePos.y >=poly.y and mousePos.y <=poly.y+poly.w);
            if(hasHover and depth >= cursorDepthInLastFrame){
                cursorDepthInThisFrame = depth;
                return true;
            }
            return false;
        }

        void lmbOn(){
            lmb.position = mousePos;
            lmb.on = true;
            lmb.layer = cursorDepthInLastFrame;
        }
        void lmbOff(){ if(lmb.position){
            lmb.off = true;
        }}
        void rmbOn(){
            rmb.position = mousePos;
            rmb.on = true;
            rmb.layer = cursorDepthInLastFrame;
        }
        void rmbOff(){ if(rmb.position){
            rmb.off = true;
        }}

    } input;

    Styler basicStyle;
    void* editedValue {nullptr}; // not for reading from, only to compare edited variables

    Imgui(int width, int height, const std::string& name = "default");
    ~Imgui();

    Panel& panel(){ // returns active panel
        return m_defaultPanel;
    }

    // call on beginning of frame to cleanup states and conteners
    // creates default, full screen panel
    void restart();

    void getKey(const glm::vec4& box){}

    RenderedUIItems& getToRender(){
        return *m_renderedUIItems;
    }
};
