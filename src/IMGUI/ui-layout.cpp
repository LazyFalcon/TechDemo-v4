#include "ui-layout.hpp"
#include "Logging.hpp"
// used with assume that

std::vector<glm::vec4> even::precalculate(LayoutStrategy& feedback, glm::vec4 panelSize, float spacing, int indexOfAxis){
    int notAxis = (indexOfAxis+1)%2;
    glm::vec4 singleItem(0);
    // cut items padding
    float availbleLen = panelSize[indexOfAxis+2] - (elements - 1.f) * spacing;
    // set item width and height
    singleItem[indexOfAxis+2] = availbleLen/elements;
    singleItem[notAxis+2] = panelSize[notAxis+2]; // assuming that full availble space will be taken;

    auto out = std::vector<glm::vec4>(elements, singleItem);
    for(auto& it : out){
        it = feedback(it);
    }

    return out;
}

std::vector<glm::vec4> notEven::precalculate(LayoutStrategy& feedback, glm::vec4 panelSize, float spacing, int indexOfAxis){
    int notAxis = (indexOfAxis+1)%2;
    float availbleLen = panelSize[indexOfAxis+2] - (parts.size()-1.f) * spacing;
    float sumOfFloats = 0;
    float sumOfInts = 0;
    float numberOfFloats = 0;
    for(auto& it : parts){
        if(it > 1.f) {
            sumOfInts += it;
        }
        else {
            sumOfFloats += it;
            numberOfFloats += 1.f;
        }
    }

    float correctFloats = numberOfFloats ? (1-sumOfFloats)/numberOfFloats : 0.f;
    for(auto& it : parts){
        if(it > 1.f) continue;
        it += correctFloats;
        it = it*(availbleLen-sumOfInts);
    }
    // now width of each part is calced

    auto out = std::vector<glm::vec4>(parts.size());
    glm::vec4 sampleItem(0);
    sampleItem[notAxis+2] = panelSize[notAxis+2]; // assuming that full availble space will be taken;
    for(int i=0; i<parts.size(); i++){
        sampleItem[indexOfAxis+2] = parts[i];
        out[i] = feedback(sampleItem);
    }

    return out;
}



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
        w = std::min(w, m_w);

        // in case when we want to move item, check if shift isn't smaller than height
        // little not consistent here, x,y received from item are displacement in main direction, not from lower left corner
        y = m_y + m_h - std::max(y, h);

        // center item in panel, here we have one column
        x = m_x + floor(0.5f * (m_w - w));
        // cut free space
        m_h = y - m_y - m_spacing;

        return glm::vec4(x,y,w,h);
    };

    return *this;
}
Layout& Layout::toRight(){
    feedback = [this](const glm::vec4& item){
        float x(item[0]), y(item[1]), w(item[2]), h(item[3]);
        // cut item height to panel height and padding
        h = std::min(h, m_h);

        x = m_x + m_padding[0];

        // center item in panel, here we have one column
        y = m_y + floor(0.5f * (m_h - h));
        // cut free space
        float prev_x = m_x;
        m_x = x + w + m_spacing;
        m_w -= m_x - prev_x;

        return glm::vec4(x,y,w,h);
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
