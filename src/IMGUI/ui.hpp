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
    std::vector<Panel> m_panelStack;
    i32 m_width, m_height;
    std::unique_ptr<RenderedUIItems> m_renderedUIItems;
public:
    struct {
        struct Action {
            std::optional<glm::vec2> position;
            bool on;
            bool off;
            bool pressedOn(const glm::vec4& poly, float depth){
                return position and on and (position->x>=poly.x and position->x <=poly.x+poly.z) and (position->y >=poly.y and position->y <=poly.y+poly.w);
            }
            bool pressedOff(const glm::vec4& poly, float depth){
                return position and off and (position->x>=poly.x and position->x <=poly.x+poly.z) and (position->y >=poly.y and position->y <=poly.y+poly.w);
            }
            bool pressed(const glm::vec4& poly, float depth){
                return pressedOff(poly, depth);
            }
        } main, alternate;

        glm::vec2 mousePos;
        float cursorDepthInThisFrame;
        float cursorDepthInLastFrame; // to cover scenario in which items/panels are overlapping

        bool hover(const glm::vec4& poly, float depth){
            bool hasHover = (mousePos.x>=poly.x and mousePos.x <=poly.x+poly.z) and (mousePos.y >=poly.y and mousePos.y <=poly.y+poly.w);
            if(hasHover and depth >= cursorDepthInLastFrame){
                cursorDepthInThisFrame = depth;
                return true;
            }
            return false;
        }

        void defaultOn(){
            main.position = mousePos;
            main.on = true;
        }
        void defaultOff(){ if(main.position){
            main.off = true;
        }}
        void alternateOn(){
            alternate.position = mousePos;
            alternate.on = true;
        }
        void alternateOff(){ if(alternate.position){
            alternate.off = true;
        }}

    } input;

    Styler basicStyle;

    Imgui(i32 width, i32 height, const std::string& name = "default");
    ~Imgui();

    Panel& panel(){ // returns active panel
        return m_panelStack.back();
    }

    Panel& newFixedPanel();
    Panel& instantiateNewFixedPanel();
    void finishPanel(Panel*);

    void restart();

    void getKey(const glm::vec4& box){}


    RenderedUIItems& getToRender(){
        return *m_renderedUIItems;
    }
};
