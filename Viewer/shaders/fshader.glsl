#version 330 core

struct Material
{
	sampler2D textureMap;
	// You can add more fields here...
	// Such as:
	//		1. diffuse/specular relections constants
	//		2. specular alpha constant
	//		3. Anything else you find relevant
};

// We set this field's properties from the C++ code
uniform Material material;
uniform vec3 modelColor;
uniform vec3 lightColor;
uniform int modelOrLight;

// Inputs from vertex shader (after interpolation was applied)
in vec3 fragPos;
in vec3 Normal;
in vec3 LightPos; 

in vec2 fragTexCoords;
in vec3 orig_fragPos;
// The final color of the fragment (pixel)
out vec4 frag_color;

void main()
{
	// ambient
	float ambientStrength = 0;
	vec3 ambient = ambientStrength * lightColor;    
	
	 // diffuse 
	vec3 norm = normalize(Normal);
	vec3 lightDir = normalize(LightPos - fragPos);
	float diff = max(dot(norm, lightDir), 0.0);
	vec3 diffuse = diff * 0.1 * lightColor;
	
	// specular
	float specularStrength = 1;
	vec3 viewDir = normalize(-fragPos); // the viewer is always at (0,0,0) in view-space, so viewDir is (0,0,0) - Position => -Position
	vec3 reflectDir = reflect(-lightDir, norm);  
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), 64);
	vec3 specular = specularStrength * spec * lightColor; 
	
	vec3 result = (ambient + diffuse + specular) * modelColor;
	
	// Sample the texture-map at the UV coordinates given by 'fragTexCoords'
	//vec3 textureColor = vec3(texture(material.textureMap, fragTexCoords));
	
	//frag_color = vec4(textureColor,1);
	frag_color = vec4(result, 1.0);
}
