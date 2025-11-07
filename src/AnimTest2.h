#pragma once
#include "Texture.h"
#include "myanimator.h"
#include "myutil.h"

struct MyVertex2 {
	glm::vec3 pos = glm::vec3(0);
	glm::vec3 normal = glm::vec3(0);
	glm::vec2 texCoord = glm::vec2(0);
	uint boneIDs[MAX_BONE_INFLUENCE]{};
	float weights[MAX_BONE_INFLUENCE]{};
	MyVertex2() {}

	void AddBoneData(uint boneId, float weight) {
		for (uint i = 0; i < MAX_BONE_INFLUENCE; i++) {
			if (weights[i] == 0.0f) {
				boneIDs[i] = boneId;
				weights[i] = weight;
				return;
			}
		}
		// should never get here - more bones than we have space for
		log("WARNING: MySkeletalModel2::AddBoneData: need more bones than we have space for!!!");
		//assert(0);
	}
};

struct MeshContainer {
	uint BaseVertex = 0;
	uint BaseIndex = 0;
	uint NumIndices = 0;
	uint MaterialIndex = 0;
};

struct MyBone2 {
	std::string Name;
	glm::mat4 Transformation = glm::mat4(1.0f);
	glm::mat4 FinalTransform = glm::mat4(0);
	glm::mat4 Offset = glm::mat4(1.0f);
};

class MySkeletalModel2 {
	GLuint m_VAO = 0, m_VBO = 0, m_EBO = 0;

	std::string m_Directory;

public:
	std::vector<MyVertex2>		m_Vertices;
	std::vector<GLuint>			m_Indices;
	std::vector<MeshContainer>	m_Meshes;
	std::vector<Texture*>		m_Textures;
	std::vector<MyBone2>		m_Bones;

	std::map<std::string, uint> m_BonesMap;

	glm::mat4 m_GlobalInverseTransform;

	//Assimp::Importer Importer;
	//const aiScene* pScene;

	// animation data
	std::vector<MyAnimation> m_Animations;

	AssimpNodeData m_RootNode;

	~MySkeletalModel2() { Clear(); }

	void Clear() {
		for (Texture* texture : m_Textures) safe_delete(texture);
		if (m_EBO != 0) { glDeleteBuffers(1, &m_EBO); m_EBO = 0; }
		if (m_VBO != 0) { glDeleteBuffers(1, &m_VBO); m_VBO = 0; }
		if (m_VAO != 0) { glDeleteVertexArrays(1, &m_VAO); m_VAO = 0; }
	}

	void Load(std::string path, bool flipUVs = false) {
		Clear();

		// for static meshes
		//uint flags = aiProcess_Triangulate | 
		//	aiProcess_JoinIdenticalVertices |
		//	aiProcess_ImproveCacheLocality |
		//	aiProcess_RemoveRedundantMaterials;
		// for skeletal meshes
		uint flags = 
			aiProcess_LimitBoneWeights |
			aiProcess_Triangulate
			| aiProcess_GenSmoothNormals
			| aiProcess_JoinIdenticalVertices
			| aiProcess_CalcTangentSpace;
		if(flipUVs) flags |= aiProcess_FlipUVs; // set flag

		Assimp::Importer Importer;
		Importer.SetPropertyBool(AI_CONFIG_PP_FD_REMOVE, true);
		const aiScene* pScene;
		pScene = Importer.ReadFile(path, flags);
		if (!pScene || pScene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !pScene->mRootNode) {
			std::cout << Importer.GetErrorString() << std::endl;
			throw std::exception("MySkeletalModel2::Load!"); 
		}

		m_Directory = path.substr(0, path.find_last_of('/'));

		m_GlobalInverseTransform = glm::inverse(toGlm(pScene->mRootNode->mTransformation));

		uint numMeshes = pScene->mNumMeshes;
		m_Meshes.resize(numMeshes);
		//m_Meshes.reserve(numMeshes);

		uint numVertices = 0, numIndices = 0;
		for (uint iMesh = 0; iMesh < numMeshes; iMesh++) {
			
			const aiMesh* pMesh = pScene->mMeshes[iMesh];

			m_Meshes[iMesh].BaseVertex = numVertices;
			m_Meshes[iMesh].MaterialIndex = pScene->mMeshes[iMesh]->mMaterialIndex;

			numVertices += pMesh->mNumVertices;

			//m_Vertices.reserve(numVertices);
			for (uint iVert = 0; iVert < pMesh->mNumVertices; iVert++) {

				const aiVector3D& vert = pMesh->mVertices[iVert];
				MyVertex2 myVert;
				myVert.pos = glm::vec3(vert.x, vert.y, vert.z);

				//if (pMesh->HasTextureCoords(0)) {
				//	aiVector3D tx = pScene->mMeshes[iMesh]->mTextureCoords[0][iVert];
				//	myVert.texCoord = glm::vec2(tx.x, tx.y);
				//}

				m_Vertices.push_back(myVert);
			}

			//const aiMaterial* aiMat = pScene->mMaterials[pScene->mMeshes[iMesh]->mMaterialIndex];
			//uint numTextures = aiMat->GetTextureCount(aiTextureType_DIFFUSE);
			//for (uint iTex = 0; iTex < numTextures; iTex++) {
			//	aiString str;
			//	aiMat->GetTexture(aiTextureType_DIFFUSE, iTex, &str);
			//	std::string filePath = m_Directory + "/" + str.C_Str();
			//	Texture* texture = new Texture(filePath.c_str());
			//	texture->Load();
			//	m_Textures.push_back(texture);
			//}

			uint numFaces = pMesh->mNumFaces;
			m_Meshes[iMesh].NumIndices = numFaces * 3;
			m_Meshes[iMesh].BaseIndex = numIndices;
			numIndices += m_Meshes[iMesh].NumIndices;

			//m_Indices.reserve(numFaces * 3);
			for (uint iFace = 0; iFace < numFaces; iFace++) {
				const aiFace& face = pMesh->mFaces[iFace];
				assert(face.mNumIndices == 3);
				m_Indices.push_back(face.mIndices[0]);
				m_Indices.push_back(face.mIndices[1]);
				m_Indices.push_back(face.mIndices[2]);
			}

			uint NumBones = pMesh->mNumBones;
			
			//m_Bones.reserve(NumBones);

			for (uint iBone = 0; iBone < NumBones; iBone++) {
				aiBone* pBone = pMesh->mBones[iBone];

				std::string NameBone = pBone->mName.C_Str();

				uint BoneIndex = 0;

				if (m_BonesMap.find(NameBone) == m_BonesMap.end()) {

					BoneIndex = (int)m_BonesMap.size();
					m_BonesMap[NameBone] = BoneIndex;

					MyBone2 myBone;
					myBone.Name = pBone->mName.C_Str();
					myBone.Offset = toGlm(pMesh->mBones[iBone]->mOffsetMatrix);

					m_Bones.push_back(myBone);
				}
				else BoneIndex = m_BonesMap[NameBone];			

				for (uint iWeight = 0; iWeight < pBone->mNumWeights; iWeight++) {
					const aiVertexWeight& pWeight = pBone->mWeights[iWeight];

					uint vertID = m_Meshes[iMesh].BaseVertex + pWeight.mVertexId;
					//uint vertID = pWeight.mVertexId;
					float fWeight = pWeight.mWeight;

					m_Vertices[vertID].AddBoneData(BoneIndex, fWeight);
				}
			}	
		}
		
		BuildBuffers();
		
		log("------------------------------------------------------------------------");

		glm::mat4 mIdentity(1);
		ReadNodeHeirarchy(pScene->mRootNode, mIdentity);

		log("------------------------------------------------------------------------");

		// load anim data from assimp
		LoadAnimData(pScene);

		ReadAssimpHeirarchyData(m_RootNode, pScene->mRootNode);

		Importer.FreeScene();
	}

	void LoadAnimData(const aiScene* scene) {
		log("NumAnimation: " << scene->mNumAnimations);
		m_Animations.reserve(scene->mNumAnimations);

		for (uint iAnim = 0; iAnim < scene->mNumAnimations; iAnim++) {
			aiAnimation* pAnimation = scene->mAnimations[iAnim];
			log("NameAnimation: " << pAnimation->mName.C_Str());

			MyAnimation animation;
			animation.Name = pAnimation->mName.data;
			animation.Duration = pAnimation->mDuration;
			animation.TicksPerSecond = pAnimation->mTicksPerSecond;
			
			log("NumKeys: " << pAnimation->mNumChannels);
			animation.m_Keys.reserve(pAnimation->mNumChannels);

			for (uint iChanel = 0; iChanel < pAnimation->mNumChannels; iChanel++) {
				const aiNodeAnim* pNodeAnim = pAnimation->mChannels[iChanel];
				log("NodeName: " << pNodeAnim->mNodeName.data);

				Key key;
				key.PosKeys.reserve(pNodeAnim->mNumPositionKeys);
				
				//key.Name = pNodeAnim->mNodeName.data;

				std::string str = pNodeAnim->mNodeName.data;
				key.Name = str;// .substr(0, str.find_first_of('_'));

				for (uint iPosKey = 0; iPosKey < pNodeAnim->mNumPositionKeys; iPosKey++)
					key.PosKeys.push_back(KeyPos(toGlm(pNodeAnim->mPositionKeys[iPosKey].mValue), pNodeAnim->mPositionKeys[iPosKey].mTime));

				key.RotKeys.reserve(pNodeAnim->mNumRotationKeys);

				for (uint iRotKey = 0; iRotKey < pNodeAnim->mNumRotationKeys; iRotKey++)
					key.RotKeys.push_back(KeyRot(toGlm(pNodeAnim->mRotationKeys[iRotKey].mValue), pNodeAnim->mRotationKeys[iRotKey].mTime));

				key.ScalKeys.reserve(pNodeAnim->mNumScalingKeys);

				for (uint iScalKey = 0; iScalKey < pNodeAnim->mNumScalingKeys; iScalKey++)
					key.ScalKeys.push_back(KeyScal(toGlm(pNodeAnim->mScalingKeys[iScalKey].mValue), pNodeAnim->mScalingKeys[iScalKey].mTime));

				animation.m_Keys.push_back(key);
			}

			m_Animations.push_back(animation);
		}
	}

	void ReadAssimpHeirarchyData(AssimpNodeData& dest, const aiNode* src) {
		assert(src);

		dest.name = src->mName.data;
		dest.transformation = toGlm(src->mTransformation);
		dest.childrenCount = src->mNumChildren;

		for (uint i = 0; i < src->mNumChildren; i++) {
			AssimpNodeData newData;
			ReadAssimpHeirarchyData(newData, src->mChildren[i]);
			dest.children.push_back(newData);
		}
	}

	void ReadNodeHeirarchy(aiNode* pNode, const glm::mat4& ParentTransform) {
		log("aiNode name: " << pNode->mName.data);

		//glm::mat4 NodeTransform(1);// = toGlm(pNode->mTransformation);
		glm::mat4 NodeTransform = toGlm(pNode->mTransformation);

		//if(std::string(pNode->mName.data) == "joint3")
			//NodeTransform = toGlm(pNode->mTransformation);

		glm::mat4 GlobalTransform = ParentTransform * NodeTransform;

		std::string NodeName(pNode->mName.C_Str());
		
		//if (m_BonesMap.find(NodeName) != m_BonesMap.end()) {
		//	uint boneIndex = m_BonesMap[NodeName];
		//	m_Bones[boneIndex].FinalTransform = m_GlobalInverseTransform * GlobalTransform * m_Bones[boneIndex].Offset;
		//}
		
		
		for (uint i = 0; i < m_Bones.size(); i++) {
			if (m_Bones[i].Name == NodeName) {
				//m_Bones[i].Transformation = NodeTransform;
				//m_Bones[i].Transformation = GlobalTransform;
				m_Bones[i].FinalTransform = m_GlobalInverseTransform * GlobalTransform * m_Bones[i].Offset;
				//m_Bones[i].FinalTransform = GlobalTransform * m_Bones[i].Offset;
			}
			if (m_Bones[i].Name == "joint2") {
				//m_Bones[i].FinalTransform = m_Bones[i].Offset;
			}
			if (m_Bones[i].Name == "joint3") {
				//m_Bones[i].FinalTransform = m_Bones[i].Offset;
			}
		}

		for (uint i = 0; i < pNode->mNumChildren; i++) {
			ReadNodeHeirarchy(pNode->mChildren[i], GlobalTransform);
		}
	}

	void BuildBuffers() {
		glGenVertexArrays(1, &m_VAO);
		glGenBuffers(1, &m_VBO);
		glGenBuffers(1, &m_EBO);
	
		glBindVertexArray(m_VAO);
		glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(MyVertex2) * m_Vertices.size(), &m_Vertices[0], GL_STATIC_DRAW);
		
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * m_Indices.size(), &m_Indices[0], GL_STATIC_DRAW);
	
		GLsizei stride = sizeof(MyVertex2);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (const void*)0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, stride, (const void*)offsetof(MyVertex2, normal));
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, stride, (const void*)offsetof(MyVertex2, texCoord));
		
		glEnableVertexAttribArray(3);
		glVertexAttribIPointer(3, 4, GL_INT, stride, (const void*)offsetof(MyVertex2, boneIDs));
		glEnableVertexAttribArray(4);
		glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, stride, (const void*)offsetof(MyVertex2, weights));
	
		glBindVertexArray(0);
	}

	void UpdateAnim(float TimeInSec) {

		// have to calc anim time
		float TicksPerSeconds = (float)(m_Animations[0].TicksPerSecond != 0 ? m_Animations[0].TicksPerSecond : 25.0f);
		float TimeInTicks = TimeInSec * TicksPerSeconds;
		float AnimTimeTicks = fmod(TimeInTicks, (float)m_Animations[0].Duration);

		glm::mat4 mIdentity(1);
		UpdateAnimHeirarchy(AnimTimeTicks, &m_RootNode, mIdentity);
	}

	void UpdateAnimHeirarchy(float AnimTimeTicks, const AssimpNodeData* node, const glm::mat4& ParentTransform) {

		glm::mat4 NodeTransform = node->transformation;

		std::string NodeName(node->name);

		const Key* pKey = nullptr;
		for (uint ik = 0; ik < m_Animations[0].m_Keys.size(); ik++) {
			pKey = &m_Animations[0].m_Keys[ik];
			if (pKey->Name == NodeName) {
				break;
			}
			pKey = nullptr;
		}

		if (pKey) {
			// rotate
			glm::quat qrot;
			CalcInterpolateRotation(qrot, AnimTimeTicks, pKey);
			glm::mat4 mRot = glm::toMat4(qrot);

			// translate
			glm::vec3 pos;
			CalcInterpolatePosition(pos, AnimTimeTicks, pKey);
			glm::mat4 mTrans(1);
			mTrans = glm::translate(mTrans, pos);

			NodeTransform = mTrans * mRot;// *mScale;
		}
		
		glm::mat4 GlobalTransform = ParentTransform * NodeTransform;
		

		//glm::mat4 NodeTrans(1);
		//const auto& keys = m_Animations[0].m_Keys;

		for (uint i = 0; i < m_Bones.size(); i++) {
			if (m_Bones[i].Name == NodeName) {
				//log("NodeName: " << NodeName);

			//auto iter = std::find_if(keys.begin(), keys.end(), [&](const Key& key) { return key.Name == m_Bones[i].Name; });
			//if (iter != keys.end()) {
				//log("find: " << iter->Name);
					
				//log("mNumPositionKeys: " << pNodeAnim->mNumPositionKeys);
				//log("mNumRotationKeys: " << pNodeAnim->mNumRotationKeys);
				//log("mNumScalingKeys: " << pNodeAnim->mNumScalingKeys);				

				// rotate
				//glm::quat qrot;
				//CalcInterpolateRotation(qrot, AnimTimeTicks, &(*iter));
				//glm::mat4 mRot = glm::toMat4(qrot);

				//// translate
				//glm::vec3 pos;
				//CalcInterpolatePosition(pos, AnimTimeTicks, &(*iter));
				//glm::mat4 mTrans(1);
				//mTrans = glm::translate(mTrans, pos);

				//NodeTrans = mTrans * mRot;// *mScale;

				//glm::mat4 GlbTrans = ParentTransform * NodeTrans;

				//m_Bones[i].FinalTransform = m_GlobalInverseTransform * GlbTrans * m_Bones[i].Offset;
			//}
			//else NodeTrans = m_Bones[i].Transformation;


			//glm::mat4 GlbTrans = ParentTransform * NodeTrans;
			//glm::mat4 GlbTrans = ParentTransform * m_Bones[i].Transformation;
			//m_Bones[i].FinalTransform = m_GlobalInverseTransform * GlbTrans * m_Bones[i].Offset;

				m_Bones[i].FinalTransform = m_GlobalInverseTransform * GlobalTransform * m_Bones[i].Offset;
			}
		}

		for (int i = 0; i < node->childrenCount; i++)
			UpdateAnimHeirarchy(AnimTimeTicks, &node->children[i], GlobalTransform);
	}

	void CalcInterpolatePosition(glm::vec3& translation, float AnimationTimeTicks, const Key* pKey) {
		// we need at least two values to interpolate
		if (pKey->PosKeys.size() == 1) {
			translation = pKey->PosKeys[0].position;
			return;
		}

		uint PositionIndex = FindPosition(AnimationTimeTicks, pKey);
		uint NextPositionIndex = PositionIndex + 1;
		assert(NextPositionIndex < pKey->PosKeys.size());
		float t1 = (float)pKey->PosKeys[PositionIndex].time;
		float t2 = (float)pKey->PosKeys[NextPositionIndex].time;
		float deltaTime = t2 - t1;
		float Factor = (AnimationTimeTicks - t1) / deltaTime;
		assert(Factor >= 0.0f && Factor <= 1.0f);
		const glm::vec3& Start = pKey->PosKeys[PositionIndex].position;
		const glm::vec3& End = pKey->PosKeys[NextPositionIndex].position;
		glm::vec3 Delta = End - Start;
		translation = Start + Factor * Delta;
	}

	void CalcInterpolatePosition(glm::vec3& translation, float AnimationTimeTicks, const aiNodeAnim* pNodeAnim) {
		// we need at least two values to interpolate
		if (pNodeAnim->mNumPositionKeys == 1) {
			translation = toGlm(pNodeAnim->mPositionKeys[0].mValue);
			return;
		}

		uint PositionIndex = FindPosition(AnimationTimeTicks, pNodeAnim);
		uint NextPositionIndex = PositionIndex + 1;
		assert(NextPositionIndex < pNodeAnim->mNumPositionKeys);
		float t1 = (float)pNodeAnim->mPositionKeys[PositionIndex].mTime;
		float t2 = (float)pNodeAnim->mPositionKeys[NextPositionIndex].mTime;
		float deltaTime = t2 - t1;
		float Factor = (AnimationTimeTicks - t1) / deltaTime;
		assert(Factor >= 0.0f && Factor <= 1.0f);
		const glm::vec3& Start = toGlm(pNodeAnim->mPositionKeys[PositionIndex].mValue);
		const glm::vec3& End = toGlm(pNodeAnim->mPositionKeys[NextPositionIndex].mValue);
		glm::vec3 Delta = End - Start;
		translation = Start + Factor * Delta;
	}

	void CalcInterpolateRotation(glm::quat& qrot, float animTimeTicks, const Key* pKey) {
		if (pKey->RotKeys.size() == 1) {
			qrot = pKey->RotKeys[0].rotation;
			return;
		}

		uint RotIndex = FindRotation(animTimeTicks, pKey);
		uint NextRotIndex = RotIndex + 1;
		assert(NextRotIndex < pKey->RotKeys.size());
		float t1 = (float)pKey->RotKeys[RotIndex].time;
		float t2 = (float)pKey->RotKeys[NextRotIndex].time;
		float deltaTime = t2 - t1;
		float factor = (animTimeTicks - t1) / deltaTime;
		assert(factor >= .0f && factor <= 1.f);
		const glm::quat startRot = pKey->RotKeys[RotIndex].rotation;
		const glm::quat endRot = pKey->RotKeys[NextRotIndex].rotation;
		//qrot = glm::lerp(startRot, endRot, factor);
		qrot = glm::slerp(startRot, endRot, factor);
		//qrot = startRot;
		qrot = glm::normalize(qrot);
	}

	void CalcInterpolateRotation(glm::quat& qrot, float animTimeTicks, const aiNodeAnim* pNodeAnim) {
		if (pNodeAnim->mNumRotationKeys == 1) {
			qrot = toGlm(pNodeAnim->mRotationKeys[0].mValue);
			return;
		}

		uint RotIndex = FindRotation(animTimeTicks, pNodeAnim);
		uint NextRotIndex = RotIndex + 1;
		assert(NextRotIndex < pNodeAnim->mNumRotationKeys);
		float t1 = (float)pNodeAnim->mRotationKeys[RotIndex].mTime;
		float t2 = (float)pNodeAnim->mRotationKeys[NextRotIndex].mTime;
		float deltaTime = t2 - t1;
		float factor = (animTimeTicks - t1) / deltaTime;
		assert(factor >= .0f && factor <= 1.f);
		const glm::quat startRot = toGlm(pNodeAnim->mRotationKeys[RotIndex].mValue);
		const glm::quat endRot = toGlm(pNodeAnim->mRotationKeys[NextRotIndex].mValue);
		//qrot = glm::lerp(startRot, endRot, factor);
		qrot = glm::slerp(startRot, endRot, factor);
		//qrot = startRot;
		qrot = glm::normalize(qrot);
	}

	uint FindRotation(float AnimTimeTicks, const Key* pKey) {
		assert(pKey->RotKeys.size() > 0);
		for (uint i = 0; i < pKey->RotKeys.size() - 1; i++) {
			float t = (float)pKey->RotKeys[i + 1].time;
			if (AnimTimeTicks < t) return i;
		}
		return 0;
	}

	uint FindRotation(float AnimTimeTicks, const aiNodeAnim * pNodeAnim) {
		assert(pNodeAnim->mNumRotationKeys > 0);
		for (uint i = 0; i < pNodeAnim->mNumRotationKeys - 1; i++) {
			float t = (float)pNodeAnim->mRotationKeys[i + 1].mTime;
			if (AnimTimeTicks < t) return i;
		}
		return 0;
	}

	uint FindPosition(float AnimTimeTicks, const Key* pKey) {
		assert(pKey->PosKeys.size() > 0);
		for (uint i = 0; i < pKey->PosKeys.size() - 1; i++) {
			float t = (float)pKey->PosKeys[i + 1].time;
			if (AnimTimeTicks < t) return i;
		}
		return 0;
	}

	uint FindPosition(float AnimTimeTicks, const aiNodeAnim * pNodeAnim) {
		assert(pNodeAnim->mNumPositionKeys > 0);
		for (uint i = 0; i < pNodeAnim->mNumPositionKeys - 1; i++) {
			float t = (float)pNodeAnim->mPositionKeys[i + 1].mTime;
			if (AnimTimeTicks < t) return i;
		}
		return 0;
	}

	void Render() {
		glBindVertexArray(m_VAO);

		for (int i = 0; i < m_Meshes.size(); i++) {
			uint matIndex = m_Meshes[i].MaterialIndex;
			if(m_Textures.size())
				if(m_Textures[matIndex]) m_Textures[matIndex]->Bind();

			glDrawElementsBaseVertex(GL_TRIANGLES, m_Meshes[i].NumIndices, GL_UNSIGNED_INT, (void*)(sizeof(uint) * m_Meshes[i].BaseIndex), m_Meshes[i].BaseVertex);
		}

		glBindVertexArray(0);
	}
};
