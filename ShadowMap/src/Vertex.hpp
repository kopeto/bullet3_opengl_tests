#ifndef VERTEX_HPP
#define VERTEX_HPP

#define MAX_BONE_INFLUENCE 4

#include <glm/glm.hpp>

struct Vertex
{
    // position
    glm::vec3 Position;
    // normal
    glm::vec3 Normal;
    // texCoords
    glm::vec2 TexCoords;
    // tangent
    glm::vec3 Tangent;
    // bitangent
    glm::vec3 Bitangent;
    // bone indexes which will influence this vertex
    int m_BoneIDs[MAX_BONE_INFLUENCE];
    // weights from each bone
    float m_Weights[MAX_BONE_INFLUENCE];

    Vertex() = default;
    Vertex(glm::vec3 Position);
    Vertex(glm::vec3 Position, glm::vec2 _texCoords);
};

#endif