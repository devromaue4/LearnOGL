#pragma once
#include "core.h"

#include "mymath.h"

#include <fstream>
#include <string>

#include <iomanip>

//#ifdef _WIN32
//	#pragma warning( disable : 4005 ) // 'APIENTRY': macro redefinition
//	#include <windows.h> // for GetTickCount();
//#endif
//
//unsigned long long GetCurrentTimeMillis();

bool ReadFile(const char* fileName, std::string& outFile);

//std::string readFile(const char* filename);

//std::vector<char> readFileBin(const std::string& filename);

////////////////////// Utils //////////////////////

////////////////////// glm //////////////////////
FI glm::quat toGlm(const aiQuaternion& q) {
	return glm::quat(q.w, q.x, q.y, q.z);
}

FI glm::vec3 toGlm(const aiVector3D& v) {
	return glm::vec3(v.x, v.y, v.z);
}

FI glm::mat4 toGlm(const aiMatrix4x4& aiMat) {
	return glm::mat4(aiMat.a1, aiMat.b1, aiMat.c1, aiMat.d1,
		aiMat.a2, aiMat.b2, aiMat.c2, aiMat.d2,
		aiMat.a3, aiMat.b3, aiMat.c3, aiMat.d3,
		aiMat.a4, aiMat.b4, aiMat.c4, aiMat.d4);
}

FI void toGlm(glm::mat4& m, const aiMatrix4x4& aiMat) {
	m[0][0] = aiMat.a1; m[0][1] = aiMat.b1; m[0][2] = aiMat.c1; m[0][3] = aiMat.d1;
	m[1][0] = aiMat.a2; m[1][1] = aiMat.b2; m[1][2] = aiMat.c2; m[1][3] = aiMat.d2;
	m[2][0] = aiMat.a3; m[2][1] = aiMat.b3; m[2][2] = aiMat.c3; m[2][3] = aiMat.d3;
	m[3][0] = aiMat.a4; m[3][1] = aiMat.b4; m[3][2] = aiMat.c4; m[3][3] = aiMat.d4;
}

////////////////////// convert assimp math to mymath //////////////////////
FI my::quat toMy(const aiQuaternion& q) {
	return my::quat(q.w, q.x, q.y, q.z);
}

FI my::vec3 toMy(const aiVector3D& v) {
	return my::vec3(v.x, v.y, v.z);
}

FI my::mat4 toMy(const aiMatrix4x4& aiMat) {
	return my::mat4(aiMat.a1, aiMat.b1, aiMat.c1, aiMat.d1,
		aiMat.a2, aiMat.b2, aiMat.c2, aiMat.d2,
		aiMat.a3, aiMat.b3, aiMat.c3, aiMat.d3,
		aiMat.a4, aiMat.b4, aiMat.c4, aiMat.d4);
}

////////////////////// output //////////////////////
FI void print_mat4(const my::mat4& m) {
	std::cout << std::setw(10) << std::fixed << std::setprecision(4) << "\n";
	for (int i = 0; i < 4; i++)
		log(m.m[i][0] << " " << m.m[i][1] << " " << m.m[i][2] << " " << m.m[i][3]);
}

FI void print_mat4(const glm::mat4& m) {
	std::cout << std::setw(10) << std::fixed << std::setprecision(4) << "\n";
	for (int i = 0; i < 4; i++)
		log(m[i][0] << " " << m[i][1] << " " << m[i][2] << " " << m[i][3]);
}
