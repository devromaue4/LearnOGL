#pragma once
#include "core.h"

struct MVertex {
	glm::vec3 Position;
	glm::vec3 Normal;
	glm::vec2 TexCoords;
	glm::vec3 Tangent;
	glm::vec3 Bitangent;
	int m_BoneIDs[MAX_BONE_INF];//bone indexes which will influence this vertex	
	float m_Weights[MAX_BONE_INF];//weights from each bone
};

struct MTexture {
	uint	id;
	std::string	type;
	std::string	path;
};

class MMesh {
public:
	// mesh data
	std::vector<MVertex>		m_vertices;
	std::vector<uint>		m_indices;
	std::vector<MTexture>	m_textures;

	MMesh(std::vector<MVertex> vertices, std::vector<uint> indices, std::vector<MTexture> textures);

	void Draw(const class Shader& shader);

private:
	// render data
	uint VAO, VBO, EBO;

	void setupMesh();
};