#version 330 core

out vec4 outColor;

in vec3 color;
in vec2 uv;

// texture sampler
uniform sampler2D albedo;

void main()
{
	vec4 samplerColor = texture2D(albedo, uv);
	outColor = samplerColor;
}