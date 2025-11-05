#version 330 core

// in vec4 vertexColor; // the input variable from the vertex shader (same name and same type)
out vec4 FragColor;
// in vec2 vTexCoord;
// in vec2 TexCoord0;
in vec2 TexCoord;
in vec3 Normal;
// uniform float gTrans;

uniform sampler2D texture_diffuse1;
// uniform sampler2D texture_specular1;
// uniform sampler2D texture_normal1;


void main() {
	vec3 normal = normalize(Normal);
	FragColor = texture(texture_diffuse1, TexCoord) * dot(normal, vec3(-0.8, -.8, .5)) * 1.7 * vec4(0.78, 0.8, 0.68, 1.0);
	// FragColor = mix(texture(texture_diffuse1, vTexCoord), texture(texture_normal1, vTexCoord), gTrans);
	// FragColor = mix(texture(texture_diffuse1, vTexCoord), texture(texture_specular1, vTexCoord), .5);
	// FragColor = vec4(0.8f, 0.3f, 0.2f, 1.0f);
	// FragColor = mix(texture(tex0, vTexCoord) * vec4(ourColor, 1.0), texture(tex1, vTexCoord), gTrans);
	 // FragColor = mix(texture(tex0, vTexCoord), texture(tex1, vTexCoord), gTrans);
	//FragColor = vec4(1,0,0,1);

	// FragColor = texture(texture_diffuse1, TexCoord) * dot(normal, vec3(-1, -1, 0)) * 1.7 * vec4(0.65, 0.6, 0.01, 1.0);
}