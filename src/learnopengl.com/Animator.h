// from tutor learnopengl.com
#pragma once
#include "core.h"

struct KeyPosition {
	glm::vec3 position;
	float timeStamp;
};
struct KeyRotation {
	glm::quat orientation;
	float timeStamp;
};
struct KeyScale {
	glm::vec3 scale;
	float timeStamp;
};

struct Bone {
private:
	std::vector<KeyPosition>	m_Positions;
	std::vector<KeyRotation>	m_Rotations;
	std::vector<KeyScale>		m_Scales;
	int m_NumPositions, m_NumRotations, m_NumScalings;

	glm::mat4 m_LocalTransform;
	std::string m_Name;
	int m_ID;

public:
	Bone(const std::string& name, int ID, const aiNodeAnim* channel)
		: m_Name(name),
		m_ID(ID),
		m_LocalTransform(1.0f) {
		m_NumPositions = channel->mNumPositionKeys;

		for (int posIndex = 0; posIndex < m_NumPositions; ++posIndex) {
			aiVector3D aiPos = channel->mPositionKeys[posIndex].mValue;
			float timeStamp = (float)channel->mPositionKeys[posIndex].mTime;
			KeyPosition data;
			data.position = glm::vec3(aiPos.x, aiPos.y, aiPos.z);
			data.timeStamp = timeStamp;
			m_Positions.push_back(data);
		}

		m_NumRotations = channel->mNumRotationKeys;
		for (int rotIndex = 0; rotIndex < m_NumRotations; ++rotIndex) {
			aiQuaternion aiQrient = channel->mRotationKeys[rotIndex].mValue;
			float timeStamp = (float)channel->mRotationKeys[rotIndex].mTime;
			KeyRotation data;
			data.orientation = glm::quat(aiQrient.w, aiQrient.x, aiQrient.y, aiQrient.z);
			data.timeStamp = timeStamp;
			m_Rotations.push_back(data);
		}

		m_NumScalings = channel->mNumScalingKeys;
		for (int scalIndex = 0; scalIndex < m_NumScalings; ++scalIndex) {
			aiVector3D aiScale = channel->mScalingKeys[scalIndex].mValue;
			float timeStamp = (float)channel->mScalingKeys[scalIndex].mTime;
			KeyScale data;
			data.scale = glm::vec3(aiScale.x, aiScale.y, aiScale.z);
			data.timeStamp = timeStamp;
			m_Scales.push_back(data);
		}
	}

	/*
		interpolates  b/w positions,rotations & scaling keys based on the curren time of
		the animation and prepares the local transformation matrix by combining all keys
		tranformations
	*/
	void Update(float animationTime) {
		glm::mat4 translation = InterpolatePosition(animationTime);
		glm::mat4 rotation = InterpolateRotation(animationTime);
		glm::mat4 scale = InterpolateScaling(animationTime);
		m_LocalTransform = translation * rotation * scale;
	}

	//glm::mat4 GetLocalTransform() const { return m_LocalTransform; }
	const glm::mat4& GetLocalTransform() const { return m_LocalTransform; }
	const std::string& GetBoneName() const { return m_Name; }
	int GetBoneID() const { return m_ID; }

	/* 
		Gets the current index on mKeyPositions to interpolate to based on
		the current animation time
	*/
	int GetPositionIndex(float animationTime) const {
		for (int Index = 0; Index < m_NumPositions - 1; ++Index)
			if (animationTime < m_Positions[Index + 1].timeStamp) return Index;	
		assert(0);
		return -1;
	}
	int GetRotationIndex(float animationTime) const {
		for (int Index = 0; Index < m_NumRotations - 1; ++Index) 
			if (animationTime < m_Rotations[Index + 1].timeStamp) return Index;
		assert(0);
		return -1;
	}
	int GetScaleIndex(float animationTime) const {
		for (int Index = 0; Index < m_NumScalings - 1; ++Index) 
			if (animationTime < m_Scales[Index + 1].timeStamp) return Index;
		assert(0);
		return -1;
	}

private:
	// Gets normalized value for Lerp & Slerp
	float GetScaleFactor(float lastTimeStamp, float nextTimeStamp, float animationTime) const {
		float scaleFactor = 0.0f;
		float midWayLength = animationTime - lastTimeStamp;
		float frameDiff = nextTimeStamp - lastTimeStamp;
		scaleFactor = midWayLength / frameDiff;
		return scaleFactor;
	}

	/*
		figures out which position keys to interpolate b/w and performs the interpolation
		and returns the translation matrix
	*/
	glm::mat4 InterpolatePosition(float animationTime) {
		if (m_NumPositions == 1) return glm::translate(glm::mat4(1.0f), m_Positions[0].position);

		int p0Index = GetPositionIndex(animationTime);
		int p1Index = p0Index + 1;
		float scaleFactor = GetScaleFactor(m_Positions[p0Index].timeStamp, m_Positions[p1Index].timeStamp, animationTime);
		glm::vec3 finalPosition = glm::mix(m_Positions[p0Index].position, m_Positions[p1Index].position, scaleFactor);

		return glm::translate(glm::mat4(1.0f), finalPosition);
	}

	glm::mat4 InterpolateRotation(float animationTime) {
		if (m_NumRotations == 1) {
			auto rotation = glm::normalize(m_Rotations[0].orientation);
			return glm::toMat4(rotation);
		}

		int p0Index = GetRotationIndex(animationTime);
		int p1Index = p0Index + 1;
		float scaleFactor = GetScaleFactor(m_Rotations[p0Index].timeStamp, m_Rotations[p1Index].timeStamp, animationTime);
		glm::quat finalRotation = glm::slerp(m_Rotations[p0Index].orientation, m_Rotations[p1Index].orientation, scaleFactor);
		finalRotation = glm::normalize(finalRotation);
		return glm::toMat4(finalRotation);
	}

	glm::mat4 InterpolateScaling(float animationTime) {
		if (m_NumScalings == 1) return glm::scale(glm::mat4(1.0f), m_Scales[0].scale);

		int p0Index = GetScaleIndex(animationTime);
		int p1Index = p0Index + 1;
		float scaleFactor = GetScaleFactor(m_Scales[p0Index].timeStamp, m_Scales[p1Index].timeStamp, animationTime);
		glm::vec3 finalScale = glm::mix(m_Scales[p0Index].scale, m_Scales[p1Index].scale, scaleFactor);

		return glm::scale(glm::mat4(1.0f), finalScale);
	}
};

struct AssimpNodeData {
	glm::mat4 transformation;
	std::string name;
	int childrenCount;
	std::vector<AssimpNodeData> children;
};

class Animation {
	float m_Duration;
	//int m_TicksPerSeconds;
	float m_TicksPerSeconds;
	std::vector<Bone> m_Bones;
	AssimpNodeData m_RootNode;
	std::map<std::string, BoneInfo> m_BoneInfoMap;

public:
	glm::mat4 m_GlobalInverseTransform;

	Animation() = default;
	Animation(const std::string& animPath, Model* model) {
		Assimp::Importer importer;
		const aiScene* scene = importer.ReadFile(animPath, aiProcess_Triangulate);
		assert(scene && scene->mRootNode);
		auto animation = scene->mAnimations[0];
		m_Duration = (float)animation->mDuration;
		m_TicksPerSeconds = (float)animation->mTicksPerSecond;

		m_GlobalInverseTransform = toGlm(scene->mRootNode->mTransformation);
		m_GlobalInverseTransform = glm::inverse(m_GlobalInverseTransform);

		ReadHeirarchyData(m_RootNode, scene->mRootNode);
		ReadMissingBones(animation, *model);
	}
	~Animation() {}

	Bone* FindBone(const std::string& name) {
		auto iter = std::find_if(m_Bones.begin(), m_Bones.end(), [&](const Bone& Bone) { return Bone.GetBoneName() == name; });
		if (iter == m_Bones.end()) return nullptr;
		else return &(*iter);
	}

	float GetTicksPerSecons() { return m_TicksPerSeconds; }
	float GetDuration() { return m_Duration; }
	const AssimpNodeData& GetRootNode() { return m_RootNode; }
	const std::map<std::string, BoneInfo>& GetBoneIDMap() { return m_BoneInfoMap; }

private:
	void ReadMissingBones(const aiAnimation* animation, Model& model) {
		int size = animation->mNumChannels;

		auto& boneInfoMap = model.GetBoneInfoMap();
		int& boneCount = model.GetBoneCount();

		for (int i = 0; i < size; i++) {
			auto channel = animation->mChannels[i];
			std::string boneName = channel->mNodeName.data;
			if (boneInfoMap.find(boneName) == boneInfoMap.end()) {
				boneInfoMap[boneName].id = boneCount;
				boneCount++;
			}
			m_Bones.push_back(Bone(channel->mNodeName.data, boneInfoMap[channel->mNodeName.data].id, channel));
		}
		m_BoneInfoMap = boneInfoMap;
	}

	void ReadHeirarchyData(AssimpNodeData& dest, const aiNode* src) {
		assert(src);

		dest.name = src->mName.data;
		dest.transformation = toGlm(src->mTransformation);
		dest.childrenCount = src->mNumChildren;

		for (uint i = 0; i < src->mNumChildren; i++) {
			AssimpNodeData newData;
			ReadHeirarchyData(newData, src->mChildren[i]);
			dest.children.push_back(newData);
		}
	}
};

class Animator {
	std::vector<glm::mat4> m_FinalBoneMatrices;
	Animation* m_CurrentAnimation;
	float m_CurrentTime, m_DeltaTime;

public:
	Animator(Animation* pAnimation) {
		m_CurrentTime = 0.0f;
		m_CurrentAnimation = pAnimation;

		m_FinalBoneMatrices.reserve(100);

		for (int i = 0; i < 100; i++)
			m_FinalBoneMatrices.push_back(glm::mat4(1.0f));
	}

	void UpdateAnimation(float dt) {
		m_DeltaTime = dt;
		if (m_CurrentAnimation) {
			m_CurrentTime += m_CurrentAnimation->GetTicksPerSecons() * dt;
			m_CurrentTime = fmod(m_CurrentTime, m_CurrentAnimation->GetDuration());
			CalculateBoneTransform(&m_CurrentAnimation->GetRootNode(), glm::mat4(1.0f));
		}
	}

	void PlayAnimation(Animation* pAnimation) {
		m_CurrentAnimation = pAnimation;
		m_CurrentTime = 0.0f;
	}

	void CalculateBoneTransform(const AssimpNodeData* node, const glm::mat4& parentTransform) {
		const std::string& nodeName = node->name;
		glm::mat4 nodeTransform = node->transformation;

		Bone* pBone = m_CurrentAnimation->FindBone(nodeName);
		if (pBone) {
			pBone->Update(m_CurrentTime);
			nodeTransform = pBone->GetLocalTransform();
		}

		glm::mat4 globalTransformation = parentTransform * nodeTransform;

		auto boneInfoMap = m_CurrentAnimation->GetBoneIDMap();
		if (boneInfoMap.find(nodeName) != boneInfoMap.end()) {
			int index = boneInfoMap[nodeName].id;
			glm::mat4 offset = boneInfoMap[nodeName].offset;

			m_FinalBoneMatrices[index] = m_CurrentAnimation->m_GlobalInverseTransform * globalTransformation * offset;
		}

		for (int i = 0; i < node->childrenCount; i++)
			CalculateBoneTransform(&node->children[i], globalTransformation);
	}

	//std::vector<glm::mat4> GetFinalBoneMatrices() { return m_FinalBoneMatrices; }
	//const std::vector<glm::mat4>& GetFinalBoneMatrices() { return m_FinalBoneMatrices; }
	std::vector<glm::mat4>* GetFinalBoneMatrices() { return &m_FinalBoneMatrices; }
};