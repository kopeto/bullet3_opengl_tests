#include "Mesh.hpp"

// constructor
Mesh::Mesh(vector<Vertex> vertices, vector<unsigned int> indices, vector<Texture> textures, std::string name)
{
    this->vertices = vertices;
    this->indices = indices;
    this->textures = textures;
    this->name = name;

    // now that we have all the required data, set the vertex buffers and its attribute pointers.
    setupMesh();
}

// render the mesh
void Mesh::Draw(Shader &shader) const
{
    // bind appropriate textures
    unsigned int diffuseNr = 1;
    unsigned int specularNr = 1;
    unsigned int normalNr = 1;
    unsigned int heightNr = 1;
    for (unsigned int i = 0; i < textures.size(); i++)
    {
        glActiveTexture(GL_TEXTURE0 + i); // active proper texture unit before binding
        // retrieve texture number (the N in diffuse_textureN)
        string number;
        string name = textures[i].type;
        if (name == "texture_diffuse")
            number = std::to_string(diffuseNr++);
        else if (name == "texture_specular")
            number = std::to_string(specularNr++); // transfer unsigned int to string
        else if (name == "texture_normal")
            number = std::to_string(normalNr++); // transfer unsigned int to string
        else if (name == "texture_height")
            number = std::to_string(heightNr++); // transfer unsigned int to string

        // now set the sampler to the correct texture unit
        glUniform1i(glGetUniformLocation(shader.ID, (name + number).c_str()), i);
        // and finally bind the texture
        glBindTexture(GL_TEXTURE_2D, textures[i].id);
    }

    // draw mesh
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, static_cast<unsigned int>(indices.size()), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);

    // always good practice to set everything back to defaults once configured.
    //glActiveTexture(GL_TEXTURE0);
}

// initializes all the buffer objects/arrays
void Mesh::setupMesh()
{
    // create buffers/arrays
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);
    // load data into vertex buffers
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    // A great thing about structs is that their memory layout is sequential for all its items.
    // The effect is that we can simply pass a pointer to the struct and it translates perfectly to a glm::vec3/2 array which
    // again translates to 3/2 floats which translates to a byte array.
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

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
    // vertex tangent
    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)offsetof(Vertex, Tangent));
    // vertex bitangent
    glEnableVertexAttribArray(4);
    glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)offsetof(Vertex, Bitangent));
    // ids
    glEnableVertexAttribArray(5);
    glVertexAttribIPointer(5, 4, GL_INT, sizeof(Vertex), (void *)offsetof(Vertex, m_BoneIDs));

    // weights
    glEnableVertexAttribArray(6);
    glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)offsetof(Vertex, m_Weights));
    glBindVertexArray(0);
}

CubeShape::CubeShape()
{
    float cubeVertices[24] =
        {
            //FRONT FACE
            -1.0,
            -1.0,
            -1.0,

            1.0,
            -1.0,
            -1.0,

            -1.0,
            1.0,
            -1.0,

            1.0,
            1.0,
            -1.0,

            // BACK
            -1.0,
            -1.0,
            1.0,

            1.0,
            -1.0,
            1.0,

            -1.0,
            1.0,
            1.0,

            1.0,
            1.0,
            1.0,
        };

    unsigned int cubeIndices[36] =
        {
            0, 2, 1,
            1, 2, 3, // bottom
            4, 5, 6,
            5, 7, 6, // top
            4, 0, 5,
            5, 0, 1, // front
            6, 7, 2,
            7, 3, 2, // back
            6, 2, 0,
            4, 6, 0, // left
            7, 5, 1,
            7, 1, 3 // right
        };

    float cubeUVs[16] =
    {   
        0, 0, //
        25, 0, //
        0, 0, //
        25, 0, //
        0, 25, //
        25, 1, //
        0, 25, //
        25, 25  //
    };

    for (unsigned int i = 0; i < 8; ++i)
    {
        vertices.push_back(Vertex(glm::vec3(cubeVertices[i*3], cubeVertices[i*3 + 1], cubeVertices[i*3 + 2])));
        vertices.back().TexCoords = {cubeUVs[i*2], cubeUVs[i*2+1]};
    }
    for (unsigned int i = 0; i < 36; ++i)
    {
        indices.push_back(cubeIndices[i]);
    }

    // now that we have all the required data, set the vertex buffers and its attribute pointers.
    setupMesh();
}