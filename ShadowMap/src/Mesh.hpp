#ifndef MESH_H
#define MESH_H

#include <GL/glew.h> // holds all OpenGL type declarations

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <string>
#include <vector>

#include "Shader.hpp"
#include "Texture.hpp"
#include "Vertex.hpp"


using namespace std;

struct Mesh
{
public:

    // mesh Data
    vector<Vertex> vertices;
    vector<unsigned int> indices;
    vector<Texture> textures;
    std::string name;
    unsigned int VAO;

    // render data
    unsigned int VBO, EBO;

    // constructor
    Mesh() = default;
    Mesh(vector<Vertex> vertices, vector<unsigned int> indices, vector<Texture> textures, std::string name);

    // render the mesh
    void Draw(Shader &shader) const;

    // initializes all the buffer objects/arrays
    void setupMesh();

};
#endif

struct CubeShape : Mesh {
    CubeShape();
};