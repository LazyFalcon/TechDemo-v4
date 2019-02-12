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
    m_toExecute.emplace_back([points, color]{
        gl::Enable(gl::DEPTH_TEST);
        gl::LineWidth(1);
        gl::Enable(gl::LINE_SMOOTH);
        auto shader = assets::bindShader("Lines");
        shader.uniform("uColor", toVec4(it.color));
        shader.uniform("uPV", camera.PV);
        shader.uniform("uEye", camera.position());

        context.getBuffer().update(points)
               .attrib(0).pointer_float(4, sizeof(glm::vec4)).divisor(0);

        gl::DrawArrays(gl::LINE, 0, sparks.size());
    });
}
void Details::drawWaypoints(const std::vector<Waypoint>& points, uint color){
    m_toExecute.emplace_back([points, color]{
        gl::Enable(gl::DEPTH_TEST);
        gl::LineWidth(1);
        gl::Enable(gl::LINE_SMOOTH);
        auto shader = assets::bindShader("Lines");
        shader.uniform("uColor", toVec4(it.color));
        shader.uniform("uPV", camera.PV);
        shader.uniform("uEye", camera.position());

        context.getBuffer().update(points)
               .attrib(0).pointer_float(4, sizeof(Waypoint), (void*).offsetof(Waypoint, position)).divisor(0);

        gl::DrawArrays(gl::LINE, 0, sparks.size());
    });
}
