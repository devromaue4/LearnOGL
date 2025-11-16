#version 460 core
layout (location = 0) out vec4 out_color;

in vec3 NormalO;
in vec2 TexCoordO;
in vec3 modelPos;

// out vec4 FragColor;

uniform vec3 lightDir;
uniform vec3 camPos;

layout(binding = 0) uniform sampler2D texture_diffuse1;

struct BaseLight {
	vec3 Color;
	float AmbientIntensity;
};

struct Material {
	vec3 AmbientColor;
};

uniform BaseLight gLight;
uniform Material gMaterial;

// void main() {
	// float ambient = 0.2f;
	// vec3 lightDir = normalize(lightDir - crntPos);
	// float diffLight = max(dot(normalize(NormalO), lightDir), 0.0f);

	// float specLight = 0.5f;
	// vec3 vewDir = normalize(camPos - modelPos);
	// vec3 reflectDir = reflect(-lightDir, NormalO);
	// float specAmount = pow(max(dot(vewDir, reflectDir), 0.0f), 8);
	// float specular = specAmount * specLight;

	// vec4 lightClr = vec4(0.0f, 0.42f, 0.18f, 1.0f);
	// vec4 toylClr = vec4(1.0f, 0.5f, 0.31f, 1.0f);
	// vec4 whiteClr = vec4(1.0f, 1.0f, 1.0f, 1.0f);
	// vec4 whiteClr = vec4(.7f, .7f, 0.5f, 1.0f);
	// vec4 resClr = lightClr * toylClr;
	// FragColor = resClr * (diffLight + ambient + specular);
	// FragColor = texture(texture_diffuse1, TexCoordO);
	// FragColor = texture(texture_diffuse1, TexCoordO) * resClr * (diffLight + ambient + specular);
	// FragColor = texture(texture_diffuse1, TexCoordO) * vec4(1,1,1,1) * (diffLight + ambient + specular);
	// FragColor = dot(NormalO, vec3(-1, -1, .0)) * 1.7 * vec4(0.65, 0.6, 0.01, 1.0); // only light

	// vec3 lDir = normalize(lightDir);// - modelPos);
	// vec3 lDir = normalize(lightDir);
	// float diffL = dot(NormalO, lDir);
	// FragColor = whiteClr * diffL;
	// FragColor = texture(texture_diffuse1, TexCoordO);
	// FragColor = texture(texture_diffuse1, TexCoordO) * whiteClr * diffL;
// }

void main() {
	// vec4 ambient = vec4(1.0, 1.0, 1.0, 1.0);
	// float diffuse = max(dot(NormalO, normalize(lightDir)), 0.2);
	// float diffuse = dot(NormalO, normalize(lightDir - modelPos));

	// spec light
	// float specLight = 0.5f;
	// vec3 vewDir = normalize(camPos - modelPos);
	// vec3 reflectDir = reflect(normalize(-lightDir), NormalO);
	// float specAmount = pow(max(dot(vewDir, reflectDir), 0.0f), 8);
	// float specular = specAmount * specLight;

	// FragColor = texture(texture_diffuse1, TexCoordO) * ambient * diffuse;// + specAmount;
	// FragColor = ambient + diffuse + specAmount;

	// FragColor = texture(texture_diffuse1, TexCoordO) * vec4(gMaterial.AmbientColor, 1.0f) * vec4(gLight.Color, 1.0f) * gLight.AmbientIntensity;

	// FragColor = texture(texture_diffuse1, TexCoordO) * vec4(gMaterial.AmbientColor, 1.0f) * vec4(gLight.Color, 1.0f) * gLight.AmbientIntensity;
	
	// float AmbientIntensity = 0.1;
	// // vec3 ambient = gLight.Color * gLight.AmbientIntensity;
	// vec3 ambient = gLight.Color * AmbientIntensity;
	// vec4 result = vec4(ambient * gMaterial.AmbientColor, 1.0f);
	// FragColor = result;

	float intensity = max(dot(normalize(NormalO), normalize(lightDir)), 0.15);
	out_color = texture(texture_diffuse1, TexCoordO) * intensity;
}