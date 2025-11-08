#pragma once
#include "myutil.h"
#include "Texture.h"
#include "myanimator.h"

__declspec(align(16))
struct WeightedVertex {
	glm::vec3 pos;
	glm::vec3 normal;
	glm::vec2 texCoord;
	uint boneIDs[MAX_BONE_INFLUENCE]{};
	float weights[MAX_BONE_INFLUENCE]{};

	void addWeight(uint iBoneID, float fWeight) {
		if (fWeight == 0.0f) return;
		//if (fWeight == 0.0f || fWeight <= 0.01f) return; // if skiped need renormalize weights (it not implements yet)

		for (uint i = 0; i < MAX_BONE_INFLUENCE; i++) {
			//if (boneIDs[i] == iBoneID) return; // not work in my case
			if (weights[i] == 0.0f) {
				boneIDs[i] = iBoneID;
				weights[i] = fWeight;
				return;
			}
		}

		// should never get here - more bones than we have space for
		log("WARNING: MySkeletalModel3::addWeight: need more bones than we have space for!!!");
		assert(0);
	}

	bool operator==(const WeightedVertex& v) const {
		return pos.x == v.pos.x && pos.y == v.pos.y && pos.z == v.pos.z && 
			normal.x == v.normal.x && normal.y == v.normal.y && normal.z == v.normal.z &&
			texCoord.x == v.texCoord.x && texCoord.y == v.texCoord.y;
	}
};

struct MeshIndexedData {
	uint BaseVertex = 0;
	uint NumIndices = 0;
	uint BaseIndex = 0;
	uint MaterialIndex = 0;
};

struct Bone {
	std::string BoneName;
	glm::mat4 FinalTransform;
	//glm::mat4 Transform;
	glm::mat4 Offset = glm::mat4(1.0f);
};

//struct Key {
//	std::string Name;
//	std::vector<my::vec3> PosKeys;
//	std::vector<my::quat> RotKeys;
//	std::vector<my::vec3> ScalKeys;
//	double time;
//};

//struct MyAnimation3 {
//	std::string Name;
//	double Duration;
//	double TicksPerSecond;
//	std::vector<Key> m_Keys;
//};

//struct Node {
//	my::mat4 transformation;
//	std::string name;
//	int childrenCount;
//	std::vector<Node> children;
//};

class SkeletalModel {
	GLuint m_VAO = 0, m_VBO = 0, m_EBO = 0;

	std::vector<WeightedVertex>		m_Vertices;
	std::vector<uint>				m_Indices;
	std::vector<MeshIndexedData>	m_Meshes;
	std::map<std::string, uint>		m_BonesMap;
	std::vector<Texture*>			m_Textures;

	glm::mat4 m_GlobalInverseTransform = glm::mat4(1.0f);

	std::string m_Directory;

	std::vector<MyAnimation> m_Animations;

	//uint numNodes;
	
	struct LocalTranform {
		glm::vec3 pos;
		glm::quat rot;
		glm::vec3 scale;
	};

	void clear() {
		for (Texture* texture : m_Textures) safe_delete(texture);
		if (m_EBO != 0) { glDeleteBuffers(1, &m_EBO); m_EBO = 0; }
		if (m_VBO != 0) { glDeleteBuffers(1, &m_VBO); m_VBO = 0; }
		if (m_VAO != 0) { glDeleteVertexArrays(1, &m_VAO); m_VAO = 0; }
	}

public:
	~SkeletalModel() { clear(); }

	std::vector<Bone> m_Bones;
	//std::vector<Node> m_Nodes;
	Node m_RootNode;

	std::map<std::string, NodeInfo> m_requiredNodeMap;

public:
	void InitReqNodeMap(const aiNode* pNode) {
		std::string NodeName(pNode->mName.C_Str());
		NodeInfo info(pNode);

		m_requiredNodeMap[NodeName] = info;

		for (uint i = 0; i < pNode->mNumChildren; i++)
			InitReqNodeMap(pNode->mChildren[i]);
	}

	void MarkReqNodesForBone(const aiBone* pBone) {
		std::string NodeName(pBone->mName.data);

		const aiNode* pParent = nullptr;

		do {
			std::map<std::string, NodeInfo>::iterator it = m_requiredNodeMap.find(NodeName);
			if (it == m_requiredNodeMap.end()) {
				log_error("Cannot find bone in the hierarchy " << NodeName.c_str());
				assert(0);
			}

			it->second.isRequired = true;
			pParent = it->second.pNode->mParent;
			if (pParent)
				NodeName = std::string(pParent->mName.data);

		} while (pParent);
	}

	void Load(std::string_view fileName, bool bFlipUVs = false);
	void calcVertices(const aiScene* pScene, uint& numVertices, uint& numIndices);
	void loadGeoData(const aiScene* pScene);
	void NormalizeWeights();
	void loadMaterials(const aiScene* pScene);
	void loadDiffuseTexture(const aiScene* pScene, const aiMaterial* pMaterial, int index);
	void buildBuffers();
	void loadAnimData(const aiScene* scene);
	void ReadNodeHierarchy(Node& node, const aiNode* pNode, const glm::mat4& mParentTransform);
	void UpdateAnimBlended(float TimeInSec, uint animA, uint animB, float BlendFactor);
	void UpdateNodeHierarchyBlended(float animTimeA, float animTimeB, const Node& pNode, const glm::mat4& parentTrans, const MyAnimation& pAnimA, const MyAnimation& pAnimB, float blendFactor);

	float CalcAnimTimeTicks(float TimeInSec, unsigned int AnimIndex);
	float GetScaleFactor(double firstPos, double nextPos, float animTime);

	void UpdateAnim(float TimeInSec, uint AnimIndex);
	void UpdateAnimHierarchy(float AnimTimeTicks, const Node* pNode, const glm::mat4& mParentTransform, int AnimIndex);

	void CalcInterpolatePosition(float AnimTimeTicks, const Key* pAnimNode, glm::vec3& pos);
	void CalcInterpolateRotation(float AnimTimeTicks, const Key* pAnimNode, glm::quat& rot);
	void CalcInterpolateScaling(float AnimTimeTicks, const Key* pAnimNode, glm::vec3& scale);

	const Key* FindNodeAnim(const MyAnimation& pAnim, std::string_view NodeName);

	uint FindPosIndex(float AnimTimeTicks, const Key* pAnimNode);
	uint FindRotIndex(float AnimTimeTicks, const Key* pAnimNode);
	uint FindScaleIndex(float AnimTimeTicks, const Key* pAnimNode);

	void Render();
};
