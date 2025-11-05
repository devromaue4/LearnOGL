#version 330 core

in vec3 Normal;
in vec2 TexCoordO;
out vec4 FragColor;

uniform sampler2D texture_diffuse1;

void main() {
	FragColor = texture(texture_diffuse1, TexCoordO);// * vec4(1,0,0,1);
	// FragColor = vec4(1,0,0,1);
}