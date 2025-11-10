#pragma once
#include "myutil.h"
#include "Texture.h"

struct SVertex {
	glm::vec3 pos = glm::vec3(0);
	glm::vec3 normal = glm::vec3(0);
	glm::vec2 texCoord = glm::vec2(0);
};

struct MeshIndexedData {
	uint BaseVertex = 0;
	uint NumIndices = 0;
	uint BaseIndex = 0;
	uint MaterialIndex = 0;
};

class StaticModel {
	GLuint m_VAO = 0, m_VBO = 0, m_EBO = 0;

	std::vector<SVertex>			m_Vertices;
	std::vector<uint>				m_Indices;
	std::vector<MeshIndexedData>	m_Meshes;
	//std::vector<Texture*>			m_Textures;
	std::vector<std::shared_ptr<Texture>> m_Textures;

	std::string m_Directory;

	void clear() {
		//for (Texture* texture : m_Textures) safe_delete(texture);
		if (m_EBO != 0) { glDeleteBuffers(1, &m_EBO); m_EBO = 0; }
		if (m_VBO != 0) { glDeleteBuffers(1, &m_VBO); m_VBO = 0; }
		if (m_VAO != 0) { glDeleteVertexArrays(1, &m_VAO); m_VAO = 0; }
	}

public:
	~StaticModel() { clear(); }

	void Load(std::string_view fileName, bool bFlipUVs = false);
	void calcVertices(const aiScene* pScene, uint& numVertices, uint& numIndices);
	void loadGeoData(const aiScene* pScene);
	void loadMaterials(const aiScene* pScene);
	void loadDiffuseTexture(const aiScene* pScene, const aiMaterial* pMaterial, int index);
	void buildBuffers();
	void Render();
};