#pragma once
#include "core.h"
#include "Texture.h"

const int SMAX_NUM_BONES_PER_VERTEX = 4;
const uint SINVALID_MATERIALS = 0xFFFFFFFF;
const uint SASSIMP_LOAD_FLAGS = (aiProcess_Triangulate
	//| aiProcess_GenSmoothNormals
	//| aiProcess_FlipUVs
	//| aiProcess_JoinIdenticalVertices
	//| aiProcess_OptimizeGraph
	);

class SkinnedMesh {
public:
	SkinnedMesh() {}
	~SkinnedMesh();

	bool LoadMesh(const std::string& fileName);
	void Render();

	uint NumBones() const { return (uint)m_BoneNameToIndexMap.size(); }

	void GetBoneTransforms(float TimeInSeconds, std::vector<glm::mat4>& Transforms, int AnimIndex);

private:
	void Clear();

	bool InitFromScene(const aiScene* pScene, const std::string& fileName);
	void CountVerticesAndIndices(const aiScene* pScene, uint& NumVertices, uint& NumIndices);
	void ReserveSpace(uint NumVertices, uint NimIndices);
	void InitAllMeshes(const aiScene* pScene);
	void InitSingleMesh(uint MeshIndex, const aiMesh* paiMesh);
	void PopulateBuffers();

	bool InitMaterials(const aiScene* pScene, const std::string& fileName);
	//void LoadTextures(const std::string& Dir, const aiMaterial* pMaterial, int index);
	void LoadDiffuseTexture(const std::string& Dir, const aiMaterial* pMaterial, int index);
	//void LoadSpecularTexture(const std::string& Dir, const aiMaterial* pMaterial, int index);
	//void LoadColors(const aiMaterial* pMaterial, int index);

	struct VertexBoneData {
		uint BoneIDs[SMAX_NUM_BONES_PER_VERTEX] = {};
		float Weights[SMAX_NUM_BONES_PER_VERTEX] = {};

		VertexBoneData() {}

		void AddBoneData(uint BoneID, float Weight) {
			if (Weight == .0f || Weight <= 0.01f) return; // if skiped need renormalize weights (it not implements yet)

			for (uint i = 0; i < array_num_elems(BoneIDs); i++) {
				//if (BoneIDs[i] == BoneID) return;
				if (Weights[i] == 0.0f) {
					BoneIDs[i] = BoneID;
					Weights[i] = Weight;
					return; 
				}
			}

			// should never get here - more bones than we have space for
			assert(0);
			std::cout << "WARNING: SkinnedMesh::AddBoneData(): need more bones than we have space for!!!\n";
		}
	};

	void LoadMeshBones(uint MeshIndex, const aiMesh* paiMesh);
	void LoadSingleBone(uint MeshIndex, const aiBone* pBone);
	int GetBoneId(const aiBone* pBone);
	void ReadNodeHierarchy(float AnimationTimeTicks, const aiNode* pNode, const glm::mat4& ParentTransform, int AnimIndex);
	const aiNodeAnim* FindNodeAnim(const aiAnimation* pAnimation, const std::string NodeName);
	uint FindScaling(float AnimationTimeTicks, const aiNodeAnim* pNodeAnim);
	uint FindRotation(float AnimationTimeTicks, const aiNodeAnim* pNodeAnim);
	uint FindPosition(float AnimationTimeTicks, const aiNodeAnim* pNodeAnim);

	void CalcInterpolatedScaling(aiVector3D& scaling, float AnimationTimeTicks, const aiNodeAnim* pNodeAnim);
	void CalcInterpolatedRotation(aiQuaternion& rotation, float AnimationTimeTicks, const aiNodeAnim* pNodeAnim);
	void CalcInterpolatedPosition(aiVector3D& translation, float AnimationTimeTicks, const aiNodeAnim* pNodeAnim);

	enum BUFFER_TYPE {
		INDEX_BUFFER = 0,
		POS_VB = 1,
		NORMAL_VB = 2,
		TEXCOORD_VB = 3,
		BONE_VB = 4,
		NUM_BUFFERS = 5
	};

	GLuint m_VAO = 0;
	GLuint m_Buffers[NUM_BUFFERS] = { 0 };

	struct BasicMeshEntry {
		uint NumIndices;
		uint BaseVertex;
		uint BaseIndex;
		uint MaterialIndex;
		BasicMeshEntry() { NumIndices = 0; BaseVertex = 0; BaseIndex = 0; MaterialIndex = SINVALID_MATERIALS; }
	};

	Assimp::Importer m_Importer;
	const aiScene* m_pScene = nullptr;

	std::vector<BasicMeshEntry> m_Meshes;
	std::vector<Texture*>		m_Materials;

	// temporary space for vertex stuff before we load them into the GPU
	std::vector<glm::vec3>			m_Positons;
	std::vector<glm::vec3>			m_Normals;
	std::vector<glm::vec2>			m_TexCoords;
	std::vector <uint>				m_Indices;
	std::vector <VertexBoneData>	m_Bones;

	std::map<std::string, uint> m_BoneNameToIndexMap;

	struct BoneInfo {
		glm::mat4 OffsetMatrix;
		glm::mat4 FinalTransformation = glm::mat4(0);
		BoneInfo(const aiMatrix4x4& Offset) {
			memcpy(&OffsetMatrix[0], &Offset, sizeof(float) * 16);
			OffsetMatrix = glm::transpose(OffsetMatrix);
		}
	};

	std::vector<BoneInfo> m_BoneInfo;
	glm::mat4 m_GlobalInverseTransform;
};