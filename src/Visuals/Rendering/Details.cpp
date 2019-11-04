#include "core.hpp"
#include "Details.hpp"
#include "Assets.hpp"
#include "Color.hpp"
#include "Context.hpp"

void Details::executeAtEndOfFrame() {
    // * setup buffers and commonModels VAO
    for(auto& it : m_toExecute) it();
    m_toExecute.clear();
}

void Details::drawPositionMarker(glm::vec4 position) {
    m_toExecute.emplace_back([position] {

    });
}
void Details::drawCurve(const std::vector<glm::vec4>& points, uint color) {
    m_toExecute.emplace_back([points, color, this] {
        gl::Enable(gl::DEPTH_TEST);
        gl::LineWidth(1);
        gl::Enable(gl::LINE_SMOOTH);
        auto shader = assets::bindShader("Lines");
        shader.uniform("uColor", toVec4(color));

        m_context.getBuffer().update(points).attrib(0).pointer_float(4, sizeof(glm::vec4)).divisor(0);

        gl::DrawArrays(gl::LINE, 0, points.size());
    });
}
void Details::drawWaypoints(const std::vector<Waypoint>& points, uint color) {
    m_toExecute.emplace_back([points, color, this] {
        gl::Enable(gl::DEPTH_TEST);
        gl::LineWidth(3);
        gl::Enable(gl::LINE_SMOOTH);
        auto shader = assets::bindShader("Lines");

        uint bindingPoint = 1;
        u32 blockIndex = gl::GetUniformBlockIndex(
            shader.ID, "UniformBufferObject"); // * get ubo index from shader, should be set to constant
        gl::UniformBlockBinding(shader.ID, blockIndex, bindingPoint); // * bind block to binding point

        shader.uniform("uColor", colorToVec4(color));

        m_context.getBuffer()
            .update(points)
            .attrib(0)
            .pointer_float(4, sizeof(Waypoint), (void*)offsetof(Waypoint, position))
            .divisor(0);

        gl::DrawArrays(gl::LINE_STRIP, 0, points.size());

        m_context.errors();
    });
}
