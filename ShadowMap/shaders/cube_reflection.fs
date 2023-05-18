#version 460 core

out vec4 FragColor;

in vec3 Position;
in vec3 Normals;

uniform vec3  cameraPos;
uniform samplerCube skybox;
uniform vec2 resolution;

void main()
{    
    vec3 I = normalize(Position - cameraPos);
    vec3 R = reflect(I, normalize(Normals));

    FragColor = vec4(texture(skybox, R).rgb, 1.0);
}