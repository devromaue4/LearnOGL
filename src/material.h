#pragma once
#include "core.h"

class Material {
public:
	glm::vec3 AmbientColor = glm::vec3(0.0f);
	glm::vec3 DiffuseColor = glm::vec3(0.0f);
	glm::vec3 SpecularColor = glm::vec3(0.0f);

	std::shared_ptr<Texture> texDiffuse, texSpecular;
};
