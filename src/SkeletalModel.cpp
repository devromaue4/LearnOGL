#include "SkeletalModel.h"
#include <numeric>

void SkeletalModel::InitReqNodeMap(const aiNode* pNode) {
	std::string NodeName(pNode->mName.C_Str());
	NodeInfo info(pNode);

	m_RequiredNodeMap[NodeName] = info;

	for (uint i = 0; i < pNode->mNumChildren; i++)
		InitReqNodeMap(pNode->mChildren[i]);
}

void SkeletalModel::MarkReqNodesForBone(const aiBone* pBone) {
	std::string NodeName(pBone->mName.data);

	const aiNode* pParent = nullptr;

	do {
		std::map<std::string, NodeInfo>::iterator it = m_RequiredNodeMap.find(NodeName);
		if (it == m_RequiredNodeMap.end()) {
			log_error("Cannot find bone in the hierarchy " << NodeName.c_str());
			assert(0);
		}

		it->second.isRequired = true;
		pParent = it->second.pNode->mParent;
		if (pParent)
			NodeName = std::string(pParent->mName.data);

	} while (pParent);
}

void SkeletalModel::Load(std::string_view fileName, bool bFlipUVs) {
	clear();

	uint flags = aiProcess_LimitBoneWeights |
		aiProcess_Triangulate
		| aiProcess_GenSmoothNormals
		| aiProcess_CalcTangentSpace
		| aiProcess_JoinIdenticalVertices;
	if (bFlipUVs) flags |= aiProcess_FlipUVs;

	Assimp::Importer importer;
	const aiScene* pScene = importer.ReadFile(fileName.data(), flags);
	if (!pScene || pScene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !pScene->mRootNode) {
		log(importer.GetErrorString());
		throw std::exception("MySkeletalModel3::Load!");
	}

	m_Directory = fileName.substr(0, fileName.find_last_of('/'));

	m_GlobalInverseTransform = glm::inverse(toGlm(pScene->mRootNode->mTransformation));

	m_Meshes.resize(pScene->mNumMeshes);
	m_Textures.resize(pScene->mNumMaterials);

	uint numVertices = 0, numIndices = 0;
	calcVertices(pScene, numVertices, numIndices);

	m_Vertices.reserve(numVertices);
	m_Indices.reserve(numIndices);

	InitReqNodeMap(pScene->mRootNode);

	loadGeoData(pScene);

	loadAnimData(pScene);

	//log("-----------------------------------------------------------");
	ReadNodeHierarchy(m_RootNode, pScene->mRootNode, glm::mat4(1.0f));
	//log("NumNodes: " << numNodes);
	//log("-----------------------------------------------------------");

	loadMaterials(pScene);

	buildBuffers();

	importer.FreeScene();
}

void SkeletalModel::calcVertices(const aiScene* pScene, uint& numVertices, uint& numIndices) {
	for (uint iMesh = 0; iMesh < pScene->mNumMeshes; iMesh++) {
		m_Meshes[iMesh].MaterialIndex = pScene->mMeshes[iMesh]->mMaterialIndex;
		m_Meshes[iMesh].BaseVertex = numVertices;
		m_Meshes[iMesh].BaseIndex = numIndices;

		numVertices += pScene->mMeshes[iMesh]->mNumVertices;
		m_Meshes[iMesh].NumIndices = pScene->mMeshes[iMesh]->mNumFaces * 3;
		numIndices += m_Meshes[iMesh].NumIndices;
	}
}

void SkeletalModel::loadGeoData(const aiScene* pScene) {
	WeightedVertex myVert;
	Bone myBone;
	uint BoneIndex = 0;

	for (uint iMesh = 0; iMesh < pScene->mNumMeshes; iMesh++) {
		const aiMesh* pMesh = pScene->mMeshes[iMesh];
		for (uint iVert = 0; iVert < pMesh->mNumVertices; iVert++) {
			myVert.pos = toGlm(pMesh->mVertices[iVert]);

			if (pMesh->HasNormals())
				myVert.normal = toGlm(pMesh->mNormals[iVert]);
			else myVert.normal = glm::vec3(.0f, 1.f, .0f);

			if (pMesh->HasTextureCoords(0)) {
				const aiVector3D& tc = pMesh->mTextureCoords[0][iVert];
				myVert.texCoord = glm::vec2(tc.x, tc.y);
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
				myBone.Offset = toGlm(pBone->mOffsetMatrix);
				m_Bones.push_back(myBone); // realloc everytime not efficient

				MarkReqNodesForBone(pBone);
			}
			else BoneIndex = m_BonesMap[BoneName];

			for (uint iWeight = 0; iWeight < pBone->mNumWeights; iWeight++) {

				uint vertID = m_Meshes[iMesh].BaseVertex + pBone->mWeights[iWeight].mVertexId;
				float fWeight = pBone->mWeights[iWeight].mWeight;

				m_Vertices[vertID].addWeight(BoneIndex, fWeight);
			}

			//MarkReqNodesForBone(pBone);
		}
	}

	//NormalizeWeights(); // slow down loading
}

void SkeletalModel::NormalizeWeights() {
	float threshold = 0.05f;
	for (uint i = 0; i < m_Vertices.size(); i++) {
		WeightedVertex& vertex = m_Vertices[i];
		std::vector<float> validWeights;
		for (int j = 0; j < MAX_BONE_INFLUENCE; j++) {
			if (vertex.weights[j] < threshold)
				vertex.weights[j] = 0.0f;
			else
				validWeights.push_back(vertex.weights[j]);
		}

		float sum = std::accumulate(validWeights.begin(), validWeights.end(), 0.0f);
		if (sum == 0.0f || sum == 1.0f) continue;

		int validIndex = 0;
		for (int j = 0; j < MAX_BONE_INFLUENCE; j++) {
			if (vertex.weights[j] > 0.0f) {
				vertex.weights[j] = validWeights[validIndex] / sum;
				validIndex++;
			}
		}
	}
}

//void SkeletalModel::Optimize() {
//	for (uint i = 0; i < m_Vertices.size(); i++) {
//		bool skip = false;
//		for (uint j = 0; j < m_VerticesOptimized.size(); j++) {
//			if (m_Vertices[i] == m_VerticesOptimized[j]) {
//				skip = true;
//				break;
//			}
//		}
//
//		if (!skip) m_VerticesOptimized.push_back(m_Vertices[i]);
//	}
//}

void SkeletalModel::loadMaterials(const aiScene* pScene) {
	for (uint i = 0; i < pScene->mNumMaterials; i++)
		loadDiffuseTexture(pScene, pScene->mMaterials[i], i);
}

void SkeletalModel::loadDiffuseTexture(const aiScene* pScene, const aiMaterial* pMaterial, int index) {
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

void SkeletalModel::buildBuffers() {
	glGenVertexArrays(1, &m_VAO);
	glGenBuffers(1, &m_VBO);
	glGenBuffers(1, &m_EBO);

	glBindVertexArray(m_VAO);
	glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(WeightedVertex) * m_Vertices.size(), &m_Vertices[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint) * m_Indices.size(), &m_Indices[0], GL_STATIC_DRAW);

	GLsizei stride = sizeof(WeightedVertex);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (const void*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, stride, (const void*)offsetof(WeightedVertex, normal));
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, stride, (const void*)offsetof(WeightedVertex, texCoord));
	glEnableVertexAttribArray(3);
	glVertexAttribIPointer(3, 4, GL_INT, stride, (const void*)offsetof(WeightedVertex, boneIDs));
	glEnableVertexAttribArray(4);
	glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, stride, (const void*)offsetof(WeightedVertex, weights));

	glBindVertexArray(0);
}

void SkeletalModel::loadAnimData(const aiScene* scene) {
	//log("NumAnimation: " << scene->mNumAnimations);
	m_Animations.reserve(scene->mNumAnimations);

	for (uint iAnim = 0; iAnim < scene->mNumAnimations; iAnim++) {
		aiAnimation* pAnimation = scene->mAnimations[iAnim];
		//log("NameAnimation: " << pAnimation->mName.C_Str());

		MyAnimation animation;
		animation.Name = pAnimation->mName.data;
		animation.Duration = pAnimation->mDuration;
		animation.TicksPerSecond = pAnimation->mTicksPerSecond;

		//log("NumKeys: " << pAnimation->mNumChannels);
		animation.m_Keys.reserve(pAnimation->mNumChannels);

		for (uint iChanel = 0; iChanel < pAnimation->mNumChannels; iChanel++) {
			const aiNodeAnim* pNodeAnim = pAnimation->mChannels[iChanel];
			//log("NodeName: " << pNodeAnim->mNodeName.data);

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

void SkeletalModel::ReadNodeHierarchy(Node& node, const aiNode* pNode, const glm::mat4& mParentTransform) {
	glm::mat4 GlobalTransform = mParentTransform * toGlm(pNode->mTransformation);

	const char* NodeName = pNode->mName.C_Str();
	
	//log("NodeName: " << NodeName);
	node.Name = pNode->mName.data;
	node.InvBindTransform = toGlm(pNode->mTransformation);
	node.childrenCount = pNode->mNumChildren;
	//numNodes++;

	if (m_BonesMap.find(NodeName) != m_BonesMap.end()) {
		uint BoneIndex = m_BonesMap[NodeName];
		m_Bones[BoneIndex].FinalTransform = m_GlobalInverseTransform * GlobalTransform * m_Bones[BoneIndex].Offset;
	}

	for (uint i = 0; i < pNode->mNumChildren; i++) {
		Node lNode;
		ReadNodeHierarchy(lNode, pNode->mChildren[i], GlobalTransform);
		node.children.push_back(lNode);
	}
}

void SkeletalModel::UpdateAnimBlended(float TimeInSec, uint animA, uint animB, float BlendFactor) {
	if (animA >= m_Animations.size() || animB >= m_Animations.size()) {
		log("No Animation"); assert(0);
	}
	if (BlendFactor < 0.0f || BlendFactor > 1.0f) {
		log_error("invalid blend factor"); assert(0);
	}

	float AnimTimeTicksA = CalcAnimTimeTicks(TimeInSec, animA);
	float AnimTimeTicksB = CalcAnimTimeTicks(TimeInSec, animB);

	const MyAnimation& pAnimA = m_Animations[animA];
	const MyAnimation& pAnimB = m_Animations[animB];

	UpdateNodeHierarchyBlended(AnimTimeTicksA, AnimTimeTicksB, m_RootNode, glm::mat4(1.0f), pAnimA, pAnimB, BlendFactor);
}

void SkeletalModel::UpdateNodeHierarchyBlended(float animTimeA, float animTimeB, const Node& pNode, const glm::mat4& parentTrans, const MyAnimation& pAnimA, const MyAnimation& pAnimB, float blendFactor) {
	glm::mat4 NodeTransform = pNode.InvBindTransform;

	const Key* pStartNodeAnim = FindNodeAnim(pAnimA, pNode.Name);

	// calc local transform pStartNodeAnim
	LocalTranform transA;
	if (pStartNodeAnim) {
		CalcInterpolatePosition(animTimeA, pStartNodeAnim, transA.pos);
		CalcInterpolateRotation(animTimeA, pStartNodeAnim, transA.rot);
		CalcInterpolateScaling(animTimeA, pStartNodeAnim, transA.scale);
	}

	const Key* pEndNodeAnim = FindNodeAnim(pAnimB, pNode.Name);

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
		//glm::vec3 blendedTrans = (1.0f - blendFactor) * transA.pos + transB.pos * blendFactor;
		glm::vec3 blendedTrans = glm::mix(transA.pos, transB.pos, blendFactor);
		glm::mat4 mTrans(1.0f);
		mTrans = glm::translate(mTrans, blendedTrans);

		glm::quat rot = glm::slerp(transA.rot, transB.rot, blendFactor);
		rot = glm::normalize(rot);
		glm::mat4 mRot = glm::toMat4(rot);

		//glm::vec3 blendedScale = (1.0f - blendFactor) * transA.scale + transB.scale * blendFactor;
		glm::vec3 blendedScale = glm::mix(transA.scale, transB.scale, blendFactor);
		glm::mat4 mScale(1.0f);
		mScale = glm::scale(mScale, blendedScale);

		NodeTransform = mTrans * mRot * mScale;
	}

	glm::mat4 GlobalTransform = parentTrans * NodeTransform;

	if (m_BonesMap.find(pNode.Name) != m_BonesMap.end()) {
		uint BoneIndex = m_BonesMap[pNode.Name];
		m_Bones[BoneIndex].FinalTransform = m_GlobalInverseTransform * GlobalTransform * m_Bones[BoneIndex].Offset;
	}

	for (int i = 0; i < pNode.childrenCount; i++) {
		const std::string& childName = pNode.children[i].Name;

		std::map<std::string, NodeInfo>::iterator it = m_RequiredNodeMap.find(childName);
		if (it == m_RequiredNodeMap.end()) {
			log_error("Cannot find bone in the hierarchy " << childName.c_str());
			assert(0);
		}

		if (it->second.isRequired)
			UpdateNodeHierarchyBlended(animTimeA, animTimeB, pNode.children[i], GlobalTransform, pAnimA, pAnimB, blendFactor);
	}
}

float SkeletalModel::CalcAnimTimeTicks(float TimeInSec, unsigned int AnimIndex) {
	float TicksPerSecond = (float)(m_Animations[AnimIndex].TicksPerSecond != 0 ? m_Animations[AnimIndex].TicksPerSecond : 25.0f);
	float TimeInTicks = TimeInSec * TicksPerSecond;
	// we need to use the integral part of mDuration for the total length of the animation
	float Duration = 0.0f;
	//float fraction = 
	modf((float)m_Animations[AnimIndex].Duration, &Duration);
	float AnimationTimeTicks = fmod(TimeInTicks, Duration);
	return AnimationTimeTicks;
}

void SkeletalModel::UpdateAnim(float TimeInSec, uint AnimIndex) {

	if (m_Animations.size() == 0 || AnimIndex > m_Animations.size() - 1) {
		log("No Animation");
		return;
	}

	// calc anim time
	float TimePerSeconds = (float)(m_Animations[AnimIndex].TicksPerSecond != 0 ? m_Animations[AnimIndex].TicksPerSecond : 25.0f);
	float TimeInTicks = TimeInSec * TimePerSeconds;
	float AnimTimeTicks = (float)fmod(TimeInTicks, m_Animations[AnimIndex].Duration);

	UpdateAnimHierarchy(AnimTimeTicks, &m_RootNode, glm::mat4(1.0f), AnimIndex);
}

void SkeletalModel::UpdateAnimHierarchy(float AnimTimeTicks, const Node* pNode, const glm::mat4& mParentTransform, int AnimIndex) {

	glm::mat4 mNodeTransform = pNode->InvBindTransform;

	std::string NodeName(pNode->Name);

	// find keys and interpolate between them
	const Key* pKey = FindNodeAnim(m_Animations[AnimIndex], NodeName);
	if (pKey) {
		glm::vec3 pos;
		CalcInterpolatePosition(AnimTimeTicks, pKey, pos);
		glm::mat4 mTrans(1.0f);
		mTrans = glm::translate(mTrans, pos);

		glm::quat rot;
		CalcInterpolateRotation(AnimTimeTicks, pKey, rot);
		glm::mat4 mRot = glm::toMat4(rot);

		glm::vec3 scale;
		CalcInterpolateScaling(AnimTimeTicks, pKey, scale);
		glm::mat4 mScale(1.0f);
		mScale = glm::scale(mScale, scale);

		mNodeTransform = mTrans * mRot * mScale;
	}

	glm::mat4 GlobalTransform = mParentTransform * mNodeTransform;

	if (m_BonesMap.find(NodeName) != m_BonesMap.end()) {
		uint BoneIndex = m_BonesMap[NodeName];
		m_Bones[BoneIndex].FinalTransform = m_GlobalInverseTransform * GlobalTransform * m_Bones[BoneIndex].Offset;
	}

	for (int i = 0; i < pNode->childrenCount; i++) {
		const std::string& childName = pNode->children[i].Name;

		std::map<std::string, NodeInfo>::iterator it = m_RequiredNodeMap.find(childName);
		if (it == m_RequiredNodeMap.end()) {
			log_error("Cannot find bone in the hierarchy " << childName.c_str());
			assert(0);
		}

		if (it->second.isRequired)
			UpdateAnimHierarchy(AnimTimeTicks, &pNode->children[i], GlobalTransform, AnimIndex);
	}
}

void SkeletalModel::CalcInterpolatePosition(float AnimTimeTicks, const Key* pAnimNode, glm::vec3& pos) {
	if (pAnimNode->PosKeys.size() == 1) {
		pos = pAnimNode->PosKeys[0].position;
		return;
	}

	uint index = FindPosIndex(AnimTimeTicks, pAnimNode);
	uint nextIndex = index + 1;
	assert(nextIndex < pAnimNode->PosKeys.size());
	float factor = GetScaleFactor(pAnimNode->PosKeys[index].time, pAnimNode->PosKeys[nextIndex].time, AnimTimeTicks);
	pos = glm::mix(pAnimNode->PosKeys[index].position, pAnimNode->PosKeys[nextIndex].position, factor);
}

void SkeletalModel::CalcInterpolateRotation(float AnimTimeTicks, const Key* pAnimNode, glm::quat& rot) {
	if (pAnimNode->RotKeys.size() == 1) {
		rot = pAnimNode->RotKeys[0].rotation;
		return;
	}

	uint index = FindRotIndex(AnimTimeTicks, pAnimNode);
	uint nextIndex = index + 1;
	assert(nextIndex < pAnimNode->RotKeys.size());
	float factor = GetScaleFactor(pAnimNode->RotKeys[index].time, pAnimNode->RotKeys[nextIndex].time, AnimTimeTicks);
	rot = glm::slerp(pAnimNode->RotKeys[index].rotation, pAnimNode->RotKeys[nextIndex].rotation, factor);
	rot = glm::normalize(rot);
}

void SkeletalModel::CalcInterpolateScaling(float AnimTimeTicks, const Key* pAnimNode, glm::vec3& scale) {
	if (pAnimNode->ScalKeys.size() == 1) {
		scale = pAnimNode->ScalKeys[0].scale;
		return;
	}

	uint index = FindScaleIndex(AnimTimeTicks, pAnimNode);
	uint nextIndex = index + 1;
	assert(nextIndex < pAnimNode->ScalKeys.size());
	float factor = GetScaleFactor(pAnimNode->ScalKeys[index].time, pAnimNode->ScalKeys[nextIndex].time, AnimTimeTicks);
	scale = glm::mix(pAnimNode->ScalKeys[index].scale, pAnimNode->ScalKeys[nextIndex].scale, factor);
}

float SkeletalModel::GetScaleFactor(double firstPos, double nextPos, float animTime) {
	double deltaTime = nextPos - firstPos;
	double factor = (animTime - firstPos) / deltaTime;
	assert(factor >= 0.0f && factor <= 1.0f);
	return (float)factor;
}

const Key* SkeletalModel::FindNodeAnim(const MyAnimation& pAnim, std::string_view NodeName) {
	//const aiNodeAnim* pAnimNode = nullptr;
	const Key* pkey = nullptr;
	//const aiAnimation* pAnim = m_pScene->mAnimations[AnimIndex]; // 0 get first animation
	//if (pAnim) {
		for (uint ikey = 0; ikey < pAnim.m_Keys.size(); ikey++) {
			pkey = &pAnim.m_Keys[ikey];
			if (pkey->Name == NodeName)
				return pkey;
			//if (pAnimNode->mNodeName.data == NodeName)
				//return pAnimNode;
		}
	//}

	return nullptr;
}

uint SkeletalModel::FindPosIndex(float AnimTimeTicks, const Key* pAnimNode) {
	for (uint i = 0; i < pAnimNode->PosKeys.size() - 1; i++) {
		float t = (float)pAnimNode->PosKeys[i + 1].time;
		if (t > AnimTimeTicks) return i;
	}
	return 0;
}

uint SkeletalModel::FindRotIndex(float AnimTimeTicks, const Key* pAnimNode) {
	for (uint i = 0; i < pAnimNode->RotKeys.size() - 1; i++) {
		float t = (float)pAnimNode->RotKeys[i + 1].time;
		if (t > AnimTimeTicks) return i;
	}
	return 0;
}

uint SkeletalModel::FindScaleIndex(float AnimTimeTicks, const Key* pAnimNode) {
	for (uint i = 0; i < pAnimNode->ScalKeys.size() - 1; i++) {
		float t = (float)pAnimNode->ScalKeys[i + 1].time;
		if (t > AnimTimeTicks) return i;
	}
	return 0;
}

void SkeletalModel::Render() {
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