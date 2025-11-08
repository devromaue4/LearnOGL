// from tutor learnopengl.com
#pragma once
#include "core.h"
#include "Mesh.h"

struct BoneInfo {
	/*id is index in finalBoneMatrices*/
	int id;
	/*offset matrix transforms vertex from model space to bone space*/
	glm::mat4 offset;
};

class Model {
public:
	vector<MTexture> textures_loaded; // stores all the textures loaded so far, optimization to make sure textures aren't loaded more than once.
	vector<MMesh>	meshes;
	string			directory;
	bool			gammaCorrection;

	std::map<string, BoneInfo> m_BoneInfoMap;
	int m_BoneCounter = 0;

	auto& GetBoneInfoMap() { return m_BoneInfoMap; }
	int& GetBoneCount() { return m_BoneCounter; }

	void SetVertexBoneDataToDefault(MVertex& vertex);
	void SetVertexBoneData(MVertex& vertex, int boneID, float weight);
	void ExtractBoneWeightForVertices(std::vector<MVertex>& vertices, aiMesh* mesh/*, const aiScene* scene*/);

	Model(string const& path, bool gamma = false) : gammaCorrection(gamma) { loadModel(path); }
	void Draw(const class Shader& shader);

private:
	void loadModel(string path, bool flipUVs = false);
	void processNode(aiNode* node, const aiScene* scene);
	MMesh processMesh(aiMesh* mesh, const aiScene* scene);
	vector<MTexture> loadMaterialTextures(aiMaterial* material, aiTextureType type, string typeName);

	void cleanup();
};
