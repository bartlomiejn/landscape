#version 330 core

in vec2 tex_coord;

uniform sampler2D depth_map;

out vec4 FragColor;

void main() {
	float depth_val = texture(depth_map, tex_coord).r;
	FragColor = vec4(vec3(depth_val), 1.0);
}
