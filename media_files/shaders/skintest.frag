#version 330 core

in vec3 NormalO;
in vec2 TexCoordO;
out vec4 FragColor;

uniform sampler2D texture_diffuse1;

void main() {
	vec3 normal = normalize(NormalO);
	// FragColor = texture(texture_diffuse1, TexCoordO);// * vec4(1,0,0,1);
	// FragColor = vec4(1,0,0,1);
	// FragColor = texture(texture_diffuse1, TexCoord0) * dot(normal, vec3(-1, -1, .0)) * 1.7 * vec4(0.65, 0.6, 0.01, 1.0);
	FragColor = dot(normal, vec3(-1, -1, .0)) * 1.7 * vec4(0.65, 0.6, 0.01, 1.0);
}