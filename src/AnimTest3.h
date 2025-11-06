#pragma once
#include "myutil.h"

//__declspec(align(16))
struct MyVertex3 {
	my::vec3 pos;
	my::vec3 normal;
	my::vec2 texCoord;
	uint boneIDs[MAX_BONE_INFLUENCE]{};
	float weights[MAX_BONE_INFLUENCE]{};

	void addWeight(uint iBoneID, float fWeight) {
		if (fWeight == 0.0f || fWeight <= 0.01f) return; // if skiped need renormalize weights (it not implements yet)

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
};

struct MyMeshContainer3 {
	uint BaseVertex = 0;
	uint NumIndices = 0;
	uint BaseIndex = 0;
	uint MaterialIndex = 0;
};

struct MyBone3 {
	std::string BoneName;
	my::mat4 FinalTransform;
	my::mat4 Transform;
	my::mat4 Offset = my::mat4(1);
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

class MySkeletalModel3 {
	GLuint m_VAO = 0, m_VBO = 0, m_EBO = 0;

	std::vector<MyVertex3>			m_Vertices;
	std::vector<uint>				m_Indices;
	std::vector<MyMeshContainer3>	m_Meshes;
	std::map<std::string, uint>		m_BonesMap;
	std::vector<Texture*>			m_Textures;

	my::mat4 m_GlobalInverseTransform = my::mat4(1);

	std::string m_Directory;

	// temporary
	Assimp::Importer importer;
	const aiScene* m_pScene;

	struct LocalTranform {
		my::vec3 pos;
		my::quat rot;
		my::vec3 scale;
	};

	void clear() {
		for (Texture* texture : m_Textures) safe_delete(texture);
		if (m_EBO != 0) { glDeleteBuffers(1, &m_EBO); m_EBO = 0; }
		if (m_VBO != 0) { glDeleteBuffers(1, &m_VBO); m_VBO = 0; }
		if (m_VAO != 0) { glDeleteVertexArrays(1, &m_VAO); m_VAO = 0; }
	}

public:
	std::vector<MyBone3>			m_Bones;

	void Load(std::string_view fileName, bool bFlipUVs = false) {
		clear();

		uint flags =
			aiProcess_LimitBoneWeights |
			aiProcess_Triangulate
			| aiProcess_GenSmoothNormals 
			//| aiProcess_CalcTangentSpace |
			| aiProcess_JoinIdenticalVertices
			//| aiProcess_MakeLeftHanded
			//| aiProcess_OptimizeGraph
			//| aiProcess_FindDegenerates
			;
		if (bFlipUVs) flags |= aiProcess_FlipUVs;

		//Assimp::Importer importer;
		//const aiScene* 
		m_pScene = importer.ReadFile(fileName.data(), flags);
		if (!m_pScene || m_pScene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !m_pScene->mRootNode) {
			log(importer.GetErrorString());
			throw std::exception("MySkeletalModel3::Load!");
		}

		m_Directory = fileName.substr(0, fileName.find_last_of('/'));

		m_GlobalInverseTransform = my::inverseAffineSSE(toMy(m_pScene->mRootNode->mTransformation));

		m_Meshes.resize(m_pScene->mNumMeshes);
		m_Textures.resize(m_pScene->mNumMaterials);

		uint numVertices = 0, numIndices = 0;
		calcVertices(m_pScene, numVertices, numIndices);

		m_Vertices.reserve(numVertices);
		m_Indices.reserve(numIndices);

		loadGeoData(m_pScene);

		ReadNodeHierarchy(m_pScene->mRootNode, my::mat4(1));

		loadMaterials(m_pScene);

		buildBuffers();

		//importer.FreeScene();
	}

	void calcVertices(const aiScene* pScene, uint& numVertices, uint& numIndices) {
		for (uint iMesh = 0; iMesh < pScene->mNumMeshes; iMesh++) {
			m_Meshes[iMesh].MaterialIndex = pScene->mMeshes[iMesh]->mMaterialIndex;
			m_Meshes[iMesh].BaseVertex = numVertices;
			m_Meshes[iMesh].BaseIndex = numIndices;

			numVertices += pScene->mMeshes[iMesh]->mNumVertices;
			m_Meshes[iMesh].NumIndices = pScene->mMeshes[iMesh]->mNumFaces * 3;
			numIndices += m_Meshes[iMesh].NumIndices;
		}
	}

	void loadGeoData(const aiScene* pScene) {
		MyVertex3 myVert;
		MyBone3 myBone;
		uint BoneIndex = 0;

		for (uint iMesh = 0; iMesh < pScene->mNumMeshes; iMesh++) {
			const aiMesh* pMesh = pScene->mMeshes[iMesh];
			for (uint iVert = 0; iVert < pMesh->mNumVertices; iVert++) {
				myVert.pos = toMy(pMesh->mVertices[iVert]);

				if (pMesh->HasNormals())
					myVert.normal = toMy(pMesh->mNormals[iVert]);
				else myVert.normal = my::vec3(.0f, 1.f, .0f);

				if (pMesh->HasTextureCoords(0)) {
					const aiVector3D& tc = pMesh->mTextureCoords[0][iVert];
					myVert.texCoord = my::vec2(tc.x, tc.y);
				}

				m_Vertices.push_back(myVert);
			}

			for (uint iface = 0; iface < pMesh->mNumFaces; iface++) {
				m_Indices.push_back(pMesh->mFaces[iface].mIndices[0]);
				m_Indices.push_back(pMesh->mFaces[iface].mIndices[1]);
				m_Indices.push_back(pMesh->mFaces[iface].mIndices[2]);
			}

			for (uint ibone = 0; ibone < pMesh->mNumBones; ibone++) {
				const aiBone* pBone = pMesh->mBones[ibone];
				
				const char* BoneName = pBone->mName.C_Str();
				if (m_BonesMap.find(BoneName) == m_BonesMap.end()) {
					BoneIndex = (uint)m_BonesMap.size();
					m_BonesMap[BoneName] = BoneIndex;

					myBone.BoneName = BoneName;
					//log(BoneIndex << " " << myBone.BoneName);
					myBone.Offset = toMy(pBone->mOffsetMatrix);
					m_Bones.push_back(myBone); // realloc everytime not efficient
				}
				else BoneIndex = m_BonesMap[BoneName];

				for (uint iWeight = 0; iWeight < pBone->mNumWeights; iWeight++) {
					
					uint vertID = m_Meshes[iMesh].BaseVertex + pBone->mWeights[iWeight].mVertexId;
					float fWeight = pBone->mWeights[iWeight].mWeight;

					m_Vertices[vertID].addWeight(BoneIndex, fWeight);
				}
			}
		}
	}

	void loadMaterials(const aiScene* pScene) {
		for (uint i = 0; i < pScene->mNumMaterials; i++)
			loadDiffuseTexture(pScene, pScene->mMaterials[i], i);
	}

	void loadDiffuseTexture(const aiScene* pScene, const aiMaterial* pMaterial, int index) {
		m_Textures[index] = nullptr;

		if (pMaterial->GetTextureCount(aiTextureType_DIFFUSE) > 0) {
			aiString path;
			if (pMaterial->GetTexture(aiTextureType_DIFFUSE, 0, &path) == AI_SUCCESS) {
				const aiTexture* pAiTexture = pScene->GetEmbeddedTexture(path.data);
				if (pAiTexture) {
					m_Textures[index] = new Texture;
					if (!m_Textures[index]->Load(pAiTexture->mWidth, pAiTexture->pcData))
						log("Error loading Embedded texture");
				}
				else {
					std::string p(path.data);
					//if (p.substr(0, 2) == ".\\") // don't know this code
					//	p = p.substr(2, p.size() - 2);

					//int sz = p.find_last_of('/') + 1;
					//if(sz > 0) p = p.substr(sz, sz);

					std::string fullPath = m_Directory + "/" + p;

					m_Textures[index] = new Texture(fullPath.c_str());
					if (!m_Textures[index]->Load()) {
						log_error("loading texture " << fullPath);
						safe_delete(m_Textures[index]);
						m_Textures[index] = nullptr;
						return;
					}
				}
			}
		}
	}

	void buildBuffers() {
		glGenVertexArrays(1, &m_VAO);
		glGenBuffers(1, &m_VBO);
		glGenBuffers(1, &m_EBO);

		glBindVertexArray(m_VAO);
		glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(MyVertex3) * m_Vertices.size(), &m_Vertices[0], GL_STATIC_DRAW);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint) * m_Indices.size(), &m_Indices[0], GL_STATIC_DRAW);

		GLsizei stride = sizeof(MyVertex3);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (const void*)0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, stride, (const void*)offsetof(MyVertex3, normal));
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, stride, (const void*)offsetof(MyVertex3, texCoord));
		glEnableVertexAttribArray(3);
		glVertexAttribIPointer(3, 4, GL_INT, stride, (const void*)offsetof(MyVertex3, boneIDs));
		glEnableVertexAttribArray(4);
		glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, stride, (const void*)offsetof(MyVertex3, weights));

		glBindVertexArray(0);
	}

	void ReadNodeHierarchy(const aiNode* pNode, const my::mat4& mParentTransform) {
		my::mat4 GlobalTransform = mParentTransform * toMy(pNode->mTransformation);

		const char* NodeName = pNode->mName.C_Str();

		if (m_BonesMap.find(NodeName) != m_BonesMap.end()) {
			uint BoneIndex = m_BonesMap[NodeName];
			m_Bones[BoneIndex].FinalTransform = m_GlobalInverseTransform * GlobalTransform * m_Bones[BoneIndex].Offset;
		}

		for (uint i = 0; i < pNode->mNumChildren; i++)
			ReadNodeHierarchy(pNode->mChildren[i], GlobalTransform);
	}

	void UpdateAnimBlended(float TimeInSec, uint animA, uint animB, float BlendFactor) {
		if (animA >= m_pScene->mNumAnimations || animB >= m_pScene->mNumAnimations) {
			log("No Animation"); assert(0);
		}
		if (BlendFactor < 0.0f || BlendFactor > 1.0f) {
			log_error("invalid blend factor"); assert(0);
		}

		float AnimTimeTicksA = CalcAnimTimeTicks(TimeInSec, animA);
		float AnimTimeTicksB = CalcAnimTimeTicks(TimeInSec, animB);

		const aiAnimation* pAnimA= m_pScene->mAnimations[animA];
		const aiAnimation* pAnimB= m_pScene->mAnimations[animB];

		UpdateNodeHierarchyBlended(AnimTimeTicksA, AnimTimeTicksB, m_pScene->mRootNode, my::mat4(1), pAnimA, pAnimB, BlendFactor);
	}

	void UpdateNodeHierarchyBlended(float animTimeA, float animTimeB, const aiNode* pNode, const my::mat4& parentTrans, const aiAnimation* pAnimA, const aiAnimation* pAnimB, float blendFactor) {
		std::string NodeName(pNode->mName.data);

		my::mat4 NodeTransform = toMy(pNode->mTransformation);

		const aiNodeAnim* pStartNodeAnim = FindNodeAnim(pAnimA, NodeName);

		// calc local transform pStartNodeAnim
		LocalTranform transA;
		if (pStartNodeAnim) {
			CalcInterpolatePosition(animTimeA, pStartNodeAnim, transA.pos);
			CalcInterpolateRotation(animTimeA, pStartNodeAnim, transA.rot);
			CalcInterpolateScaling(animTimeA, pStartNodeAnim, transA.scale);
		}

		const aiNodeAnim* pEndNodeAnim = FindNodeAnim(pAnimB, NodeName);

		if ((pStartNodeAnim && !pEndNodeAnim) || (!pStartNodeAnim && pEndNodeAnim)) {
			log_error("There is only one animation node.\n This case is not supported.");
			exit(0);
		}

		// calc local transform pEndNodeAnim
		LocalTranform transB;
		if (pEndNodeAnim) {
			CalcInterpolatePosition(animTimeB, pEndNodeAnim, transB.pos);
			CalcInterpolateRotation(animTimeB, pEndNodeAnim, transB.rot);
			CalcInterpolateScaling(animTimeB, pEndNodeAnim, transB.scale);
		}

		if (pStartNodeAnim && pEndNodeAnim) {
			my::vec3 blendedTrans = (1.0f - blendFactor) * transA.pos + transB.pos * blendFactor;
			//my::vec3 blendedTrans = my::lerp(transA.pos, transB.pos, blendFactor);
			my::mat4 mTrans(1);
			mTrans = my::translate(mTrans, blendedTrans);

			//my::quat rot = my::lerp(transA.rot, transB.rot, blendFactor);
			my::quat rot = my::slerp(transA.rot, transB.rot, blendFactor);
			//aiQuaternion::Interpolate
			my::mat4 mRot = my::toMat4(rot);

			my::vec3 blendedScale = (1.0f - blendFactor) * transA.scale + transB.scale * blendFactor;
			//my::vec3 blendedScale = my::lerp(transA.pos, transB.pos, blendFactor);
			my::mat4 mScale(1);
			mScale = my::scale(mScale, blendedScale);

			NodeTransform = mTrans * mRot * mScale;
		}

		my::mat4 GlobalTransform = parentTrans * NodeTransform;

		if (m_BonesMap.find(NodeName) != m_BonesMap.end()) {
			uint BoneIndex = m_BonesMap[NodeName];
			m_Bones[BoneIndex].FinalTransform = m_GlobalInverseTransform * GlobalTransform * m_Bones[BoneIndex].Offset;
		}

		for (uint i = 0; i < pNode->mNumChildren; i++)
			UpdateNodeHierarchyBlended(animTimeA, animTimeB, pNode->mChildren[i], GlobalTransform, pAnimA, pAnimB, blendFactor);
	}

	float CalcAnimTimeTicks(float TimeInSec, unsigned int AnimIndex)
	{
		float TicksPerSecond = (float)(m_pScene->mAnimations[AnimIndex]->mTicksPerSecond != 0 ? m_pScene->mAnimations[AnimIndex]->mTicksPerSecond : 25.0f);
		float TimeInTicks = TimeInSec * TicksPerSecond;
		// we need to use the integral part of mDuration for the total length of the animation
		float Duration = 0.0f;
		float fraction = modf((float)m_pScene->mAnimations[AnimIndex]->mDuration, &Duration);
		float AnimationTimeTicks = fmod(TimeInTicks, Duration);
		return AnimationTimeTicks;
	}

	void UpdateAnim(float TimeInSec, uint AnimIndex) {

		if (m_pScene->mNumAnimations == 0 || AnimIndex > m_pScene->mNumAnimations - 1) {
			log("No Animation");
			return;
		}
		
		// calc anim time
		float TimePerSeconds = (float)(m_pScene->mAnimations[AnimIndex]->mTicksPerSecond != 0 ? m_pScene->mAnimations[AnimIndex]->mTicksPerSecond : 25.0f);
		float TimeInTicks = TimeInSec * TimePerSeconds;
		float AnimTimeTicks = (float)fmod(TimeInTicks, m_pScene->mAnimations[AnimIndex]->mDuration);
		//float AnimTimeTicks = (float)fmod(TimeInSec * 10.0f, m_pScene->mAnimations[0]->mDuration); // for loop
		//float AnimTimeTicks = TimeInSec * 10.0f;

		UpdateAnimHierarchy(AnimTimeTicks, m_pScene->mRootNode, my::mat4(1), AnimIndex);
	}

	void UpdateAnimHierarchy(float AnimTimeTicks, const aiNode* pNode, const my::mat4& mParentTransform, int AnimIndex) {
		
		my::mat4 mNodeTransform = toMy(pNode->mTransformation);

		std::string NodeName = pNode->mName.C_Str();

		const aiNodeAnim* pAnimNode = nullptr;
		const aiAnimation* pAnim = m_pScene->mAnimations[AnimIndex]; // 0 get first animation
		if (pAnim) {
			for (uint ikey = 0; ikey < pAnim->mNumChannels; ikey++) {
				pAnimNode = pAnim->mChannels[ikey];
				if (pAnimNode->mNodeName.data == NodeName)
					break;
				pAnimNode = nullptr;
			}
		}

		// find keys and interpolate between them
		if (pAnimNode) {
			my::vec3 pos;
			CalcInterpolatePosition(AnimTimeTicks, pAnimNode, pos);
			my::mat4 mTrans(1);
			mTrans = my::translate(mTrans, pos);

			my::quat rot;
			CalcInterpolateRotation(AnimTimeTicks, pAnimNode, rot);
			my::mat4 mRot = my::toMat4(rot);

			my::vec3 scale;
			CalcInterpolateScaling(AnimTimeTicks, pAnimNode, scale);
			my::mat4 mScale(1);
			mScale = my::scale(mScale, scale);

			mNodeTransform = mTrans * mRot * mScale;
		}

		my::mat4 GlobalTransform = mParentTransform * mNodeTransform;
		
		if (m_BonesMap.find(NodeName) != m_BonesMap.end()) {
			uint BoneIndex = m_BonesMap[NodeName];
			m_Bones[BoneIndex].FinalTransform = m_GlobalInverseTransform * GlobalTransform * m_Bones[BoneIndex].Offset;
		}

		for (uint i = 0; i < pNode->mNumChildren; i++)
			UpdateAnimHierarchy(AnimTimeTicks, pNode->mChildren[i], GlobalTransform, AnimIndex);
	}

	void CalcInterpolatePosition(float AnimTimeTicks, const aiNodeAnim* pAnimNode, my::vec3& pos) {
		if(pAnimNode->mNumPositionKeys == 1) {
			pos = toMy(pAnimNode->mPositionKeys[0].mValue);
			return;
		}

		uint index = FindPosIndex(AnimTimeTicks, pAnimNode);
		uint nextIndex = index + 1;
		assert(nextIndex < pAnimNode->mNumPositionKeys);
		float factor = GetScaleFactor(pAnimNode->mPositionKeys[index].mTime, pAnimNode->mPositionKeys[nextIndex].mTime, AnimTimeTicks);
		pos = my::lerp_imp(toMy(pAnimNode->mPositionKeys[index].mValue), toMy(pAnimNode->mPositionKeys[nextIndex].mValue), factor);
	}

	void CalcInterpolateRotation(float AnimTimeTicks, const aiNodeAnim* pAnimNode, my::quat& rot) {
		if (pAnimNode->mNumRotationKeys == 1) {
			rot = toMy(pAnimNode->mRotationKeys[0].mValue);
			return;
		}

		uint index = FindRotIndex(AnimTimeTicks, pAnimNode);
		uint nextIndex = index + 1;
		assert(nextIndex < pAnimNode->mNumRotationKeys);
		float factor = GetScaleFactor(pAnimNode->mRotationKeys[index].mTime, pAnimNode->mRotationKeys[nextIndex].mTime, AnimTimeTicks);
		rot = my::slerp(toMy(pAnimNode->mRotationKeys[index].mValue), toMy(pAnimNode->mRotationKeys[nextIndex].mValue), factor);
		rot = my::normalize(rot);
	}

	void CalcInterpolateScaling(float AnimTimeTicks, const aiNodeAnim* pAnimNode, my::vec3& scale) {
		if (pAnimNode->mNumScalingKeys == 1) {
			scale = toMy(pAnimNode->mScalingKeys[0].mValue);
			return;
		}

		uint index = FindScaleIndex(AnimTimeTicks, pAnimNode);
		uint nextIndex = index + 1;
		assert(nextIndex < pAnimNode->mNumScalingKeys);
		float factor = GetScaleFactor(pAnimNode->mScalingKeys[index].mTime, pAnimNode->mScalingKeys[nextIndex].mTime, AnimTimeTicks);
		scale = my::lerp_imp(toMy(pAnimNode->mScalingKeys[index].mValue), toMy(pAnimNode->mScalingKeys[nextIndex].mValue), factor);
	}

	float GetScaleFactor(double firstPos, double nextPos, float animTime) {
		double deltaTime = nextPos - firstPos;
		double factor = (animTime - firstPos) / deltaTime;
		assert(factor >= 0.0f && factor <= 1.0f);
		return (float)factor;
	}

	const aiNodeAnim* FindNodeAnim(const aiAnimation* pAnim, std::string_view NodeName) {
		const aiNodeAnim* pAnimNode = nullptr;
		//const aiAnimation* pAnim = m_pScene->mAnimations[AnimIndex]; // 0 get first animation
		if (pAnim) {
			for (uint ikey = 0; ikey < pAnim->mNumChannels; ikey++) {
				pAnimNode = pAnim->mChannels[ikey];
				if (pAnimNode->mNodeName.data == NodeName)
					return pAnimNode;// break;
				//pAnimNode = nullptr;
			}
		}

		return nullptr;
	}

	uint FindPosIndex(float AnimTimeTicks, const aiNodeAnim* pAnimNode) {
		for (uint i = 0; i < pAnimNode->mNumPositionKeys - 1; i++) {
			float t = (float)pAnimNode->mPositionKeys[i + 1].mTime;
			if (t > AnimTimeTicks) return i;
		}
		return 0;
	}

	uint FindRotIndex(float AnimTimeTicks, const aiNodeAnim* pAnimNode) {
		for (uint i = 0; i < pAnimNode->mNumRotationKeys - 1; i++) {
			float t = (float)pAnimNode->mRotationKeys[i + 1].mTime;
			if (t > AnimTimeTicks) return i;
		}
		return 0;
	}

	uint FindScaleIndex(float AnimTimeTicks, const aiNodeAnim* pAnimNode) {
		for (uint i = 0; i < pAnimNode->mNumScalingKeys - 1; i++) {
			float t = (float)pAnimNode->mScalingKeys[i + 1].mTime;
			if (t > AnimTimeTicks) return i;
		}
		return 0;
	}

	void Render() {
		glBindVertexArray(m_VAO);

		for (uint i = 0; i < m_Meshes.size(); i++) {
			uint MatIndex = m_Meshes[i].MaterialIndex;
			assert(MatIndex < m_Textures.size());
			if (m_Textures.size())
				if (m_Textures[MatIndex]) m_Textures[MatIndex]->Bind();

			glDrawElementsBaseVertex(GL_TRIANGLES, m_Meshes[i].NumIndices, GL_UNSIGNED_INT, (void*)(sizeof(uint) * m_Meshes[i].BaseIndex), m_Meshes[i].BaseVertex);
		}

		glBindVertexArray(0);
	}
};
