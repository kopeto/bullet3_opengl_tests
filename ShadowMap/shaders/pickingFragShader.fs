#version 460 core

in  vec4 color;
out vec3 FragColor;

uniform int objectId;
uniform int drawId;


void main(){
    FragColor = vec3(float(objectId), float(drawId), float(drawId));
}