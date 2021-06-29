#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColor;
layout (location = 2) in vec2 aUV;
layout (location = 3) in vec3 aNormal;
layout (location = 4) in vec3 aTangent;
layout (location = 5) in vec3 aBitangent;

out vec3 color;
out vec2 uv;
out vec3 fragmentWorldPosition;
out mat3 TBN;

uniform mat4 transform;
uniform mat4 view;
uniform mat4 projection;

void main()
{
	gl_Position = projection * view * transform * vec4(aPos, 1.0);
	color = aColor;
	uv = aUV;
	
	fragmentWorldPosition = vec3(transform * vec4(aPos, 1.0));
	
	// TBN
	vec3 T = normalize(mat3(transform) * aTangent);
    vec3 B = normalize(mat3(transform) * aBitangent);
	vec3 N = normalize(mat3(transform) * aNormal);
	mat3 _TBN = mat3(T, B, N);
	
	TBN = _TBN;
}