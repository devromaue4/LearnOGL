#pragma once

#include "Texture.h"

struct MyVertex {
	glm::vec3 position; // pos
	glm::vec3 normal;	// normal
	glm::vec2 texCoord; // tex coord

	//int	boneIDs[MAX_BONE_INFLUENCE]{ -1,-1,-1,-1 };
	uint	boneIDs[MAX_BONE_INFLUENCE]{};
	float	weights[MAX_BONE_INFLUENCE]{};
};

class MyMesh {
	GLuint VAO, VBO, EBO;

public:
	std::vector<MyVertex> verts;
	std::vector<uint> indices;
	std::vector<Texture*> textures;

	void Clear() {
		if (EBO != 0) glDeleteBuffers(1, &EBO);
		if (VBO != 0) glDeleteBuffers(1, &VBO);

		if (VAO != 0) {
			glDeleteVertexArrays(1, &VAO);
			VAO = 0;
		}
	}

	void BuildBuffers() {	
		Clear();

		glGenVertexArrays(1, &VAO);
		glGenBuffers(1, &VBO);
		glGenBuffers(1, &EBO);

		glBindVertexArray(VAO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(MyVertex) * verts.size(), &verts[0], GL_STATIC_DRAW);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint) * indices.size(), &indices[0], GL_STATIC_DRAW);

		GLsizei stride = sizeof(MyVertex);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void*)0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, stride, (void*)offsetof(MyVertex, normal));
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, stride, (void*)offsetof(MyVertex, texCoord));

		// for skinning
		glEnableVertexAttribArray(3);
		glVertexAttribIPointer(3, 4, GL_INT, stride, (void*)offsetof(MyVertex, boneIDs));
		glEnableVertexAttribArray(4);
		glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, stride, (void*)offsetof(MyVertex, weights));

		glBindVertexArray(0);
	}

	void Draw() {
		for (int i = 0; i < textures.size(); i++) {
			textures[i]->Bind();
		}

		glBindVertexArray(VAO);
		glPointSize(5);
		glDrawElements(GL_TRIANGLES, (GLsizei)indices.size(), GL_UNSIGNED_INT, 0);
		//glDrawElements(GL_POINTS, (GLsizei)indices.size(), GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);

		//glActiveTexture(GL_TEXTURE0);
	}
};

struct MyBone {
	std::string name;
	glm::mat4 Offset;
	glm::mat4 Transform = glm::mat4(0);
};

class MySkeletalModel {
	std::vector<MyMesh> m_Meshes;

	std::string m_Directory;

	glm::mat4 m_GlobalInverseTransform;

	Assimp::Importer Importer;
	const aiScene* m_pScene;

public:
	std::vector<MyBone> m_Bones;
	std::map<std::string, uint> m_BonesMap;

public:
	bool Load(std::string path, bool flipUVs = false) {
		uint flags = aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_JoinIdenticalVertices;
		if(flipUVs) flags |= aiProcess_FlipUVs; // set flag

		//Assimp::Importer Importer;
		//const aiScene* m_pScene;
		m_pScene = Importer.ReadFile(path, flags);
		if (!m_pScene || m_pScene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !m_pScene->mRootNode) {
			std::cout << "MySkeletalModel::load " << Importer.GetErrorString() << std::endl;
			throw std::exception("failed to loadModel!");
		}

		m_Directory = path.substr(0, path.find_last_of('/'));

		m_GlobalInverseTransform = glm::inverse(toGlm(m_pScene->mRootNode->mTransformation));

		uint NumMeshes = m_pScene->mNumMeshes;
		m_Meshes.reserve(NumMeshes);
		for (uint imesh = 0; imesh < NumMeshes; imesh++) {
			uint numVertices = m_pScene->mMeshes[imesh]->mNumVertices;
			MyMesh mesh;
			mesh.verts.reserve(numVertices);
			//if (m_pScene->mMeshes[imesh]->HasPositions()) { // vertices
				for (uint vIndx = 0; vIndx < numVertices; vIndx++) {
					aiVector3D aiVec = m_pScene->mMeshes[imesh]->mVertices[vIndx];
					MyVertex myVert;
					myVert.position = glm::vec3(aiVec.x, aiVec.y, aiVec.z);

					if (m_pScene->mMeshes[imesh]->HasNormals()) { // normals
						aiVector3D aiNormal = m_pScene->mMeshes[imesh]->mNormals[vIndx];
						myVert.normal = glm::vec3(aiNormal.x, aiNormal.y, aiNormal.z);
					}
					else myVert.normal = glm::vec3(.0f, 1.f, .0f);

					if (m_pScene->mMeshes[imesh]->HasTextureCoords(0)) { // tex coords
						aiVector3D aiTXC = m_pScene->mMeshes[imesh]->mTextureCoords[0][vIndx];
						myVert.texCoord = glm::vec2(aiTXC.x, aiTXC.y);
					}
					else myVert.texCoord = glm::vec2(.0f, .0f);

					mesh.verts.push_back(myVert);
				}
			//}

			aiMaterial* aiMat = m_pScene->mMaterials[m_pScene->mMeshes[imesh]->mMaterialIndex];
			uint numTextures = aiMat->GetTextureCount(aiTextureType_DIFFUSE);
			for (uint iTex = 0; iTex < numTextures; iTex++) {
				aiString str;
				aiMat->GetTexture(aiTextureType_DIFFUSE, iTex, &str);
				std::string filePath = m_Directory + "/" + str.C_Str();
				Texture* texture = new Texture(filePath.c_str());
				texture->Load();
				mesh.textures.push_back(texture);
			}

			// skinning fill bone weights
			uint NumBones = m_pScene->mMeshes[imesh]->mNumBones;
			//m_Bones.reserve(NumBones);
			for (uint iBone = 0; iBone < NumBones; iBone++) {

				aiBone* pBone = m_pScene->mMeshes[imesh]->mBones[iBone];

				// load single bone
				uint BoneIndex = 0;
				std::string BoneName(pBone->mName.C_Str());
				if (m_BonesMap.find(BoneName) == m_BonesMap.end()) {
					//BoneIndex = (uint)m_Bones.size();
					BoneIndex = (uint)m_BonesMap.size();
					m_BonesMap[BoneName] = BoneIndex;

					MyBone bone;
					//bone.name = pBone->mName.C_Str();
					bone.name = BoneName;
					bone.Offset = toGlm(pBone->mOffsetMatrix);
					m_Bones.push_back(bone);
				}
				else BoneIndex = m_BonesMap[BoneName];

				//if (BoneIndex == m_Bones.size()) {
				//	MyBone bone;
				//	bone.name = pBone->mName.C_Str();
				//	bone.Offset = toGlm(pBone->mOffsetMatrix);
				//	m_Bones.push_back(bone);
				//}

				uint numWeights = pBone->mNumWeights;
				for (uint iweight = 0; iweight < numWeights; iweight++) {

					uint vertID = pBone->mWeights[iweight].mVertexId;
					float fWeight = pBone->mWeights[iweight].mWeight;

					for (uint slot = 0; slot < MAX_BONE_INFLUENCE; slot++) {
						//if (mesh.verts[vertID].boneIDs[slot] < 0) {
						if (mesh.verts[vertID].weights[slot] == 0.0f) {
							mesh.verts[vertID].boneIDs[slot] = BoneIndex;// iBone;
							//mesh.verts[vertID].boneIDs[slot] = m_BoneCounter;// iBone;
							//mesh.verts[vertID].boneIDs[slot] = iBone;
							mesh.verts[vertID].weights[slot] = fWeight;
							break;
						}
						//else continue;

						// should never get here - more bones than we have space for
						//log("WARNING: MySkeletalModel2::Load: need more bones than we have space for!!!");
						//assert(0);
					}
				}
			}

			//if (m_pScene->mMeshes[imesh]->HasFaces()) { // indices
				uint numFaces = m_pScene->mMeshes[imesh]->mNumFaces;
				//mesh.indices.reserve(numFaces);
				mesh.indices.reserve(numFaces * 3);
				for (uint faceId = 0; faceId < numFaces; faceId++) {
					const aiFace& face = m_pScene->mMeshes[imesh]->mFaces[faceId];
					assert(face.mNumIndices == 3);
					mesh.indices.push_back(face.mIndices[0]);
					mesh.indices.push_back(face.mIndices[1]);
					mesh.indices.push_back(face.mIndices[2]);
				}
			//}

			mesh.BuildBuffers();
			m_Meshes.push_back(mesh);
		}

		glm::mat4 Identity(1);
		ReadNodeHierarchy(m_pScene->mRootNode, Identity);
		//ReadNodeHierarchy(m_pScene->mRootNode, toGlm(m_pScene->mRootNode->mTransformation));

		return true;
	}

	void ReadNodeHierarchy(aiNode* pNode, const glm::mat4& ParentTransform) {
		glm::mat4 NodeTransform = toGlm(pNode->mTransformation);
		
		glm::mat4 GlobalTransform = ParentTransform * NodeTransform;
	
		for (uint io = 0; io < m_Bones.size(); io++) {
			if (m_Bones[io].name == pNode->mName.C_Str()) {
				//m_Bones[io].Transform = GlobalTransform * m_Bones[io].Offset;
				m_Bones[io].Transform = m_GlobalInverseTransform * GlobalTransform * m_Bones[io].Offset;
				break;
			}
		}

		//string NodeName(pNode->mName.data);
		//if (m_BonesMap.find(NodeName) != m_BonesMap.end()) {
		//	uint BoneIndex = m_BonesMap[NodeName];
		//	m_Bones[BoneIndex].Transform = m_GlobalInverseTransform * GlobalTransform * m_Bones[BoneIndex].Offset;
		//}

		//for (size_t i = 0; i < m_Bones.size(); i++) { // not work
		//	m_Bones[i].Transform = m_GlobalInverseTransform * GlobalTransform * m_Bones[i].Offset;
		//}

		for (uint i = 0; i < pNode->mNumChildren; i++) {
			ReadNodeHierarchy(pNode->mChildren[i], GlobalTransform);
		}
	}

	void UpdateNodeHierarchy(float AnimTimeTicks, const aiNode* pNode, const glm::mat4& ParentTransform) {

		const aiAnimation* pAnimation = m_pScene->mAnimations[0];

		glm::mat4 NodeTransform = toGlm(pNode->mTransformation);

		std::string NodeName(pNode->mName.data);

		// find node anim
		aiNodeAnim* pNodeAnim = nullptr;
		for (uint i = 0; i < pAnimation->mNumChannels; i++) {
			pNodeAnim = pAnimation->mChannels[i];
			//if (pNodeAnim->mNodeName == pNode->mName) break;
			if (std::string(pNodeAnim->mNodeName.data) == NodeName) break;

			pNodeAnim = nullptr; // if remove or comment this line well be bug (i spend a lot of time to find it) -> !!!!!!!!!UNCOMMENT THIS LINE!!!!!!!!!
		}

		if (pNodeAnim) {

			//glm::vec3 scale;
			//glm::mat4 mScale(1);
			//CalcInterpolateScaling(scale, AnimTimeTicks, pNodeAnim);
			//mScale = glm::scale(glm::mat4(1), scale);

			// rotate
			glm::quat qrot;
			CalcInterpolateRotation(qrot, AnimTimeTicks, pNodeAnim);
			//glm::mat4 mRot = glm::mat4_cast(qrot);
			glm::mat4 mRot = glm::toMat4(qrot);

			// translate
			glm::vec3 pos;
			CalcInterpolatePosition(pos, AnimTimeTicks, pNodeAnim);
			glm::mat4 mTrans(1);
			mTrans = glm::translate(mTrans, pos);

			NodeTransform = mTrans * mRot;// *mScale;
			//NodeTransform = mRot;// *mScale;
		}

		glm::mat4 GlobalTransform = ParentTransform * NodeTransform;

		for (uint io = 0; io < m_Bones.size(); io++) {
			if (m_Bones[io].name == pNode->mName.C_Str()) {
				m_Bones[io].Transform = m_GlobalInverseTransform * GlobalTransform * m_Bones[io].Offset;
				break;
			}
		}

		//if (m_BonesMap.find(NodeName) != m_BonesMap.end()) {
		//	uint BoneIndex = m_BonesMap[NodeName];
		//	//m_Bones[BoneIndex].Transform = GlobalTransform * m_Bones[BoneIndex].Offset;
		//	m_Bones[BoneIndex].Transform = m_GlobalInverseTransform * GlobalTransform * m_Bones[BoneIndex].Offset;
		//}

		for (uint i = 0; i < pNode->mNumChildren; i++) {
			UpdateNodeHierarchy(AnimTimeTicks, pNode->mChildren[i], GlobalTransform);
		}
	}

	// for testing purpose
	void UpdateAnim(float TimeInSeconds) {

		glm::mat4 Identity(1);

		// have to calc anim time
		float TicksPerSeconds = (float)(m_pScene->mAnimations[0]->mTicksPerSecond != 0 ? m_pScene->mAnimations[0]->mTicksPerSecond : 25.0f);
		float TimeInTicks = TimeInSeconds * TicksPerSeconds;
		float AnimTimeTicks = fmod(TimeInTicks, (float)m_pScene->mAnimations[0]->mDuration);

		UpdateNodeHierarchy(AnimTimeTicks, m_pScene->mRootNode, Identity);
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

	void CalcInterpolateScaling(glm::vec3& scale, float animTimeTicks, const aiNodeAnim* pNodeAnim) {
		if (pNodeAnim->mNumScalingKeys == 1) {
			scale = toGlm(pNodeAnim->mScalingKeys[0].mValue);
			return;
		}

		uint ScaleIndex = FindScaling(animTimeTicks, pNodeAnim);
		uint NextScaleIndex = ScaleIndex + 1;
		assert(NextScaleIndex < pNodeAnim->mNumScalingKeys);
		float t1 = (float)pNodeAnim->mScalingKeys[ScaleIndex].mTime;
		float t2 = (float)pNodeAnim->mScalingKeys[NextScaleIndex].mTime;
		float deltaTime = t2 - t1;
		float factor = (animTimeTicks - t1) / deltaTime;
		assert(factor >= .0f && factor <= 1.f);
		const glm::vec3 start = toGlm(pNodeAnim->mScalingKeys[ScaleIndex].mValue);
		const glm::vec3 end = toGlm(pNodeAnim->mScalingKeys[NextScaleIndex].mValue);
		glm::vec3 delta = end - start;
		scale = start + factor * delta;
	}

	uint FindPosition(float AnimTimeTicks, const aiNodeAnim* pNodeAnim) {
		assert(pNodeAnim->mNumPositionKeys > 0);
		for (uint i = 0; i < pNodeAnim->mNumPositionKeys - 1; i++) {
			float t = (float)pNodeAnim->mPositionKeys[i + 1].mTime;
			if (AnimTimeTicks < t) return i;
		}
		return 0;
	}

	uint FindRotation(float AnimTimeTicks, const aiNodeAnim* pNodeAnim) {
		assert(pNodeAnim->mNumRotationKeys > 0);
		for (uint i = 0; i < pNodeAnim->mNumRotationKeys - 1; i++) {
			float t = (float)pNodeAnim->mRotationKeys[i + 1].mTime;
			if (AnimTimeTicks < t) return i;
		}
		return 0;
	}

	uint FindScaling(float AnimTimeTicks, const aiNodeAnim* pNodeAnim) {
		assert(pNodeAnim->mNumScalingKeys > 0);
		for (uint i = 0; i < pNodeAnim->mNumScalingKeys - 1; i++) {
			float t = (float)pNodeAnim->mScalingKeys[i + 1].mTime;
			if (AnimTimeTicks < t) return i;
		}
		return 0;
	}

	void Render() {
		for (size_t i = 0; i < m_Meshes.size(); i++)
			m_Meshes[i].Draw();
	}
};