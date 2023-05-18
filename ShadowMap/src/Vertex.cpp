#include "Vertex.hpp"

Vertex::Vertex(glm::vec3 _pos) : Position{_pos} , TexCoords{0.0f,0.0f} {}
Vertex::Vertex(glm::vec3 _pos, glm::vec2 _texCoords) : Position{_pos}, TexCoords{_texCoords} {}