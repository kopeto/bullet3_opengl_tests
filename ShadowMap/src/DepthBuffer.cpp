#include "DepthBuffer.hpp"

Depthbuffer::Depthbuffer(unsigned int _w, unsigned int _h, unsigned int _scr_w, unsigned int _scr_h)
    : w{_w}, h{_h}, scr_w{_scr_w}, scr_h{_scr_h}
{
    // FRAMEBUFFER
    glGenFramebuffers(1, &fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);

    // TEXTURE
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, w, h, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, texture, 0);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
    GLenum Status = glCheckFramebufferStatus(GL_FRAMEBUFFER);

    if ( Status != GL_FRAMEBUFFER_COMPLETE)
    {
        printf("FB error, status: 0x%x\n", Status);
        exit(1);
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

Depthbuffer::~Depthbuffer()
{
    printf("Deleting depthbuffer...\n");
    glDeleteFramebuffers(1, &fbo);
}

void Depthbuffer::Bind() const
{
    glViewport(0, 0, w, h);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    glClear(GL_DEPTH_BUFFER_BIT);
    glCullFace(GL_FRONT);
}

void Depthbuffer::Unbind() const
{
    glCullFace(GL_BACK);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glViewport(0, 0, scr_w, scr_h);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Depthbuffer::BindTexture(Shader &shader)
{
    shader.use();
    shader.setInt("shadowMap", 1);
    // and finally bind the texture
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, texture);
}