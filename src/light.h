#pragma once
#include "core.h"

class BaseLight {
public:
	glm::vec3 Color = glm::vec3(1.0f, 1.0f, 1.0f);
	float AmbientIntesity = 0.0f;

	BaseLight() {}
};