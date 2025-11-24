#pragma once
#include "core.h"

class BaseLight {
public:
	glm::vec3 m_Color = glm::vec3(1.0f);
	float m_AmbientIntesity = 0.0f;
	float m_DiffuseIntesity = 0.0f;

	BaseLight() {}
};

class DirectLight : public BaseLight {
public:
	glm::vec3 m_WorldDir = glm::vec3(0.0f);
};

struct LightAttenuation {
	float Constant = 1.0f;
	float Linear = 0.0f;
	float Exp = 0.0f;
};

class PointLight : public BaseLight {
public:
	glm::vec3 m_WorldPos = glm::vec3(0.0f);

	LightAttenuation Attenuation;
};

class SpotLight : public PointLight {
public:
	glm::vec3 m_WorldDir = glm::vec3(0.0f);
	float Cutoff = 0.0f;
};