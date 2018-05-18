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
    std::vector<glm::vec4> precalculate(LayoutStrategy& feedback, glm::vec4 panelSize, glm::vec4 padding, int indexOfAxis);

private:
    float elements;
};

class distribute
{};

class notEven
{
public:
    notEven(std::vector<float>&& parts) : parts(std::move(parts)){}
    std::vector<glm::vec4> precalculate(LayoutStrategy& feedback, glm::vec4 panelSize, glm::vec4 padding, int indexOfAxis);
private:
    std::vector<float> parts;
};

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
    template<typename T>
    Layout& toRight(T&& precalculator){
        toRight();
        auto itemSize = precalculator.precalculate(feedback, m_free, m_padding, 0);

        feedback = [this, itemSize](const glm::vec4& item){
            return itemSize[m_helper++];
    };
    return *this;
}
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
