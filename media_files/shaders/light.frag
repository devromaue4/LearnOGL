#version 460 core
// layout (location = 0) out vec4 out_color;

in vec3 NormalO;
in vec2 TexCoordO;
in vec3 localPosO;
in vec3 modelPos;

out vec4 FragColor;

struct Material {
	vec3 AmbientColor;
	vec3 DiffuseColor;
	vec3 SpecularColor;
};

struct BaseLight {
	vec3 Color;
	float AmbientIntensity;
	float DiffuseIntensity;
};

struct DirectLight {
	BaseLight Base;
	vec3 Direction;
};

struct Attenuation {
	float Constant;
	float Linear;
	float Exp;
};

struct PointLight {
	BaseLight Base;
	vec3 Pos;
	Attenuation Atten;
};

struct SpotLight {
	PointLight Base;
	vec3 Direction;
	float Cutoff;
};

// uniform vec3 lightDir;
uniform vec3 gCamPos;

layout(binding = 0) uniform sampler2D texture_diffuse1;
layout(binding = 6) uniform sampler2D texture_specular1;

const int MAX_POINT_LIGHTS = 3;
const int MAX_SPOT_LIGHTS = 2;

uniform DirectLight gDirLight;
uniform Material gMaterial;
uniform int gNumPointLights;
uniform int gNumSpotLights;
uniform PointLight gPointLights[MAX_POINT_LIGHTS];
uniform SpotLight gSpotLights[MAX_SPOT_LIGHTS];
uniform bool gUseTextures = true;

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
	// FragColor = texture(texture_diffuse1, TexCoordO) * resClr * (diffLight + ambient + specular);

	// vec3 lDir = normalize(lightDir);// - modelPos);
	// float diffL = dot(NormalO, lDir);
	// FragColor = whiteClr * diffL;
	// FragColor = texture(texture_diffuse1, TexCoordO) * whiteClr * diffL;
// }

vec4 CalcLightInternal(BaseLight Light, vec3 LightDirection, vec3 Normal) {
	vec4 AmbientColor = vec4(Light.Color, 1.0f) * 
						Light.AmbientIntensity * 
						vec4(gMaterial.AmbientColor, 1.0f);

	vec3 lightDir = normalize(LightDirection);
	
	float diff_fact = max(dot(Normal, lightDir), 0.0);
	vec4 DiffuseColor = vec4(Light.Color, 1.0f) * Light.DiffuseIntensity *
					vec4(gMaterial.DiffuseColor, 1.0) * diff_fact;

	// vec3 pixelToCamera = normalize(gCamPos - localPosO);
	vec3 pixelToCamera = normalize(gCamPos - modelPos);
	vec3 lightReflect = reflect(-lightDir, Normal);
	float spec_fact = max(dot(pixelToCamera, lightReflect), 0.0);
	float specExp = texture(texture_specular1, TexCoordO).r * 255.0;
	spec_fact = pow(spec_fact, specExp); // use map
	// spec_fact = pow(spec_fact, 50); // use const
	vec4 SpecularColor = vec4(Light.Color, 1.0f) * vec4(gMaterial.SpecularColor, 1.0) * spec_fact;

	//////////////////////////////////////////////////////////////////
	// my solution
	// vec3 normal = normalize(NormalO);
	// vec3 lightDir = normalize(gDirLight.Direction);
	// vec3 viewDir = normalize(gCamPos - modelPos);
	// vec3 reflect = reflect(-lightDir, normal);
	// // float specular = pow(max(dot(viewDir, reflect), 0.0), 100.0);
	// float specular = pow(max(dot(viewDir, reflect), 0.0), 50.0);
	// float specular = pow(max(dot(viewDir, reflect), 0.0), texture(texture_specular1, TexCoordO).r * 255.0);
	//////////////////////////////////////////////////////////////////

	// FragColor = texture(texture_diffuse1, TexCoordO) * (AmbientColor + DiffuseColor + SpecularColor);
	// return (AmbientColor);
	return (DiffuseColor + SpecularColor);
	// return (AmbientColor);// + DiffuseColor + SpecularColor);

	// cherno tuts
	// float intensity = max(dot(normalize(NormalO), normalize(lightDir)), 0.15);
	// out_color = texture(texture_diffuse1, TexCoordO) * intensity;
}

vec4 CalcPointLight(PointLight pl, vec3 Normal) {
	vec3 LightDir = pl.Pos - modelPos;
	float Distance= length(LightDir);
	LightDir = normalize(LightDir);

	vec4 Color = CalcLightInternal(pl.Base, LightDir, Normal);
	float Attenuation = pl.Atten.Constant + 
						pl.Atten.Linear * Distance + 
						pl.Atten.Exp * Distance * Distance;

	return Color / Attenuation;
}

vec4 CalcSpotLight(SpotLight sl, vec3 Normal) {
	// vec3 LightToPixel = normalize(sl.Base.Pos - modelPos);
	vec3 LightToPixel = normalize(modelPos - sl.Base.Pos);
	float SpotFactor = dot(LightToPixel, sl.Direction);
	// float SpotFactor = dot(sl.Direction, LightToPixel);

	if(SpotFactor > sl.Cutoff) {
		vec4 Color = CalcPointLight(sl.Base, Normal);
		float SpotIntensity = (1.0 - (1.0 - SpotFactor) / (1.0 - sl.Cutoff));
		return Color * SpotIntensity;
	}
	else 
		return vec4(0,0,0,0);
}

void main() {
	vec3 normal = normalize(NormalO);
	vec4 TotalLight = CalcLightInternal(gDirLight.Base, gDirLight.Direction, normal); // dir light
	// vec4 TotalLight = vec4(0); // dir light off

	for(int i = 0; i < gNumPointLights; i++) TotalLight += CalcPointLight(gPointLights[i], normal);
	for(int i = 0; i < gNumSpotLights; i++) TotalLight += CalcSpotLight(gSpotLights[i], normal);

	FragColor = gUseTextures ? texture(texture_diffuse1, TexCoordO) * TotalLight : TotalLight;
}