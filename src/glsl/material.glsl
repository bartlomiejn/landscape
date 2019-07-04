#version 330 core

#define MAX_POINT_LIGHTS 	8
#define MAX_SPOT_LIGHTS 	8

struct Material
{
	sampler2D diffuse;
	sampler2D specular;
	float shininess;
};

struct DirLight
{
	vec3 direction;
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};

struct PointLight
{
	vec3 position;
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
	float constant;
	float linear;
	float quadratic;
};

struct SpotLight
{
	vec3 position;
	vec3 direction;
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
	float cut_off_cos;
	float outer_cut_off_cos;
	float constant;
	float linear;
	float quadratic;
};

in vec2 tex_coords;
in vec3 frag_pos;
in vec3 normal;

uniform vec3 view_pos;
uniform Material material;
uniform DirLight dir_light;
uniform PointLight pt_lights[MAX_POINT_LIGHTS];
uniform SpotLight spot_lights[MAX_POINT_LIGHTS];
uniform int pt_light_count;
uniform int spot_light_count;

out vec4 FragColor;

vec3 dir_light_contribution(DirLight light, vec3 normal, vec3 view_dir)
{
	vec3 light_dir = normalize(-light.direction);
	vec3 reflect_dir = reflect(-light_dir, normal);

	// Ambient
	vec3 ambient = light.ambient * vec3(texture(material.diffuse, tex_coords));

	// Diffuse
	float diff = max(dot(normal, light_dir), 0.0);
	vec3 diffuse = vec3(texture(material.diffuse, tex_coords))
		* diff
		* light.diffuse;

	// Specular
	float spec = pow(max(dot(view_dir, reflect_dir), 0.0), 32);
	vec3 specular =
		vec3(texture(material.specular, tex_coords))
		* spec
		* light.specular;

	return (ambient + diffuse + specular);
}

vec3 pt_light_contribution(
	PointLight light, vec3 normal, vec3 view_dir, vec3 frag_pos
){
	vec3 light_dir = normalize(light.position - frag_pos);
	vec3 reflect_dir = reflect(-light_dir, normal);
	float distance = length(light.position - frag_pos);
	float attenuation = 1.0 / (light.constant + light.linear * distance
	+ light.quadratic * (distance * distance));

	// Ambient
	vec3 ambient = vec3(texture(material.diffuse, tex_coords))
		* light.ambient
		* attenuation;

	// Diffuse
	float diff = max(dot(normal, light_dir), 0.0);
	vec3 diffuse = vec3(texture(material.diffuse, tex_coords))
		* diff
		* light.diffuse
		* attenuation;

	// Specular
	float spec = pow(max(dot(view_dir, reflect_dir), 0.0), 32);
	vec3 specular = vec3(texture(material.specular, tex_coords))
		* spec
		* light.specular
		* attenuation;

	return (ambient + diffuse + specular);
}

vec3 spot_light_contribution(
SpotLight light, vec3 normal, vec3 view_dir, vec3 frag_pos
){
	vec3 light_dir = normalize(light.position - frag_pos);
	vec3 reflect_dir = reflect(-light_dir, normal);

	// Light falloff
	float distance = length(light.position - frag_pos);
	float attenuation = 1.0 / (light.constant + light.linear * distance
		+ light.quadratic * (distance * distance));

	// Ambient
	vec3 ambient = vec3(texture(material.diffuse, tex_coords))
		* light.ambient
		* attenuation;

	// Angle between spotlight direction and fragment vector
	// Dot product on two vectors results in vector lengths multiplied by cosine
	// of the angle between vectors - if we use normalized vectors, we're left
	// with just the cosine.
	float theta_cos = dot(light_dir, normalize(-light.direction));

	float epsilon = light.cut_off_cos - light.outer_cut_off_cos;
	float intensity =
		clamp((theta_cos - light.outer_cut_off_cos) / epsilon, 0.0, 1.0);

	// Diffuse
	float diff = max(dot(normal, light_dir), 0.0);
	vec3 diffuse = vec3(texture(material.diffuse, tex_coords))
		* diff
		* light.diffuse
		* attenuation
		* intensity;

	// Specular
	float spec = pow(max(dot(view_dir, reflect_dir), 0.0), 32);
	vec3 specular = vec3(texture(material.specular, tex_coords))
		* spec
		* light.specular
		* attenuation
		* intensity;

	return ambient + diffuse + specular;
}

void main()
{
	vec3 norm = normalize(normal);
	vec3 view_dir = normalize(view_pos - frag_pos);
	vec3 output_color = vec3(0.0);
	output_color += dir_light_contribution(dir_light, norm, view_dir);
	for (int i = 0; i < pt_light_count; i++)
		output_color +=
			pt_light_contribution(pt_lights[i], norm, view_dir, frag_pos);
	for (int i = 0; i < spot_light_count; i++)
		output_color +=
			spot_light_contribution(spot_lights[i], norm, view_dir, frag_pos);
	FragColor = vec4(output_color, 1.0);
}
