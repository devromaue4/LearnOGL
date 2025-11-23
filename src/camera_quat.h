#pragma once

#include <glm/glm.hpp>
//#include <glm/vec3.hpp> // glm::vec3
//#include <glm/vec4.hpp> // glm::vec4
//#include <glm/mat4x4.hpp> // glm::mat4
#include <glm/gtc/matrix_transform.hpp> 
#include <glm/gtc/type_ptr.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtx/vector_angle.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>

__forceinline
glm::vec3 rotate(const glm::vec3& v, float angle, const glm::vec3& axis) {
	return glm::angleAxis(glm::radians(angle), glm::normalize(glm::vec3(axis.x, axis.y, axis.z))) * v;
}

class CameraQuat {
private:
	glm::vec3 m_pos = glm::vec3(0, 0, 0);
	glm::vec3 m_target = glm::vec3(0, 0, 1);
	glm::vec3 m_up = glm::vec3(0, 1, 0);

	glm::mat4 m_mView = glm::mat4(1);
	glm::mat4 m_mProj = glm::mat4(1);

	float m_speed = 0.1f;
	float Sensitivity = 0.05f;

	int m_width = 0, m_height = 0;

	float mouseLastX = 0.0f;
	float mouseLastY = 0.0f;
	bool firstMouse = true;
	float Yaw = 0.0f, Pitch = 0.0f;

public:
	CameraQuat() {}
	CameraQuat(int w, int h,
		const glm::vec3& pos,
		const glm::vec3& target,
		const glm::vec3& up = glm::vec3(0, 1, 0));

	void setProj(float fAspect = 1.0f, float fovy = 45.0f, float zNear = 0.1f, float zFar = 1000.0f);

	const glm::vec3& GetPosition() const { return m_pos; }
	//glm::vec3 GetPosTarget() const { return m_pos + m_target; }
	//my::mat4 GetMatrix() { return my::lookAtRH(m_pos, m_pos + m_target, m_up); }
	const glm::mat4& getMat();
	const glm::mat4& getProj() { return m_mProj; }

	void SetPosition(float x, float y, float z) { m_pos.x = x; m_pos.y = y; m_pos.z = z; }
	void SetSpeedMove(float speed) { m_speed = speed; }

	void update();
	void OnMouse(float x, float y, bool constrainPitch = true);
	void OnKeyboard(unsigned int key/*, float deltaTime = 0*/);

	// left-handed z+ pint to the screen
	glm::mat4 GetMatrixLH();
	// right-handed z- pint to the screen
	glm::mat4 GetMatrixRH();
};