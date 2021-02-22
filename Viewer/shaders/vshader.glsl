#version 330 core

layout(location = 0) in vec3 pos;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 texCoords;

// The model/view/projection matrices
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform vec3 lightPos;


// These outputs will be available in the fragment shader as inputs
out vec3 orig_fragPos;
out vec3 fragPos;
out vec3 Normal;
out vec2 fragTexCoords;
out vec3 LightPos;

void main()
{
	// This is an internal OpenGL variable, we must set a value to this variable
	gl_Position = projection * view *  model * vec4(pos, 1.0f);
	fragPos = vec3(view * model * vec4(pos, 1.0f));
	Normal = mat3(transpose(inverse(view * model))) * normal;
	LightPos = vec3(view * vec4(lightPos, 1.0));

	orig_fragPos = vec3(vec4(pos, 1.0f));

	fragTexCoords = texCoords;

}