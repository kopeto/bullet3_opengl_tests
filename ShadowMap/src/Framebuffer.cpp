#include "Framebuffer.hpp"

Framebuffer::~Framebuffer()
{
    printf("Deleting frame buffer...\n");
    glDeleteFramebuffers(1, &fbo);
}

Framebuffer::Framebuffer(unsigned int _w, unsigned int _h) : w{_w}, h{_h}
{
    // FRAMEBUFFER
    glGenFramebuffers(1, &fbo);
    while (!(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE))
        printf("ERROR::creating Framebuffer\n");
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);

    // TEXTURE
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, w, h, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture, 0);

    // RENDERBUFFER
    glGenRenderbuffers(1, &rbo);
    glBindRenderbuffer(GL_RENDERBUFFER, rbo);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, w, h);
    glBindRenderbuffer(GL_RENDERBUFFER, 0);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        printf("ERROR::creating Framebuffer2\n");

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    vertices.emplace_back(glm::vec3{-1.0, 1.0, 0.0}, glm::vec2{0.0, 1.0});
    vertices.emplace_back(glm::vec3{-1.0, -1.0, 0.0}, glm::vec2{0.0, 0.0});
    vertices.emplace_back(glm::vec3{1.0, -1.0, 0.0}, glm::vec2{1.0, 0.0});

    vertices.emplace_back(glm::vec3{-1.0, 1.0, 0.0}, glm::vec2{0.0, 1.0});
    vertices.emplace_back(glm::vec3{1.0, -1.0, 0.0}, glm::vec2{1.0, 0.0});
    vertices.emplace_back(glm::vec3{1.0, 1.0, 0.0}, glm::vec2{1.0, 1.0});

    // create buffers/arrays
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);
    // load data into vertex buffers
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    // A great thing about structs is that their memory layout is sequential for all its items.
    // The effect is that we can simply pass a pointer to the struct and it translates perfectly to a glm::vec3/2 array which
    // again translates to 3/2 floats which translates to a byte array.
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);

    // set the vertex attribute pointers
    // vertex Positions
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)0);

    // vertex normals
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)offsetof(Vertex, Normal));
    // vertex texture coords

    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)offsetof(Vertex, TexCoords));

    // // vertex tangent
    // glEnableVertexAttribArray(3);
    // glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)offsetof(Vertex, Tangent));

    // // vertex bitangent
    // glEnableVertexAttribArray(4);
    // glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)offsetof(Vertex, Bitangent));

    // // ids
    // glEnableVertexAttribArray(5);
    // glVertexAttribIPointer(5, 4, GL_INT, sizeof(Vertex), (void *)offsetof(Vertex, m_BoneIDs));

    // // weights
    // glEnableVertexAttribArray(6);
    // glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)offsetof(Vertex, m_Weights));
    // glBindVertexArray(0);
}

void Framebuffer::Bind() const
{
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    glClearColor(00.1f, 0.1f, 0.1f, 0.1f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);
}

void Framebuffer::Unbind() const
{
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Framebuffer::DrawFrame(Shader &shader)
{
    Unbind();
    glClearColor(1.0, 1.0, 1.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT);

    shader.use();
    shader.setInt("screenTexture", 0);
    shader.setVec2("resolution", glm::vec2(w, h));
    glBindVertexArray(VAO);
    glDisable(GL_DEPTH_TEST);
    glBindTexture(GL_TEXTURE_2D, texture);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glEnable(GL_DEPTH_TEST);
}

PickingFramebuffer::PickingFramebuffer(unsigned int _w, unsigned int _h)
{
    w = _w;
    h = _h;

    // Create the FBO
    glGenFramebuffers(1, &fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);

    // Create the texture object for the primitive information buffer
    glGenTextures(1, &color_texture);
    glBindTexture(GL_TEXTURE_2D, color_texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, w, h, 0, GL_RGB, GL_FLOAT, NULL);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, color_texture, 0);

    // Create the texture object for the depth buffer
    glGenTextures(1, &depth_texture);
    glBindTexture(GL_TEXTURE_2D, depth_texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, w, h, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depth_texture, 0);

    // Disable reading to avoid problems with older GPUs
    glReadBuffer(GL_NONE);
    glDrawBuffer(GL_COLOR_ATTACHMENT0);

    // Verify that the FBO is correct
    GLenum Status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if (Status != GL_FRAMEBUFFER_COMPLETE)
    {
        printf("FB error, status: 0x%x\n", Status);
        exit(1);
    }

    // Restore the default framebuffer
    glBindTexture(GL_TEXTURE_2D, 0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void PickingFramebuffer::Bind() const
{
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fbo);
}

void PickingFramebuffer::Unbind() const
{
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
}

PickingFramebuffer::PixelInfo PickingFramebuffer::ReadPixel(unsigned int x, unsigned int y) const
{
    glBindFramebuffer(GL_READ_FRAMEBUFFER, fbo);
    glReadBuffer(GL_COLOR_ATTACHMENT0);

    PixelInfo Pixel;
    glReadPixels(x, y, 1, 1, GL_RGB, GL_FLOAT, &Pixel);

    glReadBuffer(GL_NONE);
    glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);

    return Pixel;
}

void PickingFramebuffer::DrawFrame(Shader &shader)
{
    Unbind();
    glClearColor(1.0, 1.0, 1.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT);

    shader.use();
    shader.setInt("screenTexture", 0);
    shader.setVec2("resolution", glm::vec2(w, h));
    glBindVertexArray(VAO);
    glDisable(GL_DEPTH_TEST);
    glBindTexture(GL_TEXTURE_2D, color_texture);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glEnable(GL_DEPTH_TEST);
}