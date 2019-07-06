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

in vec3 normal;
in vec2 tex_coords;
in vec3 frag_pos;
in vec4 frag_pos_light_space;

uniform vec3 light_pos;
uniform sampler2D shadow_map;
uniform vec3 view_pos;
uniform Material material;
uniform DirLight dir_light;
uniform int is_dir_light;
uniform PointLight pt_lights[MAX_POINT_LIGHTS];
uniform SpotLight spot_lights[MAX_SPOT_LIGHTS];
uniform int pt_light_count;
uniform int spot_light_count;

out vec4 FragColor;

vec2 poissonDisk[4] = vec2[](
	vec2(-0.94201624, -0.39906216),
	vec2(0.94558609, -0.76890725),
	vec2(-0.094184101, -0.92938870),
	vec2(0.34495938, 0.29387760)
);

float is_in_shadow(vec4 frag_pos_light_space)
{
	// Perform perspective divide (does nothing on ortographic projection),
	// calculates fragment's light space pos in [-1, 1] range
	vec3 proj_coords = frag_pos_light_space.xyz / frag_pos_light_space.w;

	// Convert from [-1, 1] to [0, 1]
	proj_coords = proj_coords * 0.5 + 0.5;

	// Get the transformed fragment depth from the depth buffer
	float closest_depth = texture(shadow_map, proj_coords.xy).r;
	float current_depth = proj_coords.z;

	// Correct shadow acne by adding an error margin
	float bias = 0.005;

	float shadow = (current_depth - bias) > closest_depth ? 1.0 : 0.0;

	if (proj_coords.z > 1.0)
		shadow = 1.0;

	return shadow;
}

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

	float shadow = is_in_shadow(frag_pos_light_space);

	// Diffuse
	float diff = max(dot(light_dir, normal), 0.0);
	vec3 diffuse = vec3(texture(material.diffuse, tex_coords))
		* diff
		* light.diffuse
		* attenuation;
//		* intensity;

	// Specular
	float spec = pow(max(dot(view_dir, reflect_dir), 0.0), 32);
	vec3 specular = vec3(texture(material.specular, tex_coords))
		* spec
		* light.specular
		* attenuation;
//		* intensity;

	return (ambient + (1.0 - shadow) * (diffuse + specular));
}

void main()
{
	vec3 norm = normalize(normal);
	vec3 view_dir = normalize(view_pos - frag_pos);
	vec3 output_color = vec3(0.0);
	if (is_dir_light > 0)
		output_color += dir_light_contribution(dir_light, norm, view_dir);
	for (int i = 0; i < pt_light_count; i++)
		output_color +=
			pt_light_contribution(pt_lights[i], norm, view_dir, frag_pos);
//	for (int i = 0; i < spot_light_count; i++)
		output_color +=
			spot_light_contribution(spot_lights[0], norm, view_dir, frag_pos);
	FragColor = vec4(output_color, 1.0);
}
