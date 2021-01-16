#version 150

out vec4 fColor;

uniform sampler2D texture;
uniform vec3 color;

in vec3 fragPos;
in vec3 fragNormal;

void main() 
{ 
   fColor = vec4(color,1);
} 

