#version 460 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;

out vec2 TexCoords;
out vec3 Normals;
out vec3 Position;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    Normals     =   mat3(transpose(inverse(model))) * aNormal;
    Position    =   vec3(model * vec4(aPos, 1.0));
    TexCoords   =   aTexCoords;    


    gl_Position = projection * view * vec4(Position, 1.0);
}