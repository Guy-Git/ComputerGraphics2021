#version 330 core

layout(location = 0) in vec3 pos;
layout(location = 1) in vec3 normal;

// The model/view/projection matrices
uniform mat4 model;
uniform mat4 camera;

// These outputs will be available in the fragment shader as inputs

out vec3 fragPos;
out vec3 fragNormal;

void main()
{
	vec4 position = vec4(model * vec4(pos,1.0f));

	fragPos = vec3(position) / position.w;
	fragNormal = vec3(model * vec4(normal,0.0f));

	gl_Position = camera * model * vec4(pos, 1.0f);
	gl_Position = gl_Position / gl_Position.w;
}