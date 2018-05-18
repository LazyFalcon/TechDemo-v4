#include "ui-layout.hpp"
#include "Logging.hpp"
// used with assume that

void Layout::setBounds(glm::vec4 b){
    m_bounds = m_free = b;
}

Layout& Layout::toUp(){

    return *this;
}
Layout& Layout::toDown(){
    feedback = [this](const glm::vec4& item){
        float x(item[0]), y(item[1]), w(item[2]), h(item[3]);
        // clamp item width to panel width
        w = std::min(w, m_w-m_padding[0]-m_padding[1]);

        // in case when we want to move item, check if shift isn't smaller than height
        // little not consistent here, x,y received from item are displacement in main direction, not from lower left corner
        y = m_y + m_h - std::max(y, h) - m_padding[3];

        // center item in panel, here we have one column
        x = m_x + floor(0.5f * (m_w - w));
        // cut free space
        m_h = y - m_y - m_padding[3];

        return glm::vec4(x,y,w,h);
    };

    return *this;
}
Layout& Layout::toRight(){
    feedback = [this](const glm::vec4& item){
        float x(item[0]), y(item[1]), w(item[2]), h(item[3]);
        // cut item height to panel height and padding
        h = std::min(h, m_h-m_padding[1]-m_padding[3]);

        x = m_x + m_padding[0];

        // center item in panel, here we have one column
        y = m_y + floor(0.5f * (m_h - h));
        // cut free space
        float prev_x = m_x;
        m_x += w + m_padding[0];
        m_w -= m_x - prev_x;

        return glm::vec4(x,y,w,h);
    };
    return *this;
}
Layout& Layout::toRight(even&& precalculator){
    toRight();
    auto itemSize = precalculator.precalculate(feedback, m_free, m_padding, 0);

    feedback = [this, itemSize](const glm::vec4& item){
        clog(m_helper, itemSize[m_helper]);
        return itemSize[m_helper++];
    };
    return *this;
}
Layout& Layout::toLeft(){

    return *this;
}
Layout& Layout::dummy(){
    feedback = [](const glm::vec4& item){return item;};
    return *this;
}

// align, domyślnie środek

// zgłaszamy że tyle i tyle obiektów o takich wymiarach będzie do wyrysowania
Layout& Layout::prepare(glm::vec4 evenSize, i32 count){
    return *this;
} // ileśtam obiektów tego samego wymiaru, do rozłożenia zgodnie z algortmem, można stackować :D
Layout& Layout::prepare(const std::vector<glm::vec4>& requestedSizes){
    return *this;
} // pamiętać o uwzględnieniu
