#version 330 core

struct Material
{
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
	float shininess;
};

struct Light
{
	vec3 position;
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};

in vec2 TexCoord;
in vec3 frag_pos;
in vec3 normal;

uniform Material material;
uniform Light light;

uniform vec3 view_pos;

out vec4 FragColor;

void main()
{
	vec3 norm = normalize(normal);

	// Ambient
	vec3 ambient = material.ambient * light.ambient;

	// Diffuse
	vec3 light_dir = normalize(light.position - frag_pos);
	float diff = max(dot(norm, light_dir), 0.0);
	vec3 diffuse = (material.diffuse * diff) * light.diffuse;

	// Specular
	vec3 view_dir = normalize(view_pos - frag_pos);
	vec3 reflect_dir = reflect(-light_dir, norm);
	float spec = pow(max(dot(view_dir, reflect_dir), 0.0), 32);
	vec3 specular = (material.specular * spec) * light.specular;

	FragColor = vec4(ambient + diffuse + specular, 1.0);
}
