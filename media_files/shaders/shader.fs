#version 330 core

// in vec4 vertexColor; // the input variable from the vertex shader (same name and same type)
out vec4 FragColor;
// uniform vec4 ourColor;
// in vec3 ourColor;
// in vec3 ourPos;
in vec2 vTexCoord;
uniform float gTrans;

// uniform sampler2D tex0;
// uniform sampler2D tex1;
uniform sampler2D texture_diffuse1;
// uniform sampler2D texture_diffuse3;
uniform sampler2D texture_specular1;
// uniform sampler2D texture_specular2;
// uniform sampler2D texture_normal1;
// uniform sampler2D texture_normal2;

void main() {
	// FragColor = texture(texture_specular1, vTexCoord);
	// FragColor = mix(texture(texture_diffuse1, vTexCoord), texture(texture_normal1, vTexCoord), gTrans);
	FragColor = mix(texture(texture_diffuse1, vTexCoord), texture(texture_specular1, vTexCoord), gTrans);
	// FragColor = texture(texture_diffuse1, vTexCoord);
	// FragColor = vec4(0.8f, 0.3f, 0.2f, 1.0f);
	// FragColor = vec4(ourColor, 1.0);
	// FragColor = vec4(ourPos, 1.0f);
	// FragColor = mix(texture(tex0, vTexCoord) * vec4(ourColor, 1.0), texture(tex1, vTexCoord), gTrans);
	 // FragColor = mix(texture(tex0, vTexCoord), texture(tex1, vTexCoord), gTrans);
	//FragColor = vec4(1,0,0,1);
}