#pragma once

struct KeyPos {
	glm::vec3 position;
	double time;
	KeyPos(const glm::vec3& v, double t) : position(v), time(t) {}
};

struct KeyRot {
	glm::quat rotation;
	double time;
	KeyRot(const glm::quat& q, double t) : rotation(q), time(t) {}
};

struct KeyScal {
	glm::vec3 scale;
	double time;
	KeyScal(const glm::vec3& s, double t) : scale(s), time(t) {}
};

struct Key {
	std::string Name;
	std::vector<KeyPos> PosKeys;
	std::vector<KeyRot> RotKeys;
	std::vector<KeyScal> ScalKeys;
};

struct MyAnimation {
	std::string Name;
	double Duration;
	double TicksPerSecond;
	std::vector<Key> m_Keys;
};

struct AssimpNodeData {
	glm::mat4 transformation;
	std::string name;
	int childrenCount;
	std::vector<AssimpNodeData> children;
};

// for the SkeletalModel
__declspec(align(16))
struct Node {
	std::string Name;
	glm::mat4 InvBindTransform; // transform
	//----------------------
	int childrenCount;
	std::vector<Node> children;
};

struct NodeInfo {
	NodeInfo() {}
	NodeInfo(const aiNode* n) : pNode(n) {}
	const aiNode* pNode = nullptr;
	bool isRequired = false;
};

