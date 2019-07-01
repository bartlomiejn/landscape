#version 330 core

in vec2 TexCoord;
in vec3 frag_pos;
in vec3 normal;

uniform vec3 object_color;

// Ambient lighting
uniform float ambient_strength;
uniform vec3 ambient_color;

// Single-source diffuse lighting
uniform vec3 light_pos;
uniform vec3 light_color;

// Specular lighting
uniform float specular_strength;
uniform vec3 view_pos;

out vec4 FragColor;

void main()
{
	vec3 norm = normalize(normal);

	// Ambient
	vec3 ambient = ambient_strength * ambient_color;

	// Diffuse
	vec3 light_dir = normalize(light_pos - frag_pos);
	float diff = max(dot(norm, light_dir), 0.0);
	vec3 diffuse = diff * light_color;

	// Specular
	vec3 view_dir = normalize(view_pos - frag_pos);
	vec3 reflect_dir = reflect(-light_dir, norm);
	float spec = pow(max(dot(view_dir, reflect_dir), 0.0), 32);
	vec3 specular = specular_strength * spec * light_color;

	FragColor = vec4((ambient + diffuse + specular) * object_color, 1.0);
}
