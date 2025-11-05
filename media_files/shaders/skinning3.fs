#version 330 core

out vec4 FragColor;

in vec3 NormalO;
in vec2 TexCoordsO;

uniform sampler2D texture_diffuse1;

void main() {
	vec3 normal = normalize(NormalO);
	// FragColor = texture(texture_diffuse1, TexCoordsO) * dot(normal, vec3(-1, -1, .0)) * 1.7 * vec4(0.5, 0.5, 0.1, 1.0);
	// FragColor = texture(texture_diffuse1, TexCoordsO) * dot(normal, vec3(-1, -1, .0)) * 0.7 * vec4(1, 1, 1, 1.0);
	FragColor = dot(normal, vec3(-1, -1, .0)) * 0.7 * vec4(1, 1, 1, 1.0);
	// FragColor = vec4(1, 0, 0, 1.0); // RED
}