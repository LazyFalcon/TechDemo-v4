#include "core.hpp"
#include "Details.hpp"

void Details::executeAtEndOfFrame(){
    // * setup buffers and commonModels VAO
    for(auto & it : m_toExecute) it();
}

void Details::drawPositionMarker(glm::vec4 position){
    m_toExecute.emplace_back([position]{

    });
}
void Details::drawCurve(const std::vector<glm::vec4>& points, uint color){
    m_toExecute.emplace_back([&points, color]{

    });
}
