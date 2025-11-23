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

	//void calcLocalDir(const glm::mat4& world) {
	//	glm::mat3 world3;// = world;
	//	world3[0][0] = world[0][0]; world3[0][1] = world[0][1]; world3[0][2] = world[0][2];
	//	world3[1][0] = world[1][0]; world3[1][1] = world[1][1]; world3[1][2] = world[1][2];
	//	world3[2][0] = world[2][0]; world3[2][1] = world[2][1]; world3[2][2] = world[2][2];
	//
	//	//glm::mat3 world3(world);
	//	world3 = glm::transpose(world3);
	//	//world3 = glm::transpose(glm::inverse(world3));
	//	m_LocalDir = world3 * m_WorldDir;
	//	m_LocalDir = glm::normalize(m_LocalDir);
	//}

	//const glm::vec3& getLocalDir() const { return m_LocalDir; }

//private:
//	glm::vec3 m_LocalDir = glm::vec3(0.0f);
};

struct LightAttenuation {
	float Constant = 1.0f;
	float Linear = 0.0f;
	float Exp = 0.0f;
};

class PointLight : public BaseLight {
public:
	//glm::vec3 m_WorldPos = glm::vec3(0.0f);
	my::vec3 m_WorldPos = my::vec3(0.0f, 0, 0);

	LightAttenuation Attenuation;

	//void CalcLocalPosition() {}
	//const glm::vec3& GetLocalPosition() const { return m_LocalPosition; }

//private:
	//glm::vec3 m_LocalPosition = glm::vec3(0.0f);
};