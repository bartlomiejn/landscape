#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;
layout (location = 3) in mat4 model; // This takes 4 location slots, so next should be 7

uniform mat4 view;
uniform mat4 projection;
uniform mat4 light_space_matrix;

out vec3 normal;
out vec2 tex_coords;
out vec3 frag_pos;
out vec4 frag_pos_light_space;
flat out int instance_id;

void main()
{
	normal = transpose(inverse(mat3(model))) * aNormal;
	tex_coords = aTexCoords;
	frag_pos = vec3(model * vec4(aPos, 1.0));
	frag_pos_light_space = light_space_matrix * vec4(frag_pos, 1.0);
	gl_Position = projection * view * model * vec4(aPos, 1.0);
	instance_id = gl_InstanceID;
}
