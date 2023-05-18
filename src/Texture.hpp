#ifndef TEXTURE_HPP
#define TEXTURE_HPP

#include <string>
#include <vector>
#include <iostream>

#include <GL/glew.h>
#include <stb/stb_image.h>

struct Texture
{
    GLuint id;
    std::string type;
    std::string path;
    std::string name;

    Texture() = default;

    Texture(const std::string& filename, bool fliptex);
    Texture(const char *path, const std::string &directory, bool fliptex);
    Texture(std::vector<std::string>& faces);

    private:

    GLuint TextureFromFile(const char *path, const std::string &directory, bool fliptex, bool gamma = false);
    GLuint TextureFromFile(const std::string& filename, bool fliptex, bool gamma = false);
    GLuint loadCubemap(std::vector<std::string>& faces);
};

#endif