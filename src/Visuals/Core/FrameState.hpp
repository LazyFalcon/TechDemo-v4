struct FrameState
{
    std::optional<int> depthTest;       // * gl::LEQUAL
    int depthMask;                      // * gl::FALSE_ or gl::TRUE_
    std::optional<int> backfaceCulling; // * gl::CCW gl::CW
    FBO fbo;
    std::optional<std::reference_wrapper<Texture>> firstTexture;
    std::optional<std::reference_wrapper<Texture>> secondTexture;
    std::optional<std::reference_wrapper<Texture>> thirdTexture;
    std::optional<std::reference_wrapper<Texture>> depthTexture;
    std::array<float, 4> viewport;

    // TODO: add comparison with current state
    void bind() {
        gl::BindFramebuffer(gl::DRAW_FRAMEBUFFER, fbo.full);
        int drawbuffers = 0;
        if(firstTexture)
            gl::FramebufferTexture2D(gl::DRAW_FRAMEBUFFER, gl::COLOR_ATTACHMENT0, gl::TEXTURE_2D, firstTexture->ID, 0);
        if(secondTexture)
            gl::FramebufferTexture2D(gl::DRAW_FRAMEBUFFER, gl::COLOR_ATTACHMENT0, gl::TEXTURE_2D, secondTexture->ID, 0);
        if(thirdTexture)
            gl::FramebufferTexture2D(gl::DRAW_FRAMEBUFFER, gl::COLOR_ATTACHMENT0, gl::TEXTURE_2D, thirdTexture->ID, 0);
        if(depthTexture) {
            gl::FramebufferTexture2D(gl::DRAW_FRAMEBUFFER, gl::COLOR_ATTACHMENT0, gl::TEXTURE_2D, depthTexture->ID, 0);
            depthTest ? gl::Enable(gl::DEPTH_TEST) : gl::Disable(gl::DEPTH_TEST);
            gl::DepthMask(depthMask);
        }
        else {
            gl::Disable(gl::DEPTH_TEST);
            gl::DepthMask(gl::FALSE_);
        }

        gl::DrawBuffers(drawbuffers, fbo.drawBuffers);
        gl::Viewport(viewport[0], viewport[1], viewport[2], viewport[3]);
    }
};
