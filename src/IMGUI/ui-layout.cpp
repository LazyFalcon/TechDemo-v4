#include "ui-layout.hpp"
#include "Logging.hpp"
// used with assume that

void even::precalculate(LayoutStrategy& feedback, glm::vec4 panelSize, float spacing, int indexOfAxis){
    int notAxis = (indexOfAxis+1)%2;
    glm::vec4 singleItem(0);
    // cut items padding
    float availbleLen = panelSize[indexOfAxis+2] - (elements - 1.f) * spacing;
    // set item width and height
    singleItem[indexOfAxis+2] = availbleLen/elements;
    singleItem[notAxis+2] = panelSize[notAxis+2]; // assuming that full availble space will be taken;

    m_generatedLayout = std::vector<glm::vec4>(elements, singleItem);
    for(auto& it : m_generatedLayout){
        it = feedback(it);
    }

}
glm::vec4 even::operator()(const glm::vec4& item){
    return m_generatedLayout[m_used++];
}

void notEven::precalculate(LayoutStrategy& feedback, glm::vec4 panelSize, float spacing, int indexOfAxis){
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

    m_generatedLayout.resize(parts.size());
    glm::vec4 sampleItem(0);
    sampleItem[notAxis+2] = panelSize[notAxis+2]; // assuming that full availble space will be taken;
    for(int i=0; i<parts.size(); i++){
        sampleItem[indexOfAxis+2] = parts[i];
        m_generatedLayout[i] = feedback(sampleItem);
    }

}

glm::vec4 notEven::operator()(const glm::vec4& item){
    return m_generatedLayout[m_used++];
}

void Layout::setBounds(glm::vec4 b){
    m_bounds = m_free = b;
}

Layout& Layout::toUp(Alignment alignment){
    feedback = [this, alignment](const glm::vec4& item){
        float x(item[0]), y(item[1]), w(item[2]), h(item[3]);
        // clamp item width to panel width
        w = std::min(w, m_w);

        y = m_y;

        // apply alignment
        x = alignment==LEFT? m_x : alignment==RIGHT? (m_x + m_w - w) : (m_x + floor(0.5f * (m_w - w)));
        // cut free space
        float prev_y = m_y;
        m_y = y + h + m_spacing;
        m_h = m_y - prev_y;

        return glm::vec4(x,y,w,h);
    };

    return *this;
}
Layout& Layout::toDown(Alignment alignment){
    feedback = [this, alignment](const glm::vec4& item){
        float x(item[0]), y(item[1]), w(item[2]), h(item[3]);
        // clamp item width to panel width
        w = std::min(w, m_w);

        // in case when we want to move item, check if shift isn't smaller than height
        // little not consistent here, x,y received from item are displacement in main direction, not from lower left corner
        y = m_y + m_h - std::max(y, h);

        // apply alignment
        x = alignment==LEFT? m_x : alignment==RIGHT? (m_x + m_w - w) : (m_x + floor(0.5f * (m_w - w)));
        // cut free space
        m_h = y - m_y - m_spacing;

        return glm::vec4(x,y,w,h);
    };

    return *this;
}
Layout& Layout::toRight(Alignment alignment){
    feedback = [this, alignment](const glm::vec4& item){
        float x(item[0]), y(item[1]), w(item[2]), h(item[3]);
        // cut item height to panel height and padding
        h = std::min(h, m_h);

        x = m_x;

        // apply alignment
        y = alignment==UP? (m_y + m_h - h) : alignment==DOWN? m_y : (m_y + floor(0.5f * (m_h - h)));

        // cut free space
        float prev_x = m_x;
        m_x = x + w + m_spacing;
        m_w -= m_x - prev_x;

        return glm::vec4(x,y,w,h);
    };
    return *this;
}
Layout& Layout::toLeft(Alignment alignment){
    feedback = [this, alignment](const glm::vec4& item){
        float x(item[0]), y(item[1]), w(item[2]), h(item[3]);

        h = std::min(h, m_h);

        // * x received in item is displacement from starting point in direction, so here it is move in left by x pixels
        x = m_x + m_w - std::max(x, w);

        y = alignment==UP? (m_y + m_h - h) : alignment==DOWN? m_y : (m_y + floor(0.5f * (m_h - h)));

        m_w = x - m_x - m_spacing;

        return glm::vec4(x,y,w,h);
    };
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
