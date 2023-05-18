#version 460 core

out vec4 FragColor;

in vec3 Position;
in vec3 Normals;

uniform vec3  cameraPos;
uniform samplerCube skybox;

// RATIOS:
float air = 1.00;
float water = 1.33;
float ice = 1.309;
float glass = 1.52;
float diamond = 2.42;

void main()
{    
    float ratio = 1.00 / diamond;

    vec3 I = normalize(Position - cameraPos);
    vec3 R = refract(I, normalize(Normals), ratio );
    FragColor = vec4(texture(skybox, R).rgb, 1.0);
}