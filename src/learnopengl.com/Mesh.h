#pragma once
#include "core.h"

using namespace std;

struct MVertex {
	glm::vec3 Position;
	glm::vec3 Normal;
	glm::vec2 TexCoords;
	glm::vec3 Tangent;
	glm::vec3 Bitangent;
	int m_BoneIDs[MAX_BONE_INFLUENCE];//bone indexes which will influence this vertex	
	float m_Weights[MAX_BONE_INFLUENCE];//weights from each bone
};

struct MTexture {
	uint	id;
	string	type;
	string	path;
};

class MMesh {
public:
	// mesh data
	vector<MVertex>		m_vertices;
	vector<uint>		m_indices;
	vector<MTexture>	m_textures;

	MMesh(vector<MVertex> vertices, vector<uint> indices, vector<MTexture> textures);

	void Draw(const class Shader& shader);

private:
	// render data
	uint VAO, VBO, EBO;

	void setupMesh();
};