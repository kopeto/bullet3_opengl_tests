#version 460 core


in vec2 TexCoord;

out vec4 color;

uniform sampler2D our_texture;


void main(){
    color = texture(our_texture, TexCoord);
	//color = vec4(1.0,0.0,0.0,1.0);
}