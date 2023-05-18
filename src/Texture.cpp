#include "Texture.hpp"

Texture::Texture(const std::string &filename, bool fliptex)
{
    id = TextureFromFile(filename, fliptex);
}

Texture::Texture(const char *path, const std::string &directory, bool fliptex)
{
    id = TextureFromFile(path, directory, fliptex);
    path = path;
};

Texture::Texture(std::vector<std::string> &faces)
{
    id = loadCubemap(faces);
};

GLuint Texture::TextureFromFile(const std::string &filename, bool fliptex, bool gamma)
{
    unsigned int textureID;
    glGenTextures(1, &textureID);

    // tell stb_image.h to flip loaded texture's on the y-axis (before loading model).
    if (fliptex)
        stbi_set_flip_vertically_on_load(true);
    else
        stbi_set_flip_vertically_on_load(false);

    int width, height, nrComponents;
    unsigned char *data = stbi_load(filename.c_str(), &width, &height, &nrComponents, 0);
    if (data)
    {
        GLenum format;
        if (nrComponents == 1)
            format = GL_RED;
        else if (nrComponents == 3)
            format = GL_RGB;
        else if (nrComponents == 4)
            format = GL_RGBA;

        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(data);
    }
    else
    {
        std::cout << "Texture failed to load at path: " << path << std::endl;
        stbi_image_free(data);
    }

    type = "texture_diffuse";

    return textureID;
}

GLuint Texture::TextureFromFile(const char *path, const std::string &directory, bool fliptex, bool gamma)
{
    std::string filename = std::string(path);
    filename = directory + '/' + filename;

    return TextureFromFile(filename, fliptex);
}

GLuint Texture::loadCubemap(std::vector<std::string> &faces)
{
    GLuint textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

    int w, h, nrChannels;

    for (unsigned int i = 0; i < faces.size(); ++i)
    {
        unsigned char *data = stbi_load(faces[i].c_str(), &w, &h, &nrChannels, 0);
        if (data)
        {
            GLenum format;
            if (nrChannels == 1)
                format = GL_RED;
            else if (nrChannels == 3)
                format = GL_RGB;
            else if (nrChannels == 4)
                format = GL_RGBA;

            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, w, h, 0, format, GL_UNSIGNED_BYTE, data);
        }
        else
        {
            std::cout << "ERROR: Cubemap failed to load at path: " << faces[i] << "\n";
            stbi_image_free(data);
        }
    }

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_LINEAR);

    return textureID;
}