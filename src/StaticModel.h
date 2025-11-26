#pragma once
#include "myutil.h"
#include "Texture.h"
#include "material.h"

#pragma pack(1)
struct MeshHeader {
	// TODO: need to add version 
	uint mNumMeshes;
	uint mNumMaterials;
	uint numVertices;
	uint numIndices;
};

struct color { 
	float r, g, b;
	//color(float x, float y, float z) : r(x), g(y), b(z) {}
};

struct MaterialHeader {
	color AmbientColor;
	color DiffuseColor;
	color SpecularColor;
};
#pragma pack() // Reset to default packing

inline color toColor(const glm::vec3& c) {
	return color(c.x, c.y, c.z); // don't get it how it works (i haven't constructor)
}

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
	std::vector<Material>			m_Materials;

	std::string m_Directory;

	void clear();

	void calcVertices(const aiScene* pScene, uint& numVertices, uint& numIndices);
	void loadGeoData(const aiScene* pScene);
	void loadMaterials(const aiScene* pScene);
	void buildBuffers();
	void loadDiffuseTexture(const aiScene* pScene, const aiMaterial* pMaterial, int index);
	void loadSpecularTexture(const aiScene* pScene, const aiMaterial* pMaterial, int index);

public:
	//StaticModel() {}
	~StaticModel() { clear(); }

	void Load(std::string_view fileName, bool bFlipUVs = false);
	void Render();

	// write binary data to file
	void Write(std::string_view fileName);
	// read binary data from file
	void Read(std::string_view fileName);
};