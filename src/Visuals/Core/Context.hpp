#pragma once
#include "gl_core_4_5.hpp"
#include "FBO.hpp"
#include "GPUResources.hpp"
#include "GraphicMiscellaneous.hpp"
#include "Texture.hpp"
#include "Window.hpp"

class Context
{
private:
    Window &window;
    void resetTextures();
    void resetFbo();
    void resetBuffers();
    void resetShapes();
public:
    struct {
        struct {
            Texture color;
            Texture normals;
            Texture depth;
        } gbuffer;
        struct {
            Texture color;
            Texture specular;
        } light;

        struct {
            Texture a;
            Texture b;
            Texture rg16a;
            Texture rg16b;
        } full;
        struct {
            Texture a;
            Texture b;
            Texture rg16a;
            Texture rg16b;
        } half;
        struct {
            Texture a;
            Texture b;
        } quarter;
        struct {
            Texture a;
            Texture b;
        } eight;

        struct {
            Texture cascade {};
            u32 size {4096};
            std::vector<glm::mat4> matrices;
        } shadows;

        Texture blurredScene;
        struct {
            Texture finalRenderedFrame;
            Texture full;
            struct {
                Texture a;
                Texture b;
                Texture c;
                Texture d;
                Texture wide;
            } half;
        } ldr;
        Texture terrainTopdownView;
        Texture terrainTopdownViewNormals;
    } tex;
    struct UBOs{
        u32 matrices;
        u32 common;
        const u32 size = 256;
        void update(std::vector<glm::mat4>&);
        void update(glm::mat4*, int);
    } ubo;
    // struct {
    //     GLenum drawBuffers[5];
    //     GLuint shadowmap;
    //     GLuint full;
    //     GLuint _12;
    //     GLuint _12_wide;
    //     GLuint _14;
    //     GLuint _18;
    // } fbo;

    struct {
        VAO vao;
        VBO point {};
        VBO quadCentered {}; // xyyv
        VBO quadCorner {}; // xyuv
        VBO screen {}; // xyuv
        VBO cube {};
    } shape;

    std::array<VBO,6> randomBuffers;
    u32 randomBufferId {0};
    VBO& getRandomBuffer(){
        randomBufferId = (randomBufferId+1)%6;
        return randomBuffers[randomBufferId];
    }
    VBO& getBuffer(){
        randomBufferId = (randomBufferId+1)%6;
        return randomBuffers[randomBufferId];
    }
    VAO defaultVAO;
    u32 currentFbo;
    FBO fbo;

    Context(Window &window) : window(window), fbo(window){}
    void reset();

    void beginFrame();
    void endFrame();

    void uploadUniforms();

    void setupFramebufferForShadowMapGeneration();
    void setupFramebufferForGBufferGeneration();
    void setupFramebufferForLighting();
    void setupFramebufferForLightingWithAddionalDepth();
    void setupFramebufferForLDRProcessing();

    void bindTexture(const Texture &texture);

    void setupDefaultBuffer();
    void setupMainFrameBuffer();
    void setupMainFrameBuffer_onlyDiffuse();
    void setupMainFrameBuffer_onlyDiffuseAndDepth();
    void cullBackFaces();
    void cullFrontFaces();

    void unbindFBO();
    void drawScreen();

    bool _errors(const std::string &text, const std::string &file, int line, const std::string &fun);
};
#define errors() _errors("", __FILE__, __LINE__, __FUNCTION__)
