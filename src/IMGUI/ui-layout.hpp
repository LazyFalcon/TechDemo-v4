#pragma once
#include "common.hpp"

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
    std::function<glm::vec4(glm::vec4)> feedback;
    std::function<glm::vec4(glm::vec4)> calcPosition;

    void setBounds(glm::vec4 b){
        m_bounds = m_free = b;
    }
    void compile(){}

    Layout& toUp();
    Layout& toDown();
    Layout& toRight();
    Layout& toLeft();

    /*  ______<w>_______
        |               |
     <x>|               | <y>
        ------<z>-------
    */
    Layout& padding(const glm::vec4& p){
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


    glm::vec4 m_bounds;
    glm::vec4 m_free;
    glm::vec4 m_padding {8,8,8,8};
    float &m_x, &m_y, &m_w, &m_h;
};
