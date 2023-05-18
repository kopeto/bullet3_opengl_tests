#ifndef DEPTHBUFFER
#define DEPTHBUFFER

#include <GL/glew.h>
#include <vector>
#include <iostream>

#include "Vertex.hpp"
#include "Shader.hpp"

struct Depthbuffer
{
    Depthbuffer(unsigned int _w, unsigned int _h, unsigned int _scr_w, unsigned int _scr_h);
    ~Depthbuffer();
    GLuint fbo; // framebufferobject
    GLuint texture;

    unsigned int w, h;
    unsigned int scr_w, scr_h;

    void Bind() const;
    void Unbind() const;

    void BindTexture(Shader& shader);
    void DrawFrame(Shader &shader);
};

#endif