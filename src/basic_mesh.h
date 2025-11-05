#pragma once
#include "core.h"
#include "world_transform.h"
#include "Texture.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#define COLOR_TEXTURE_UNIT			GL_TEXTURE0
#define COLOR_TEXTURE_UNIT_INDEX	0
#define SHADOW_TEXTURE_UNIT			GL_TEXTURE1
#define SHADOW_TEXTURE_UNIT_INDEX	0
#define NORMAL_TEXTURE_UNIT			GL_TEXTURE2
#define NORMAL_TEXTURE_UNIT_INDEX	0

#define POSITION_LOCATION 0
#define NORMAL_LOCATION 1
#define TEXT_COORD_LOCATION 2

#define INVALID_MATERIALS 0xFFFFFFFF

#define ASSIMP_LOAD_FLAGS (aiProcess_Triangulate | aiProcess_GenSmoothNormals | /*aiProcess_FlipUVs |*/ aiProcess_JoinIdenticalVertices)

class BasicMesh {
	enum BUFFER_TYPE {
		INDEX_BUFFER = 0,
		POS_VB = 1,
		NORMAL_VB = 2,
		TEXCOORD_VB = 3,
		WVP_MAT_VB = 4,
		WORLD_MAT_VB = 5,
		NUM_BUFFERS = 6
	};

	//WorldTransform m_worldTransform;

	GLuint m_VAO = 0;
	GLuint m_Buffers[NUM_BUFFERS] = {};

	struct BasicMeshEntry {
		uint NumIndices;
		uint BaseVertex;
		uint BaseIndex;
		uint MaterialIndex;
		BasicMeshEntry() { NumIndices = 0; BaseVertex = 0; BaseIndex = 0; MaterialIndex = INVALID_MATERIALS; }
	};

	std::vector<BasicMeshEntry> m_Meshes;
	std::vector<Texture*> m_Textures;

	// temporary space for vertex stuff before we load them into the GPU
	std::vector<glm::vec3>	m_Positons;
	std::vector<glm::vec3>	m_Normals;
	std::vector<glm::vec2>	m_TexCoords;
	std::vector <uint>		m_Indices;

public:
	BasicMesh() {}
	~BasicMesh();

	bool LoadMesh(const std::string& fileName);

	void Render();
	//void Render(uint NumInstances, const glm::mat4* mWVP, const glm::mat4* mWorld);

	//WorldTransform& GetWorldTransform() { return m_worldTransform; }

private:
	void Clear();

	void PopulateBuffers();

	bool InitFromScene(const aiScene* pScene, const std::string& fileName);
	void CountVerticesAndIndices(const aiScene* pScene, uint& NumVertices, uint& NumIndices);
	void ReserveSpace(uint NumVertices, uint NimIndices);
	void InitAllMeshes(const aiScene* pScene);
	void InitSingleMesh(const aiMesh* paiMesh);
	bool InitMaterials(const aiScene* pScene, const std::string& fileName);
};