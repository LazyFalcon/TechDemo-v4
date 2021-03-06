#include "core.hpp"
#include "Context.hpp"
#include "Assets.hpp"
#include "BaseStructs.hpp"
#include "Logger.hpp"
#include "PerfTimers.hpp"
#include "ResourceLoader.hpp"
#include "visuals-prepared-scene.hpp"

namespace debug
{
struct CurrentState
{
    std::map<int, std::string> nameMapping;

    std::vector<std::string> records;
    int fbo;
    int shader;
    std::vector<int> targets;
    std::string getName(int id) {
        auto it = nameMapping.find(id);
        if(it != nameMapping.end())
            return it->second;
        return std::to_string(id);
    }
    std::string print() {
        std::string targetsToPrint;
        for(auto it : targets) targetsToPrint += " " + getName(it);
        return "Shader: " + getName(shader) + ", FBO: " + getName(fbo) + ", targets:" + targetsToPrint;
    }
};

CurrentState currentState;

void rememberFbo(std::string_view name, int id) {
    currentState.nameMapping[id] = name;
}
void rememberShader(std::string_view name, int id) {
    currentState.nameMapping[id] = name;
    console.log(printSetup());
}
void rememberTexture(std::string_view name, int id) {
    currentState.nameMapping[id] = name;
}

void logFboChange(int id) {
    currentState.fbo = id;
    currentState.targets.resize(0);
}
void logShaderChange(int id) {
    currentState.shader = id;
}
void logTargetChange(int id, int targetId) {}
std::string printSetup() {
    return currentState.print();
}
}

void messageCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message,
                     const void* userParam) {
    if(gl::DEBUG_TYPE_ERROR == type) {
        console.log("GL CALLBACK:", "type:", type, "severity:", severity, "message:", message);
        console.log("GL state:", debug::printSetup());
    }
}

Context::Context(Window& window) : window(window), fbo(window.size) {
    gl::Enable(gl::DEBUG_OUTPUT);
    gl::DebugMessageCallback(messageCallback, 0);
}

void Context::reset() {
    resetTextures();
    resetFbo();
    resetBuffers();
    resetShapes();

    GpuTimerScoped::init();

    { // catch error
        int err = gl::GetError();
        if(err != gl::NO_ERROR_) {
            console.error("FBO fail:", err);
        }
        else
            console.log("[ OK ] initialisation of FBOs");
    }
}

void Context::resetTextures() {
    console.log("--textures");
    tex.gbuffer.color =
        Texture(gl::TEXTURE_2D, gl::RGBA16F, window.size.x, window.size.y, 1, gl::RGBA, gl::HALF_FLOAT, gl::LINEAR, 4);
    tex.gbuffer.normals =
        Texture(gl::TEXTURE_2D, gl::RGBA16F, window.size.x, window.size.y, 1, gl::RGBA, gl::HALF_FLOAT, gl::LINEAR, 0);
    tex.gbuffer.depth = Texture(gl::TEXTURE_2D, gl::DEPTH_COMPONENT32F, window.size.x, window.size.y, 1,
                                gl::DEPTH_COMPONENT, gl::FLOAT, gl::LINEAR, 0);

    debug::rememberTexture("gbuffer depth", tex.gbuffer.depth.ID);

    tex.light.color =
        Texture(gl::TEXTURE_2D, gl::RGBA16F, window.size.x, window.size.y, 1, gl::RGBA, gl::HALF_FLOAT, gl::LINEAR, 4);
    tex.light.specular =
        Texture(gl::TEXTURE_2D, gl::RGBA16F, window.size.x, window.size.y, 1, gl::RGBA, gl::HALF_FLOAT, gl::LINEAR, 4);

    tex.full.a =
        Texture(gl::TEXTURE_2D, gl::RGBA16F, window.size.x, window.size.y, 1, gl::RGBA, gl::HALF_FLOAT, gl::LINEAR, 4);
    tex.full.b =
        Texture(gl::TEXTURE_2D, gl::RGBA16F, window.size.x, window.size.y, 1, gl::RGBA, gl::HALF_FLOAT, gl::LINEAR, 4);
    tex.full.rg16a =
        Texture(gl::TEXTURE_2D, gl::RG16F, window.size.x, window.size.y, 1, gl::RG, gl::HALF_FLOAT, gl::LINEAR, 2);
    tex.full.rg16b =
        Texture(gl::TEXTURE_2D, gl::RG16F, window.size.x, window.size.y, 1, gl::RG, gl::HALF_FLOAT, gl::LINEAR, 2);
    tex.half.a = Texture(gl::TEXTURE_2D, gl::RGBA16F, window.size.x / 2, window.size.y / 2, 1, gl::RGBA, gl::HALF_FLOAT,
                         gl::LINEAR, 0);
    tex.half.b = Texture(gl::TEXTURE_2D, gl::RGBA16F, window.size.x / 2, window.size.y / 2, 1, gl::RGBA, gl::HALF_FLOAT,
                         gl::LINEAR, 0);
    tex.half.rg16a = Texture(gl::TEXTURE_2D, gl::RG16F, window.size.x / 2, window.size.y / 2, 1, gl::RG, gl::HALF_FLOAT,
                             gl::LINEAR, 0);
    tex.half.rg16b = Texture(gl::TEXTURE_2D, gl::RG16F, window.size.x / 2, window.size.y / 2, 1, gl::RG, gl::HALF_FLOAT,
                             gl::LINEAR, 0);
    tex.quarter.a = Texture(gl::TEXTURE_2D, gl::RGBA16F, window.size.x / 4, window.size.y / 4, 1, gl::RGBA,
                            gl::HALF_FLOAT, gl::LINEAR, 0);
    tex.quarter.b = Texture(gl::TEXTURE_2D, gl::RGBA16F, window.size.x / 4, window.size.y / 4, 1, gl::RGBA,
                            gl::HALF_FLOAT, gl::LINEAR, 0);
    tex.eight.a = Texture(gl::TEXTURE_2D, gl::RGBA16F, window.size.x / 8, window.size.y / 8, 1, gl::RGBA,
                          gl::HALF_FLOAT, gl::LINEAR, 0);
    tex.eight.b = Texture(gl::TEXTURE_2D, gl::RGBA16F, window.size.x / 8, window.size.y / 8, 1, gl::RGBA,
                          gl::HALF_FLOAT, gl::LINEAR, 0);

    // tex.blurredScene = Texture(gl::TEXTURE_2D, gl::RGB10_A2, window.size.x, window.size.y, 1, gl::RGBA, gl::UNSIGNED_INT_10_10_10_2, gl::LINEAR, 0);
    tex.ldr.finalRenderedFrame = Texture(gl::TEXTURE_2D, gl::RGB10_A2, window.size.x, window.size.y, 1, gl::RGBA,
                                         gl::UNSIGNED_INT_10_10_10_2, gl::LINEAR, 3);
    tex.ldr.full = Texture(gl::TEXTURE_2D, gl::RGB10_A2, window.size.x, window.size.y, 1, gl::RGBA,
                           gl::UNSIGNED_INT_10_10_10_2, gl::LINEAR, 3);
    tex.ldr.half.a = Texture(gl::TEXTURE_2D, gl::RGB10_A2, window.size.x / 2, window.size.y / 2, 1, gl::RGBA,
                             gl::UNSIGNED_INT_10_10_10_2, gl::LINEAR, 0);
    tex.ldr.half.b = Texture(gl::TEXTURE_2D, gl::RGB10_A2, window.size.x / 2, window.size.y / 2, 1, gl::RGBA,
                             gl::UNSIGNED_INT_10_10_10_2, gl::LINEAR, 0);
    tex.ldr.half.c = Texture(gl::TEXTURE_2D, gl::RGB10_A2, window.size.x / 2, window.size.y / 2, 1, gl::RGBA,
                             gl::UNSIGNED_INT_10_10_10_2, gl::LINEAR, 0);
    tex.ldr.half.d = Texture(gl::TEXTURE_2D, gl::RGB10_A2, window.size.x / 2, window.size.y / 2, 1, gl::RGBA,
                             gl::UNSIGNED_INT_10_10_10_2, gl::LINEAR, 0);
    tex.ldr.half.wide = Texture(gl::TEXTURE_2D, gl::RGB10_A2, window.size.x, window.size.y / 2, 1, gl::RGBA,
                                gl::UNSIGNED_INT_10_10_10_2, gl::LINEAR, 0);
}
void Context::resetFbo() {
    console.log("--fbos");
    fbo.drawBuffers[0] = gl::COLOR_ATTACHMENT0;
    fbo.drawBuffers[1] = gl::COLOR_ATTACHMENT1;
    fbo.drawBuffers[2] = gl::COLOR_ATTACHMENT2;
    {
        gl::GenFramebuffers(1, &fbo[FULL].id);
        auto& f = fbo[FULL];
        f.viewport(0, 0, window.size.x, window.size.y);
        f.tex(tex.gbuffer.color).tex(tex.gbuffer.normals).tex(tex.gbuffer.depth)();
        fbo.unbind();
        debug::rememberFbo("Full", f.id);
    }
    {
        gl::GenFramebuffers(1, &fbo[BY2].id);
        auto& f = fbo[BY2];
        f.viewport(0, 0, window.size.x / 2.f, window.size.y / 2.f);
        f.tex(tex.half.a)();
        fbo.unbind();
        debug::rememberFbo("1/2", f.id);
    }
    {
        gl::GenFramebuffers(1, &fbo[BY4].id);
        auto& f = fbo[BY4];
        f.viewport(0, 0, window.size.x / 4.f, window.size.y / 4.f);
        f.tex(tex.quarter.a)();
        fbo.unbind();
        debug::rememberFbo("1/4", f.id);
    }
    {
        gl::GenFramebuffers(1, &fbo[BY8].id);
        auto& f = fbo[BY8];
        f.viewport(0, 0, window.size.x / 8.f, window.size.y / 8.f);
        f.tex(tex.eight.a)();
        fbo.unbind();
        debug::rememberFbo("1/8", f.id);
    }
    {
        gl::GenFramebuffers(1, &fbo[HALF_WIDE].id);
        auto& f = fbo[HALF_WIDE];
        f.viewport(0, 0, window.size.x, window.size.y / 2.f);
        f.tex(tex.ldr.half.wide)();
        fbo.unbind();
        debug::rememberFbo("1/2 wide", f.id);
    }
}
void Context::resetBuffers() {
    console.log("--buffers");
    defaultVAO.setup()();
    for(auto& it : randomBuffers) { it.setup(32000, true)(); }

    gl::GenBuffers(1, &ubo.matrices);
    gl::BindBuffer(gl::UNIFORM_BUFFER, ubo.matrices);

    gl::BufferData(gl::UNIFORM_BUFFER, sizeof(glm::mat4) * ubo.size, nullptr, gl::DYNAMIC_DRAW);
    // gl::BindBufferBase(GL_UNIFORM_BUFFER, bindingPoint, ubo.matrices);
    gl::BindBuffer(gl::UNIFORM_BUFFER, 0);

    gl::GenBuffers(1, &ubo.common);
    gl::BindBuffer(gl::UNIFORM_BUFFER, ubo.common);

    gl::BufferData(gl::UNIFORM_BUFFER, sizeof(visuals::Uniforms), nullptr, gl::STREAM_DRAW);
    // gl::BindBufferBase(GL_UNIFORM_BUFFER, bindingPoint, ubo.common);
    gl::BindBuffer(gl::UNIFORM_BUFFER, 0);
}
void Context::UBOs::update(std::vector<glm::mat4>& m) {
    gl::BindBuffer(gl::UNIFORM_BUFFER, matrices);
    gl::BufferSubData(gl::UNIFORM_BUFFER, 0, sizeof(glm::mat4) * m.size(), m.data());
    gl::BindBuffer(gl::UNIFORM_BUFFER, 0);
}
void Context::UBOs::update(glm::mat4* data, int size) {
    gl::BindBuffer(gl::UNIFORM_BUFFER, matrices);
    gl::BufferSubData(gl::UNIFORM_BUFFER, 0, sizeof(glm::mat4) * size, data);
    gl::BindBuffer(gl::UNIFORM_BUFFER, 0);
}

void Context::resetShapes() { // TODO: zwalidować użycia tego
    console.log("--shapes");
    float point[] = {0.0f, 0.0f, 0.5f, 1.f};
    float quadCentered[] = {
        -0.5f, -0.5f, 0.0f, 0.0f, -0.5f, 0.5f, 0.0f, 1.0f, 0.5f, -0.5f, 1.0f, 0.0f, 0.5f, 0.5f, 1.0f, 1.0f,
    };
    float quadCorner[] = {
        0.f, 0.f, 0.0f, 0.0f, 0.f, 1.f, 0.0f, 1.f, 1.f, 0.f, 1.0f, 0.f, 1.f, 1.f, 1.0f, 1.f,
    };
    float screen[] = {
        -1.f, -1.f, 0.f, 0.f, -1.f, 1.f, 0.f, 1.f, 1.f, -1.f, 1.f, 0.f, 1.f, 1.f, 1.f, 1.f,
    };
    float cube[] = {
        // columns
        -0.5,
        -0.5,
        -0.5,
        1,
        -0.5,
        -0.5,
        0.5,
        1,
        0.5,
        -0.5,
        -0.5,
        1,
        0.5,
        -0.5,
        0.5,
        1,
        -0.5,
        0.5,
        -0.5,
        1,
        -0.5,
        0.5,
        0.5,
        1,
        0.5,
        0.5,
        -0.5,
        1,
        0.5,
        0.5,
        0.5,
        1,
        // bottom
        -0.5,
        -0.5,
        -0.5,
        1,
        0.5,
        -0.5,
        -0.5,
        1,
        -0.5,
        -0.5,
        -0.5,
        1,
        -0.5,
        0.5,
        -0.5,
        1,
        0.5,
        0.5,
        -0.5,
        1,
        -0.5,
        0.5,
        -0.5,
        1,
        0.5,
        0.5,
        -0.5,
        1,
        0.5,
        -0.5,
        -0.5,
        1,
        // top
        -0.5,
        -0.5,
        0.5,
        1,
        0.5,
        -0.5,
        0.5,
        1,
        -0.5,
        -0.5,
        0.5,
        1,
        -0.5,
        0.5,
        0.5,
        1,
        0.5,
        0.5,
        0.5,
        1,
        -0.5,
        0.5,
        0.5,
        1,
        0.5,
        0.5,
        0.5,
        1,
        0.5,
        -0.5,
        0.5,
        1,
    };
    shape.point.setup(point, sizeof(float) * 4, false)();
    shape.quadCentered.setup(quadCentered, sizeof(float) * 16, false)();
    shape.quadCorner.setup(quadCorner, sizeof(float) * 16, false)();
    shape.screen.setup(screen, sizeof(float) * 16, false)();
    shape.cube.setup(cube, sizeof(float) * 24 * 4, false)();

    std::vector<glm::vec4> vertices = {
        // point
        glm::vec4(0, 0, 0, 1),
        // quad center, xyzw
        glm::vec4(-0.5, -0.5, 0, 1), glm::vec4(-0.5, 0.5, 0, 1), glm::vec4(0.5, -0.5, 0, 1), glm::vec4(0.5, 0.5, 0, 1),
        // quad corner, xyzw
        glm::vec4(0, 0, 0, 1), glm::vec4(0, 1, 0, 1), glm::vec4(1, 0, 0, 1), glm::vec4(1, 1, 0, 1),
        // quad center, xyuv
        glm::vec4(-0.5, -0.5, 0, 0), glm::vec4(-0.5, 0.5, 0, 1), glm::vec4(0.5, -0.5, 1, 0), glm::vec4(0.5, 0.5, 1, 1),
        // quad corner, xyuv
        glm::vec4(0, 0, 0, 0), glm::vec4(0, 1, 0, 1), glm::vec4(1, 0, 1, 0), glm::vec4(1, 1, 1, 1),
        // screen, xyuv
        glm::vec4(0, 0, 0, 0), glm::vec4(0, 1, 0, 1), glm::vec4(1, 0, 1, 0), glm::vec4(1, 1, 1, 1)};
    std::vector<glm::vec2> texcoords = {
        glm::vec2(0, 0), glm::vec2(0, 0), glm::vec2(0, 1), glm::vec2(1, 0), glm::vec2(1, 1), glm::vec2(0, 0),
        glm::vec2(0, 1), glm::vec2(1, 0), glm::vec2(1, 1), glm::vec2(0, 0), glm::vec2(0, 1), glm::vec2(1, 0),
        glm::vec2(1, 1), glm::vec2(0, 0), glm::vec2(0, 1), glm::vec2(1, 0), glm::vec2(1, 1)};

    shape.vao.setup().addBuffer(vertices).addBuffer(texcoords)();
}

bool Context::_errors(const std::string& text, const std::string& file, int line, const std::string& fun) {
    int err = gl::GetError();
    if(err != gl::NO_ERROR_) {
        console.error("GLError:", err, file + "#", line, ":", fun, "::", text);
        // hardPause();
        return true;
    }
    return false;
}

void Context::bindTexture(const Texture& t0) {
    gl::FramebufferTexture(gl::DRAW_FRAMEBUFFER, gl::COLOR_ATTACHMENT0, t0.ID, 0);
}

void Context::setupDefaultBuffer() {
    gl::Viewport(0, 0, window.size.x, window.size.y);
    gl::ClearColor(0.2f, 0.2f, 0.2f, 1.f);
    gl::ClearDepth(1);
    gl::ClearStencil(0);
    gl::Clear(gl::COLOR_BUFFER_BIT | gl::DEPTH_BUFFER_BIT);
    gl::DisableVertexAttribArray(0);
    gl::DepthFunc(gl::LEQUAL);
    gl::DepthRange(0.0f, 1.0f);
    gl::Disable(gl::BLEND);

    gl::DepthMask(gl::TRUE_);
    gl::Enable(gl::CULL_FACE);
    gl::Enable(gl::DEPTH_TEST);
    gl::FrontFace(gl::CCW);
    gl::BindTexture(gl::TEXTURE_2D, 0);

    errors();
}
void Context::setupMainFrameBuffer() {
    fbo[FULL].tex(tex.gbuffer.color).tex(tex.gbuffer.normals).tex(tex.gbuffer.depth)();

    gl::DepthRange(0.0f, 1.0f);
    gl::DepthFunc(gl::LEQUAL);
    gl::FrontFace(gl::CCW);

    gl::Disable(gl::BLEND);
    gl::DepthMask(gl::TRUE_);
    // gl::Enable(gl::CULL_FACE);
    gl::Disable(gl::CULL_FACE);
    gl::Enable(gl::DEPTH_TEST);
    gl::DisableVertexAttribArray(0);
    gl::BindTexture(gl::TEXTURE_2D, 0);

    errors();
}
void Context::setupMainFrameBuffer_onlyDiffuse() {
    fbo[FULL].tex(tex.gbuffer.color)();
    gl::DepthMask(gl::FALSE_);
    gl::Disable(gl::CULL_FACE);
    gl::Disable(gl::DEPTH_TEST);

    errors();
}
void Context::setupMainFrameBuffer_onlyDiffuseAndDepth() {
    fbo[FULL].tex(tex.gbuffer.color).tex(tex.gbuffer.depth)();
    gl::DepthMask(gl::FALSE_);
    gl::Disable(gl::CULL_FACE);
    gl::Disable(gl::DEPTH_TEST);

    errors();
}

void Context::unbindFBO() {
    fbo.unbind();
    currentFbo = 0xffffff;
}

void Context::drawScreen() {
    defaultVAO.bind();
    shape.screen.bind().attrib(0).pointer_float(4).divisor(0);
    gl::DrawArrays(gl::TRIANGLE_STRIP, 0, 4);
    gl::BindVertexArray(0);
}

void Context::beginFrame() {
    gl::Viewport(0, 0, window.size.x, window.size.y);
    gl::ClearColor(0.9, 0.88, 0.87, 1.f);
    gl::ClearDepth(1);
    gl::ClearStencil(0);
    gl::Clear(gl::COLOR_BUFFER_BIT | gl::DEPTH_BUFFER_BIT);
    gl::DisableVertexAttribArray(0);
    gl::DepthFunc(gl::LEQUAL);
    gl::DepthRange(0.0f, 1.0f);
    gl::Disable(gl::BLEND);

    gl::DepthMask(gl::TRUE_);
    gl::Enable(gl::CULL_FACE);
    gl::Enable(gl::DEPTH_TEST);
    gl::FrontFace(gl::CCW);
    gl::BindTexture(gl::TEXTURE_2D, 0);

    uploadUniforms();

    errors();
}
void Context::endFrame() {
    gl::BindBuffer(gl::ARRAY_BUFFER, 0);
    gl::DisableVertexAttribArray(0);
    fbo.unbind();

    gl::Disable(gl::DEPTH_TEST);
    gl::Disable(gl::BLEND);
    gl::Disable(gl::CULL_FACE);
    gl::DepthMask(gl::FALSE_);

    auto shader = assets::getShader("ApplyFBO");
    shader.bind();
    shader.texture("uTexture", tex.gbuffer.color, 0);
    drawScreen();

    gl::UseProgram(0);
    gl::BindVertexArray(0);
    gl::DisableVertexAttribArray(0);
}

void Context::uploadUniforms() {
    uint bindingPoint = 1;
    gl::BindBuffer(gl::UNIFORM_BUFFER, ubo.common);
    gl::BufferSubData(gl::UNIFORM_BUFFER, 0, sizeof(visuals::Uniforms), &visuals::preparedScene.uniforms);
    gl::BindBuffer(gl::UNIFORM_BUFFER, 0);
    gl::BindBufferRange(gl::UNIFORM_BUFFER, bindingPoint, ubo.common, 0,
                        sizeof(visuals::Uniforms)); // * bind ubo to binding point
}

void Context::setupFramebufferForShadowMapGeneration() {}
void Context::setupFramebufferForGBufferGeneration() {
    fbo[FULL].tex(tex.gbuffer.color).tex(tex.gbuffer.normals).tex(tex.gbuffer.depth)();

    gl::DepthRange(0.0f, 1.0f);
    gl::DepthFunc(gl::LEQUAL);
    gl::Enable(gl::DEPTH_TEST);
    gl::DepthMask(gl::TRUE_);

    gl::Disable(gl::BLEND);
    gl::FrontFace(gl::CCW);
    // gl::Enable(gl::CULL_FACE);
    gl::Disable(gl::CULL_FACE);

    gl::DisableVertexAttribArray(0);
    gl::BindTexture(gl::TEXTURE_2D, 0);

    gl::ClearColor(130 / 256.f, 140 / 256.f, 151 / 256.f, 1.f);
    gl::ClearDepth(1);
    gl::Clear(gl::COLOR_BUFFER_BIT | gl::DEPTH_BUFFER_BIT);

    defaultVAO.bind();

    errors();
}
void Context::setupFramebufferForLighting() {
    fbo[FULL].tex(tex.light.color).tex(tex.light.specular)();
    gl::ClearColor(0.0, 0.0, 0.0, 0);
    gl::Clear(gl::COLOR_BUFFER_BIT);
}
void Context::setupFramebufferForLightingWithAddionalDepth() {
    fbo[FULL].tex(tex.light.color).tex(tex.light.specular).tex(tex.gbuffer.depth);
}
void Context::setupFramebufferForLDRProcessing() {
    // na razie pusty, potem przenieść na rgb10
}

void Context::cullBackFaces() {
    gl::CullFace(gl::BACK);
}
void Context::cullFrontFaces() {
    gl::CullFace(gl::FRONT);
}

// Image splashScreen {};
// void drawSplashScreen(const std::string &filename){
//     if(splashScreen.ID) gl::DeleteTextures(1, &splashScreen.ID);
//     else {
//         ResourceLoader loader;
//         loader.reloadShader("ApplyFBO");
//         splashScreen = loader.loadImage(filename);

//         glfwShowWindow(Global::main.window);
//         glfwSetWindowPos(Global::main.window, Global::main.size.x/2.f - splashScreen.width/2, Global::main.size.y/2.f - splashScreen.height/2);
//         glfwSetWindowSize(Global::main.window, splashScreen.width, splashScreen.height);
//     }
//     gl::Viewport(0, 0, splashScreen.width, splashScreen.height);
//     gl::ClearColor(0.f, 0.f, 0.f, 0.f);
//     gl::Clear(gl::COLOR_BUFFER_BIT);
//     gl::DepthMask(gl::FALSE_);
//     gl::Enable(gl::BLEND);
//     gl::BlendFunc(gl::SRC_ALPHA, gl::ONE_MINUS_SRC_ALPHA);

//     gl::BindBuffer(gl::ARRAY_BUFFER, 0);
//     gl::DisableVertexAttribArray(0);
//     gl::BindFramebuffer(gl::DRAW_FRAMEBUFFER, 0);

//     auto shader = assets::getShader("ApplyFBO");
//     shader.bind();
//     float width = Global::main.size.x;
//     float height = Global::main.size.y;

//     shader.texture("uTexture", splashScreen.ID);

//     drawScreen();

//     glfwSwapBuffers(Global::main.window);

//     CHECK_FOR_ERRORS
// }
