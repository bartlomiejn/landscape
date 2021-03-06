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

vec2 poisson_disk[16] = vec2[](
	vec2( -0.94201624, -0.39906216 ),
	vec2( 0.94558609, -0.76890725 ),
	vec2( -0.094184101, -0.92938870 ),
	vec2( 0.34495938, 0.29387760 ),
	vec2( -0.91588581, 0.45771432 ),
	vec2( -0.81544232, -0.87912464 ),
	vec2( -0.38277543, 0.27676845 ),
	vec2( 0.97484398, 0.75648379 ),
	vec2( 0.44323325, -0.97511554 ),
	vec2( 0.53742981, -0.47373420 ),
	vec2( -0.26496911, -0.41893023 ),
	vec2( 0.79197514, 0.19090188 ),
	vec2( -0.24188840, 0.99706507 ),
	vec2( -0.81409955, 0.91437590 ),
	vec2( 0.19984126, 0.78641367 ),
	vec2( 0.14383161, -0.14100790 )
);

/// Generates a pseudo-random number in range of [0, 1]
float pseudo_random(vec4 seed4)
{
	float dot_product = dot(seed4, vec4(12.9898, 78.233, 45.164, 94.673));
	return fract(sin(dot_product) * 43758.5453);
}

float is_in_shadow(vec4 frag_pos_light_space)
{
	// Perform perspective divide (does nothing on ortographic projection),
	// calculates fragment's light space pos in [-1, 1] range
	vec3 proj_coords = frag_pos_light_space.xyz / frag_pos_light_space.w;

	// Convert from [-1, 1] to [0, 1]
	proj_coords = proj_coords * 0.5 + 0.5;

	// Correct shadow acne by adding an error margin and get current depth value
	float bias = 0.005;
	float current_depth = proj_coords.z - bias;

	if (
		proj_coords.z > 1.0 || proj_coords.x > 1.0 || proj_coords.x < 0.0
		|| proj_coords.y > 1.0 || proj_coords.y < 0.0
	)
		return 0.0;

	float visibility = 0.0;
	for (int i = 0; i < 4; i++)
	{
		// Randomise the sample from the Poisson disk
		// Can just use `i` for non-randomized samples
		float random = pseudo_random(vec4(frag_pos, i));
		int index = int(16.0 * random) % 16;

		// Sample the disk
		vec2 offset = poisson_disk[index] / 700.0;

		// Correct the issue with sampling near the border
		vec2 sample_pos = clamp(
			proj_coords.xy + offset,
			vec2(0.001, 0.001), vec2(0.999, 0.999));

		// Get the closest fragment depth from the depth buffer
		float closest_depth = texture(shadow_map, sample_pos).r;
		if (current_depth > closest_depth)
			visibility += 0.25;
	}

	return visibility;
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
	float spec = pow(max(dot(view_dir, reflect_dir), 0.0), material.shininess);
	vec3 specular =
		vec3(texture(material.specular, tex_coords))
		* spec
		* light.specular;

	float shadow = is_in_shadow(frag_pos_light_space);
	return (ambient + (1.0 - shadow) * (diffuse + specular));
//	return ambient + diffuse + specular;
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
	float spec = pow(max(dot(view_dir, reflect_dir), 0.0), material.shininess);
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

	// TODO: Intensity (cutoff) calculation is broken
	float epsilon = light.cut_off_cos - light.outer_cut_off_cos;
	float intensity =
		clamp((theta_cos - light.outer_cut_off_cos) / epsilon, 0.0, 1.0);

	// Diffuse
	float diff = max(dot(light_dir, normal), 0.0);
	vec3 diffuse = vec3(texture(material.diffuse, tex_coords))
		* diff
		* light.diffuse
		* attenuation;
//		* intensity;

	// Specular
	float spec = pow(max(dot(view_dir, reflect_dir), 0.0), material.shininess);
	vec3 specular = vec3(texture(material.specular, tex_coords))
		* spec
		* light.specular
		* attenuation;
//		* intensity;

	return (ambient + diffuse + specular);
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
	for (int i = 0; i < spot_light_count; i++)
		output_color +=
			spot_light_contribution(spot_lights[0], norm, view_dir, frag_pos);
	FragColor = vec4(output_color, 1.0);
}
