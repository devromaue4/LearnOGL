#include "mymath.h"

//#include <glm/glm.hpp>
//#include <glm/gtc/quaternion.hpp>
//#include <glm/gtx/quaternion.hpp>

// glm
//void my::vec3::rotate(float angle, const vec3& q) {
//	glm::quat qrot = glm::angleAxis(glm::radians(angle), glm::normalize(glm::vec3(q.x, q.y, q.z)));
//	glm::vec3 v = qrot * glm::vec3(this->x, this->y, this->z);
//	x = v.x; y = v.y; z = v.z;
//}

void my::vec3::rotate(float angle, const my::vec3& v) {
	my::quat qrot(my::radians(angle), my::normalize(v));
	qrot = qrot * (*this) * qrot.conjugate();
	x = qrot.x; y = qrot.y; z = qrot.z;
}

my::mat4::mat4( float m00, float m01, float m02, float m03,
				float m10, float m11, float m12, float m13,
				float m20, float m21, float m22, float m23,
				float m30, float m31, float m32, float m33) {
	// no need to transpose (auto transpose)
	//m[0][0] = m00; m[0][1] = m10; m[0][2] = m20; m[0][3] = m30;
	//m[1][0] = m01; m[1][1] = m11; m[1][2] = m21; m[1][3] = m31;
	//m[2][0] = m02; m[2][1] = m12; m[2][2] = m22; m[2][3] = m32;
	//m[3][0] = m03; m[3][1] = m13; m[3][2] = m23; m[3][3] = m33;
	// need to transpose
	m[0][0] = m00; m[0][1] = m01; m[0][2] = m02; m[0][3] = m03;
	m[1][0] = m10; m[1][1] = m11; m[1][2] = m12; m[1][3] = m13;
	m[2][0] = m20; m[2][1] = m21; m[2][2] = m22; m[2][3] = m23;
	m[3][0] = m30; m[3][1] = m31; m[3][2] = m32; m[3][3] = m33;
	//transpose();
}

my::mat4::mat4(const vec4& v0, const vec4& v1, const vec4& v2, const vec4& v3) {
	m[0][0] = v0.x; m[0][1] = v0.y; m[0][2] = v0.z; m[0][3] = v0.w;
	m[1][0] = v1.x; m[0][1] = v1.y; m[0][2] = v1.z; m[0][3] = v1.w;
	m[2][0] = v2.x; m[0][1] = v2.y; m[0][2] = v2.z; m[0][3] = v2.w;
	m[3][0] = v3.x; m[0][1] = v3.y; m[0][2] = v3.z; m[0][3] = v3.w;
}

INL my::quat my::quat::normalize() {
	float l = length();
	if (l <= 0.0f) return quat(1, 0, 0, 0);
	float oneOverL = 1.0f / l;
	w *= oneOverL; x *= oneOverL; y *= oneOverL; z *= oneOverL;
	return *this;
}

my::Matrix4f::Matrix4f(float a00, float a01, float a02, float a03,
	float a10, float a11, float a12, float a13,
	float a20, float a21, float a22, float a23,
	float a30, float a31, float a32, float a33) {
	m[0][0] = a00; m[0][1] = a01; m[0][2] = a02; m[0][3] = a03;
	m[1][0] = a10; m[1][1] = a11; m[1][2] = a12; m[1][3] = a13;
	m[2][0] = a20; m[2][1] = a21; m[2][2] = a22; m[2][3] = a23;
	m[3][0] = a30; m[3][1] = a31; m[3][2] = a32; m[3][3] = a33;
}

my::Matrix4f my::Matrix4f::operator*(const Matrix4f& Right) const {
	Matrix4f Ret;
	for (unsigned int i = 0; i < 4; i++) {
		for (unsigned int j = 0; j < 4; j++) {
			Ret.m[i][j] = m[i][0] * Right.m[0][j] +
				m[i][1] * Right.m[1][j] + m[i][2] * Right.m[2][j] + m[i][3] * Right.m[3][j];
		}
	}
	return Ret;
}

// right-handed
// This creates a symmetric frustum with horizontal FOV
/*INL Matrix4f perspectiveRHHF(float fovx, float aspect, float zNear, float zFar) {
	const float DegToRad = acos(-1.0f) / 180;
	float tangent = tan(fovx / 2.0f * DegToRad);
	float right = zNear * tangent;
	float top = right * aspect;

	Matrix4f mProj(
		(zNear / right), 0, 0, 0,
		0, (zNear / top), 0, 0,
		0, 0, -(zFar + zNear) / (zFar - zNear), -(2 * zFar * zNear) / (zFar - zNear),
		0, 0, -1, 0);
	return mProj;
}
INL Matrix4f orthoRH(float left, float right, float bottom, float top, float zNear, float zFar) {
	Matrix4f mProj(
		2 / (right - left), 0, 0, -(right + left) / (right - left),
		0, 2 / (top - bottom), 0, -(top + bottom) / (top - bottom),
		0, 0, -2 / (zFar - zNear), -(zFar + zNear) / (zFar - zNear),
		0, 0, 0, 1);
	return mProj;
}*/
