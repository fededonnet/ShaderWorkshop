#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColor;
layout (location = 2) in vec2 aUV;

out vec3 color;
out vec2 uv;

uniform mat4 transform;
uniform mat4 view;
uniform mat4 projection;

void main()
{
	gl_Position = projection * view * transform * vec4(aPos, 1.0);
	color = aColor;
	uv = aUV;
}