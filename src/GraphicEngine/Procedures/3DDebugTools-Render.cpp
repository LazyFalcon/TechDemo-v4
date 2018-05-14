#include "3DDebugTools.hpp"
#include "common.hpp"
#include "Camera.hpp"

namespace graphic {

void drawCubes(Camera &camera){
    // auto &cubes = getDebugCubes();
    // if(cubes.empty()) return;

    // auto shader = assets::getShader("DebugCube");
    // shader.uniform("uPV", camera.getPV());

    // Shape::defaultVAO.bind();
    // Shape::cube.bind().attrib(0).pointer_float(4).divisor(0);

    // for(auto &cube : cubes){
    //     shader.uniform("uTransform", cube.transform);
    //     gl::DrawArrays(gl::LINES, 0, 24);
    // }
    // cubes.clear();
    // gl::BindVertexArray(0);
    // CHECK_FOR_ERRORS
}

void drawDebug(Camera &camera){
    // return;
    // gl::Enable(gl::DEPTH_TEST);
    // gl::DepthMask(gl::TRUE_);
    // gl::Disable(gl::BLEND);

    // drawCubes(camera);
}
}
