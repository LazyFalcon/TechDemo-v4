#pragma once
#include "common.hpp"
#include "Logging.hpp"

using LayoutStrategy = std::function<glm::vec4(const glm::vec4&)>;

enum Alignment
{
    RIGHT, LEFT, CENTERED, TOP, BOTTOM
};

class even
{
public:
    even(int elements) : elements(elements){}
    std::vector<glm::vec4> precalculate(LayoutStrategy& feedback, glm::vec4 panelSize, glm::vec4 padding, int indexOfAxis){
        int notAxis = (indexOfAxis+1)%2;
        glm::vec4 singleItem(0);
        // cut items padding
        float availbleLen = panelSize[indexOfAxis+2] - elements * (padding[indexOfAxis] + padding[indexOfAxis+2]);
        // set item width and height
        singleItem[indexOfAxis+2] = availbleLen/elements;
        singleItem[notAxis+2] = panelSize[notAxis+2]; // assuming that full availble space will be taken;

        clog("notAxis", notAxis);
        clog("panelSize", panelSize);
        clog("elements * (padding[indexOfAxis] + padding[indexOfAxis+2])", elements * (padding[indexOfAxis] + padding[indexOfAxis+2]));
        clog("availbleLen", availbleLen);
        clog("singleItem", singleItem);



        auto out = std::vector<glm::vec4>(elements, singleItem);
        for(auto& it : out){
            it = feedback(it);
        }

        return out;
    }

private:
    float elements;
};

class distribute
{};

class notEven
{};

class predefined
{};

// used with assume that
class Layout
{
public:
    Layout() : m_x(m_free[0]), m_y(m_free[1]), m_w(m_free[2]), m_h(m_free[3]) {}

    // in: wstępnie przeliczony item, wymiary, pozycja
    // out: finalny ymiar itemu
    // how: w zależności od algorytmu, albo olewamy to co item ma ustawione, albo traktujemy jak wskazówki
    // obecna implementacja: wymiary zostawiamy ale dodajemy padding; pozycję traktujemy jako przesunięcie od obecnej pozycji startowej,
    // prostopadły wymiar ustawiamy tak żeby wycentrować item
    // czyli to co user chce przerabiamy podług wybranego algorytmu
    LayoutStrategy feedback;
    LayoutStrategy alignPolicy; // by default is is center

    void setBounds(glm::vec4 b);
    void compile(){}

    Layout& toUp();
    Layout& toDown();
    Layout& toRight();
    Layout& toRight(even&&);
    Layout& toLeft();
    Layout& dummy();

    /*  ______<w>_______
        |               |
     <x>|               | <z>
        ------<y>-------
    */
    Layout& padding(glm::vec4 p = {}){
        m_padding = p;
        return *this;
    }

    Layout& prepare(glm::vec4 evenSize, i32 count);
    Layout& prepare(const std::vector<glm::vec4>& requestedSizes);
private:
    std::vector<glm::vec4> m_generatedLayout;
    int m_used;

    int m_axis;
    int m_direction;
    int m_helper {0};


    glm::vec4 m_bounds;
    glm::vec4 m_free;
    glm::vec4 m_padding {8,8,8,8}; // left, bottom, right, top
    float &m_x, &m_y, &m_w, &m_h;
};
