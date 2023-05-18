#ifndef FRAMEBUFFER_HPP
#define FRAMEBUFFER_HPP

#include <GL/glew.h>
#include <vector>
#include <iostream>

#include "Vertex.hpp"
#include "Shader.hpp"

struct Framebuffer
{
    Framebuffer() = default;
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

    void DrawFrame(Shader &shader);
};

struct PickingFramebuffer : Framebuffer
{

    PickingFramebuffer(unsigned int _w, unsigned int _h);

    GLuint color_texture;
    GLuint depth_texture;

    void Bind() const;
    void Unbind() const;

    struct PixelInfo
    {
        float ObjectID;
        float DrawID;
        float PrimID;

        PixelInfo()
        {
            ObjectID = 0.0f;
            DrawID = 0.0f;
            PrimID = 0.0f;
        }
    };

    PixelInfo ReadPixel(unsigned int x, unsigned int y) const;
        void DrawFrame(Shader &shader);

};

PickingFramebuffer::PixelInfo ReadPixelFromBuffer(GLuint buffer, unsigned int x, unsigned int y);

#endif