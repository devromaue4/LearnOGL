#include "skinned_mesh.h"
#include "myutil.h"

#define SCOLOR_TEXTURE_UNIT			GL_TEXTURE0
#define SCOLOR_TEXTURE_UNIT_INDEX	0
//#define SHADOW_TEXTURE_UNIT			GL_TEXTURE1
//#define SHADOW_TEXTURE_UNIT_INDEX	0
//#define NORMAL_TEXTURE_UNIT			GL_TEXTURE2
//#define NORMAL_TEXTURE_UNIT_INDEX	0

const int SPOSITION_LOCATION	= 0;
const int SNORMAL_LOCATION		= 1;
const int STEXT_COORD_LOCATION	= 2;
const int SBONE_ID_LOCATION		= 3;
const int SBONE_WEIGHT_LOCATION = 4;

SkinnedMesh::~SkinnedMesh() { Clear(); }

void SkinnedMesh::Clear() {
	if (m_Buffers[0] != 0)
		glDeleteBuffers(array_num_elems(m_Buffers), m_Buffers);

	if (m_VAO != 0) {
		glDeleteVertexArrays(1, &m_VAO);
		m_VAO = 0;
	}
}

bool SkinnedMesh::LoadMesh(const std::string& fileName) {
	Clear();

	glGenVertexArrays(1, &m_VAO);
	glBindVertexArray(m_VAO);

	glGenBuffers(array_num_elems(m_Buffers), m_Buffers);

	bool Ret = false;
	m_pScene = m_Importer.ReadFile(fileName.c_str(), SASSIMP_LOAD_FLAGS);
	if (m_pScene) {
		m_GlobalInverseTransform = glm::inverse(toGlm(m_pScene->mRootNode->mTransformation));
		Ret = InitFromScene(m_pScene, fileName);
	}
	else {
		log("Error parsing: " << fileName << " : " << m_Importer.GetErrorString());
		throw std::exception("failed to LoadMesh!");
	}

	glBindVertexArray(0);

	return Ret;
}

bool SkinnedMesh::InitFromScene(const aiScene* pScene, const std::string& fileName) {
	m_Meshes.resize(pScene->mNumMeshes);
	m_Materials.resize(pScene->mNumMaterials);

	uint NumVertices = 0, NumIndices = 0;
	CountVerticesAndIndices(pScene, NumVertices, NumIndices);
	ReserveSpace(NumVertices, NumIndices);
	InitAllMeshes(pScene);

	if (!InitMaterials(pScene, fileName)) return false;

	PopulateBuffers();

	return true;
}

void SkinnedMesh::CountVerticesAndIndices(const aiScene* pScene, uint& NumVertices, uint& NumIndices) {
	for (uint i = 0; i < m_Meshes.size(); i++) {
		m_Meshes[i].MaterialIndex = pScene->mMeshes[i]->mMaterialIndex;
		m_Meshes[i].NumIndices = pScene->mMeshes[i]->mNumFaces * 3;
		m_Meshes[i].BaseVertex = NumVertices;
		m_Meshes[i].BaseIndex = NumIndices;

		NumVertices += pScene->mMeshes[i]->mNumVertices;
		NumIndices += m_Meshes[i].NumIndices;
	}
}

void SkinnedMesh::ReserveSpace(uint NumVertices, uint NimIndices) {
	m_Positons.reserve(NumVertices);
	m_Normals.reserve(NumVertices);
	m_TexCoords.reserve(NumVertices);
	m_Indices.reserve(NimIndices);
	m_Bones.resize(NumVertices);
}

void SkinnedMesh::InitAllMeshes(const aiScene* pScene) {
	for (uint i = 0; i < m_Meshes.size(); i++) {
		const aiMesh* paiMesh = pScene->mMeshes[i];
		InitSingleMesh(i, paiMesh);
	}
}

void SkinnedMesh::InitSingleMesh(uint MeshIndex, const aiMesh* paiMesh) {
	const aiVector3D Zero3D(.0f, .0f, .0f);

	for (uint i = 0; i < paiMesh->mNumVertices; i++) {
		const aiVector3D& pPos = paiMesh->mVertices[i];
		m_Positons.push_back(glm::vec3(pPos.x, pPos.y, pPos.z));

		if (paiMesh->mNormals) {
			const aiVector3D& pNormal = paiMesh->mNormals[i];
			m_Normals.push_back(glm::vec3(pNormal.x, pNormal.y, pNormal.z));
		}
		else {
			aiVector3D Normal(.0f, 1.f, .0f);
			m_Normals.push_back(glm::vec3(Normal.x, Normal.y, Normal.z));
		}

		const aiVector3D& pTexCoord = paiMesh->HasTextureCoords(0) ? paiMesh->mTextureCoords[0][i] : Zero3D;
		m_TexCoords.push_back(glm::vec2(pTexCoord.x, pTexCoord.y));
	}

	LoadMeshBones(MeshIndex, paiMesh);

	for (uint i = 0; i < paiMesh->mNumFaces; i++) {
		const aiFace& Face = paiMesh->mFaces[i];
		assert(Face.mNumIndices == 3);
		m_Indices.push_back(Face.mIndices[0]);
		m_Indices.push_back(Face.mIndices[1]);
		m_Indices.push_back(Face.mIndices[2]);
	}
}

void SkinnedMesh::PopulateBuffers() {
	glBindBuffer(GL_ARRAY_BUFFER, m_Buffers[POS_VB]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(m_Positons[0]) * m_Positons.size(), &m_Positons[0], GL_STATIC_DRAW);
	glEnableVertexAttribArray(SPOSITION_LOCATION);
	glVertexAttribPointer(SPOSITION_LOCATION, 3, GL_FLOAT, GL_FALSE, 0, 0);

	glBindBuffer(GL_ARRAY_BUFFER, m_Buffers[NORMAL_VB]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(m_Normals[0]) * m_Normals.size(), &m_Normals[0], GL_STATIC_DRAW);
	glEnableVertexAttribArray(SNORMAL_LOCATION);
	glVertexAttribPointer(SNORMAL_LOCATION, 3, GL_FLOAT, GL_FALSE, 0, 0);

	glBindBuffer(GL_ARRAY_BUFFER, m_Buffers[TEXCOORD_VB]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(m_TexCoords[0]) * m_TexCoords.size(), &m_TexCoords[0], GL_STATIC_DRAW);
	glEnableVertexAttribArray(STEXT_COORD_LOCATION);
	glVertexAttribPointer(STEXT_COORD_LOCATION, 2, GL_FLOAT, GL_FALSE, 0, 0);

	// bones
	glBindBuffer(GL_ARRAY_BUFFER, m_Buffers[BONE_VB]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(m_Bones[0]) * m_Bones.size(), &m_Bones[0], GL_STATIC_DRAW);
	glEnableVertexAttribArray(SBONE_ID_LOCATION);
	glVertexAttribIPointer(SBONE_ID_LOCATION, SMAX_NUM_BONES_PER_VERTEX, GL_INT, sizeof(VertexBoneData), (const GLvoid*)0);
	glEnableVertexAttribArray(SBONE_WEIGHT_LOCATION);
	glVertexAttribPointer(SBONE_WEIGHT_LOCATION, SMAX_NUM_BONES_PER_VERTEX, GL_FLOAT, GL_FALSE, sizeof(VertexBoneData), (const GLvoid*)(SMAX_NUM_BONES_PER_VERTEX * sizeof(int32_t)));

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_Buffers[INDEX_BUFFER]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(m_Indices[0]) * m_Indices.size(), &m_Indices[0], GL_STATIC_DRAW);
}

void SkinnedMesh::Render() {
	glBindVertexArray(m_VAO);

	for (uint i = 0; i < m_Meshes.size(); i++) {
		uint MaterialIndex = m_Meshes[i].MaterialIndex;
		assert(MaterialIndex < m_Materials.size());

		if (m_Materials[MaterialIndex]) m_Materials[MaterialIndex]->Bind(SCOLOR_TEXTURE_UNIT);
		//if (m_Materials[MaterialIndex].pTextures[TEX__TYPE_SPECULAR]) m_Materials[MaterialIndex].pTextures[TEX__TYPE_SPECULAR]->Bind(SPECULAR_EXPONENT_UNIT);

		glDrawElementsBaseVertex(GL_TRIANGLES, m_Meshes[i].NumIndices, GL_UNSIGNED_INT, (void*)(sizeof(uint) * m_Meshes[i].BaseIndex), m_Meshes[i].BaseVertex);
	}

	glBindVertexArray(0);
}

bool SkinnedMesh::InitMaterials(const aiScene* pScene, const std::string& fileName) {
	std::string::size_type SlashIndex = fileName.find_last_of("/");
	std::string Dir;
	if (SlashIndex == std::string::npos)
		Dir = ".";
	else if (SlashIndex == 0)
		Dir = "/";
	else
		Dir = fileName.substr(0, SlashIndex);

	bool Ret = true;
	for (uint i = 0; i < pScene->mNumMaterials; i++) {
		const aiMaterial* pMaterial = pScene->mMaterials[i];
		LoadDiffuseTexture(Dir, pMaterial, i);
	}

	return Ret;
}

//void SkinnedMesh::LoadTextures(const std::string& Dir, const aiMaterial* pMaterial, int index) {
//}

void SkinnedMesh::LoadDiffuseTexture(const std::string& Dir, const aiMaterial* pMaterial, int index) {
	m_Materials[index] = nullptr;

	if (pMaterial->GetTextureCount(aiTextureType_DIFFUSE) > 0) {

		aiString Path;
		if (pMaterial->GetTexture(aiTextureType_DIFFUSE, 0, &Path) == AI_SUCCESS) {

			const aiTexture* paiTexture = m_pScene->GetEmbeddedTexture(Path.C_Str());
			if (paiTexture) {
				//log("Embedded diffuse texutre type " << paiTexture->achFormatHint);
				m_Materials[index] = new Texture;
				if (!m_Materials[index]->Load(paiTexture->mWidth, paiTexture->pcData))
					log("Error loading Embedded texture");
			}
			else {
				std::string p(Path.data);
				if (p.substr(0, 2) == ".\\")
					p = p.substr(2, p.size() - 2);

				//int sz = p.find_last_of('/') + 1;
				//p = p.substr(sz, sz);
			
				std::string FullPath = Dir + "/" + p;
				m_Materials[index] = new Texture(FullPath.c_str());
				if (!m_Materials[index]->Load()) {
					log("Error loading texture " << FullPath.c_str());
					safe_delete(m_Materials[index]);
					m_Materials[index] = nullptr;
					return;
				}
			}
		}
	}
}

//void SkinnedMesh::LoadSpecularTexture(const std::string& Dir, const aiMaterial* pMaterial, int index) {
//}
//
//void SkinnedMesh::LoadColors(const aiMaterial* pMaterial, int index) {
//}

void SkinnedMesh::LoadMeshBones(uint MeshIndex, const aiMesh* pMesh) {
	for (uint i = 0; i < pMesh->mNumBones; i++)
		LoadSingleBone(MeshIndex, pMesh->mBones[i]);
}

void SkinnedMesh::LoadSingleBone(uint MeshIndex, const aiBone* pBone) {
	int boneId = GetBoneId(pBone);

	if (boneId == m_BoneInfo.size()) {
		BoneInfo bi(pBone->mOffsetMatrix);
		m_BoneInfo.push_back(bi);
	}

	for (uint i = 0; i < pBone->mNumWeights; i++) {
		const aiVertexWeight& vw = pBone->mWeights[i];
		uint globalVertexID = m_Meshes[MeshIndex].BaseVertex + pBone->mWeights[i].mVertexId;
		m_Bones[globalVertexID].AddBoneData(boneId, vw.mWeight);
	}

}

int SkinnedMesh::GetBoneId(const aiBone* pBone) {
	int boneIndex = 0;
	std::string BoneName(pBone->mName.C_Str());

	if (m_BoneNameToIndexMap.find(BoneName) == m_BoneNameToIndexMap.end()) {
		// allocate an index for new bone
		boneIndex = (int)m_BoneNameToIndexMap.size();
		m_BoneNameToIndexMap[BoneName] = boneIndex;
	}
	else boneIndex = m_BoneNameToIndexMap[BoneName];

	return boneIndex;
}

void SkinnedMesh::GetBoneTransforms(float TimeInSeconds, std::vector<glm::mat4>& Transforms, int AnimIndex) {

	glm::mat4 Identity(1);

	float TickPerSeconds = (float)(m_pScene->mAnimations[AnimIndex]->mTicksPerSecond != 0 ? m_pScene->mAnimations[AnimIndex]->mTicksPerSecond : 25.0f);
	float TimeInTicks = TimeInSeconds * TickPerSeconds;
	float AnimationTimeTicks = fmod(TimeInTicks, (float)m_pScene->mAnimations[AnimIndex]->mDuration);

	ReadNodeHierarchy(AnimationTimeTicks, m_pScene->mRootNode, Identity, AnimIndex);
	Transforms.resize(m_BoneInfo.size());

	for (uint i = 0; i < m_BoneInfo.size(); i++)
		Transforms[i] = m_BoneInfo[i].FinalTransformation;
}

void SkinnedMesh::ReadNodeHierarchy(float AnimationTimeTicks, const aiNode* pNode, const glm::mat4& ParentTransform, int AnimIndex) {
	std::string NodeName(pNode->mName.data);

	const aiAnimation* pAnimation = m_pScene->mAnimations[AnimIndex];

	glm::mat4 NodeTransformation;
	memcpy(&NodeTransformation[0], &pNode->mTransformation, sizeof(float) * 16);
	NodeTransformation = glm::transpose(NodeTransformation);

	const aiNodeAnim* pNodeAnim = FindNodeAnim(pAnimation, NodeName);

	if (pNodeAnim) {
		// Interpolate scaling and generate scaling transformation matrix
		aiVector3D Scaling;
		CalcInterpolatedScaling(Scaling, AnimationTimeTicks, pNodeAnim);
		glm::mat4 ScalingM(1);
		ScalingM = glm::scale(ScalingM, glm::vec3(Scaling.x, Scaling.y, Scaling.z));
		//ScalingM = glm::transpose(ScalingM);
		
		// Interpolate rotation and generate rotation transformation matrix
		aiQuaternion RotationQ;
		CalcInterpolatedRotation(RotationQ, AnimationTimeTicks, pNodeAnim);
		aiMatrix3x3 mRot = RotationQ.GetMatrix();
		glm::mat3 m(1);
		glm::mat4 RotationM(1);
		memcpy(&m[0], &mRot, sizeof(float) * 9);
		RotationM = m;
		RotationM = glm::transpose(RotationM);

		// Interpolate translation and generate translation transformation matrix
		aiVector3D Translation;
		CalcInterpolatedPosition(Translation, AnimationTimeTicks, pNodeAnim);
		glm::mat4 TranslationM(1);
		TranslationM = glm::translate(TranslationM, glm::vec3(Translation.x, Translation.y, Translation.z));
		//TranslationM = glm::transpose(TranslationM);

		// Combine the above transformation
		NodeTransformation = TranslationM * RotationM * ScalingM;
	}

	glm::mat4 GlobalTransformation = ParentTransform * NodeTransformation;

	if (m_BoneNameToIndexMap.find(NodeName) != m_BoneNameToIndexMap.end()) {
		uint BoneIndex = m_BoneNameToIndexMap[NodeName];
		m_BoneInfo[BoneIndex].FinalTransformation = m_GlobalInverseTransform * GlobalTransformation * m_BoneInfo[BoneIndex].OffsetMatrix;
	}

	for (uint i = 0; i < pNode->mNumChildren; i++)
		ReadNodeHierarchy(AnimationTimeTicks, pNode->mChildren[i], GlobalTransformation, AnimIndex);
}

const aiNodeAnim* SkinnedMesh::FindNodeAnim(const aiAnimation* pAnimation, const std::string NodeName) {
	for (uint i = 0; i < pAnimation->mNumChannels; i++) {
		const aiNodeAnim* pNodeAnim = pAnimation->mChannels[i];

		if (std::string(pNodeAnim->mNodeName.data) == NodeName)
			return pNodeAnim;
	}

	return nullptr;
}

uint SkinnedMesh::FindScaling(float AnimationTimeTicks, const aiNodeAnim* pNodeAnim) {
	assert(pNodeAnim->mNumScalingKeys > 0);

	for (uint i = 0; i < pNodeAnim->mNumScalingKeys - 1; i++) {
		float t = (float)pNodeAnim->mScalingKeys[i + 1].mTime;
		if (AnimationTimeTicks < t)
			return i;
	}

	return 0;
}

uint SkinnedMesh::FindRotation(float AnimationTimeTicks, const aiNodeAnim* pNodeAnim) {
	assert(pNodeAnim->mNumRotationKeys > 0);

	for (uint i = 0; i < pNodeAnim->mNumRotationKeys - 1; i++) {
		float t = (float)pNodeAnim->mRotationKeys[i + 1].mTime;
		if (AnimationTimeTicks < t)
			return i;
	}

	return 0;
}

uint SkinnedMesh::FindPosition(float AnimationTimeTicks, const aiNodeAnim* pNodeAnim) {
	for (uint i = 0; i < pNodeAnim->mNumPositionKeys - 1; i++) {
		float t = (float)pNodeAnim->mPositionKeys[i + 1].mTime;
		if (AnimationTimeTicks < t)
			return i;
	}

	return 0;
}

void SkinnedMesh::CalcInterpolatedScaling(aiVector3D& scaling, float AnimationTimeTicks, const aiNodeAnim* pNodeAnim) {
	// we need at least two values to interpolate
	if (pNodeAnim->mNumScalingKeys == 1) {
		scaling = pNodeAnim->mScalingKeys[0].mValue;
		return;
	}

	uint ScalingIndex = FindScaling(AnimationTimeTicks, pNodeAnim);
	uint NextScalingIndex = ScalingIndex + 1;
	assert(NextScalingIndex < pNodeAnim->mNumScalingKeys);
	float t1 = (float)pNodeAnim->mScalingKeys[ScalingIndex].mTime;
	float t2 = (float)pNodeAnim->mScalingKeys[NextScalingIndex].mTime;
	float deltaTime = t2 - t1;
	float Factor = (AnimationTimeTicks - t1) / deltaTime;
	assert(Factor >= 0.0f && Factor <= 1.0f);
	const aiVector3D& Start = pNodeAnim->mScalingKeys[ScalingIndex].mValue;
	const aiVector3D& End = pNodeAnim->mScalingKeys[NextScalingIndex].mValue;
	aiVector3D Delta = End - Start;
	scaling = Start + Factor * Delta;
}

void SkinnedMesh::CalcInterpolatedRotation(aiQuaternion& rotation, float AnimationTimeTicks, const aiNodeAnim* pNodeAnim) {
	// we need at least two values to interpolate
	if (pNodeAnim->mNumRotationKeys == 1) {
		rotation = pNodeAnim->mRotationKeys[0].mValue;
		return;
	}

	uint RotationIndex = FindRotation(AnimationTimeTicks, pNodeAnim);
	uint NextRotationIndex = RotationIndex + 1;
	assert(NextRotationIndex < pNodeAnim->mNumRotationKeys);
	float t1 = (float)pNodeAnim->mRotationKeys[RotationIndex].mTime;
	float t2 = (float)pNodeAnim->mRotationKeys[NextRotationIndex].mTime;
	float DeltaTime = t2 - t1;
	float Factor = (AnimationTimeTicks - t1) / DeltaTime;
	assert(Factor >= 0.f && Factor <= 1.0f);
	const aiQuaternion StartRotationQ = pNodeAnim->mRotationKeys[RotationIndex].mValue;
	const aiQuaternion EndRotationQ = pNodeAnim->mRotationKeys[NextRotationIndex].mValue;
	aiQuaternion::Interpolate(rotation, StartRotationQ, EndRotationQ, Factor);
	//rotation = StartRotationQ; // this is a bug (stay here for fun)
	rotation.Normalize();
}

void SkinnedMesh::CalcInterpolatedPosition(aiVector3D& translation, float AnimationTimeTicks, const aiNodeAnim* pNodeAnim) {
	// we need at least two values to interpolate
	if (pNodeAnim->mNumPositionKeys == 1) {
		translation = pNodeAnim->mPositionKeys[0].mValue;
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
	const aiVector3D& Start = pNodeAnim->mPositionKeys[PositionIndex].mValue;
	const aiVector3D& End = pNodeAnim->mPositionKeys[NextPositionIndex].mValue;
	aiVector3D Delta = End - Start;
	translation = Start + Factor * Delta;
}