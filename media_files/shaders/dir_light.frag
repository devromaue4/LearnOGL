#version 330 core

in vec3 NormalO;
in vec2 TexCoordO;
out vec4 FragColor;

uniform sampler2D texture_diffuse1;

void main() {
	// FragColor = texture(texture_diffuse1, TexCoordO);// * vec4(1,0,0,1); // only texture
	FragColor = texture(texture_diffuse1, TexCoordO) * dot(NormalO, vec3(-1, -1, .0)) * 1.7 * vec4(0.65, 0.6, 0.01, 1.0); // texture & light
	// FragColor = dot(normal, vec3(-1, -1, .0)) * 1.7 * vec4(0.65, 0.6, 0.01, 1.0); // only light
}