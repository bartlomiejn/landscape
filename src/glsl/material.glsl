#version 330 core

in vec2 TexCoord;
in vec3 frag_pos;
in vec3 normal;

uniform vec3 object_color;

uniform float ambient_strength;
uniform vec3 ambient_color;

uniform vec3 light_pos;
uniform vec3 light_color;

out vec4 FragColor;

void main()
{
	vec3 ambient = ambient_strength * ambient_color;

	vec3 norm = normalize(normal);
	vec3 light_dir = normalize(light_pos - frag_pos);
	float diff = max(dot(norm, light_dir), 0.0);
	vec3 diffuse = diff * light_color;

	FragColor = vec4((ambient + diffuse) * object_color, 1.0);
}
