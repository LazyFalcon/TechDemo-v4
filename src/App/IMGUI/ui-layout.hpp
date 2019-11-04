#pragma once

using LayoutStrategy = std::function<glm::vec4(const glm::vec4&)>;

enum Alignment
{
    RIGHT,
    LEFT,
    CENTER,
    UP,
    DOWN
};

/*
    Distribute n elements evenly in given space
    |   |   |   |   |   |
*/
class even
{
public:
    even(int elements) : elements(elements) {}
    void precalculate(LayoutStrategy& feedback, glm::vec4 panelSize, float spacing, int indexOfAxis);
    glm::vec4 operator()(const glm::vec4&);

private:
    float elements;
    std::vector<glm::vec4> m_generatedLayout;
    int m_used {};
};

/*
    Not even distribution, if floats are not add up to one, they are corrected
    |  |   | |     |    |
*/
class notEven
{
public:
    notEven(std::vector<float>&& parts) : parts(std::move(parts)) {}
    void precalculate(LayoutStrategy& feedback, glm::vec4 panelSize, float spacing, int indexOfAxis);
    glm::vec4 operator()(const glm::vec4&);

private:
    std::vector<float> parts;
    std::vector<glm::vec4> m_generatedLayout;
    int m_used {};
};

/*
    Similar to notEven, dot with given sizes of items, distribution is done by adjusting spcacing between items
*/
class distribute
{};

/*
    Returns data calculated by hand or from store
*/
class predefined
{};

class horizontalGrid
{};

class verticalGrid
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
    void compile() {}

    Layout& toUp(Alignment alignment = CENTER);
    Layout& toDown(Alignment alignment = CENTER);
    template<typename T>
    Layout& toDown(T&& precalculator, Alignment alignment = CENTER) {
        toDown(alignment);
        precalculator.precalculate(feedback, m_free, m_spacing, 1);
        feedback = precalculator;

        return *this;
    }
    Layout& toRight(Alignment alignment = CENTER);
    template<typename T>
    Layout& toRight(T&& precalculator, Alignment alignment = CENTER) {
        toRight(alignment);
        precalculator.precalculate(feedback, m_free, m_spacing, 0);

        feedback = precalculator;
        return *this;
    }
    Layout& toLeft(Alignment alignment = CENTER);
    Layout& dummy();

    Layout& twoColumns();

    /*  ______<w>_______
        |               |
     <x>|               | <z>
        ------<y>-------
    */
    Layout& padding(glm::vec4 p = {}) {
        m_padding = p;
        m_free[0] += p[0];
        m_free[1] += p[1];
        m_free[2] -= p[0] + p[2];
        m_free[3] -= p[1] + p[3];
        m_bounds = m_free;
        return *this;
    }
    Layout& spacing(float s) {
        m_spacing = s;

        return *this;
    }
    const glm::vec4& getBounds() const {
        return m_bounds;
    }
    const glm::vec4& getFree() const {
        return m_free;
    }

    Layout& prepare(glm::vec4 evenSize, i32 count);
    Layout& prepare(const std::vector<glm::vec4>& requestedSizes);
    glm::vec4 getSpaceLeft() {
        return m_free;
    }

protected:
    glm::vec4 m_bounds;
    glm::vec4 m_free;
    glm::vec4 m_padding {0}; // left, bottom, right, top
    float m_spacing {8.f};
    float &m_x, &m_y, &m_w, &m_h;
};
