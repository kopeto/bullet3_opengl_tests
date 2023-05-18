#ifndef FRAMEBUFFER_HPP
#define FRAMEBUFFER_HPP

#include <GL/glew.h>
#include <vector>
#include <iostream>

#include "Vertex.hpp"
#include "Shader.hpp"

struct Framebuffer
{
    Framebuffer(unsigned int _w, unsigned int _h);
    ~Framebuffer();
    GLuint fbo; // framebufferobject
    GLuint rbo; // renderbuffer object
    GLuint VAO, VBO;
    GLuint texture;

    unsigned int w, h;
    std::vector<Vertex> vertices;

    void Bind() const;
    void Unbind() const;

    void DrawFrame(Shader& shader);
};

#endif