#include "ui-layout.hpp"
// used with assume that

void Layout::setBounds(glm::vec4 b){
    m_bounds = m_free = b;
}

Layout& Layout::toUp(){

    return *this;
}
Layout& Layout::toDown(){
    feedback = [this](glm::vec4 item){
        // clamp item width to panel width
        item[2] = std::min(item[2], m_w-m_padding[0]-m_padding[1]);

        // in case when we want to move item, check if shift isn't smaller than height
        // little not consistent here, x,y received from item are displacement in main direction, not from lower left corner
        item[1] = m_y + m_h - std::max(item[1], item[3]) - m_padding[2];

        // center item in panel, here we have one column
        item[0] = m_x + floor(0.5f * (m_w - item[2]));
        // cut free space
        m_h = item[1] - m_y - m_padding[3];

        return item;
    };

    calcPosition = [this](glm::vec4 item){
        // clamp item width to panel width
        item[2] = std::min(item[2], m_w);

        // in case when we nant to move item, check if shift isn't smaller than height
        item[1] = m_y + m_h - std::max(item[1], item[3]);

        // center item in panel, here we have one column
        item[0] = floor(0.5f * (m_w - item[2]));

        return item;
    };

    return *this;
}
Layout& Layout::toRight(){

    return *this;
}
Layout& Layout::toLeft(){

    return *this;
}
Layout& Layout::dummy(){
    feedback = [](glm::vec4 item){return item;};
    calcPosition = [](glm::vec4 item){return item;};
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
