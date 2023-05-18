#version 460 core

layout(location = 0) in vec3 vert_pos;
layout(location = 1) in vec2 tex_coord;

out vec2 TexCoord;

void main(){
	TexCoord = tex_coord;
	gl_Position = vec4(vert_pos.x, vert_pos.y, vert_pos.z, 1.0);
}
