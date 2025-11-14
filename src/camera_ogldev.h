#pragma once

//#include "mymath.h"

__forceinline
glm::vec3 rotate(const glm::vec3& v, float angle, const glm::vec3& axis) {
	return glm::angleAxis(glm::radians(angle), glm::normalize(glm::vec3(axis.x, axis.y, axis.z))) * v;
}

class CameraOGLDEV {
private:
	glm::vec3 m_pos = glm::vec3(0, 0, 0);
	glm::vec3 m_target = glm::vec3(0, 0, 1);
	glm::vec3 m_up = glm::vec3(0, 1, 0);

	glm::mat4 m_mView = glm::mat4(1);
	glm::mat4 m_mProj = glm::mat4(1);

	float m_speed = 0.1f;
	float Sensitivity = 0.05f;

	int m_width, m_height;

	float mouseLastX = 0.0f;
	float mouseLastY = 0.0f;
	bool firstMouse = true;
	float Yaw = 0.0f, Pitch = 0.0f;

public:
	CameraOGLDEV() {}
	CameraOGLDEV(int w, int h,
		const glm::vec3& pos,
		const glm::vec3& target,
		const glm::vec3& up = glm::vec3(0, 1, 0))
		: m_pos(pos), m_target(target), m_up(up), m_width(w), m_height(h), Yaw(0.0f), Pitch(0.0f) {
		m_target = glm::normalize(m_target);
		m_up = glm::normalize(m_up);
		mouseLastX = (float)w / 2;
		mouseLastY = (float)h / 2;

		m_mProj = glm::perspective(glm::radians(45.0f), (float)w / h, .1f, 1000.0f);
	}

	void SetProjParams(float fAspect = 1.0f, float fovy = 45.0f, float zNear = 0.1f, float zFar = 1000.0f) {
		m_mProj = glm::perspective(glm::radians(fovy), fAspect, zNear, zFar);
	}

	const glm::vec3& GetPosition() const { return m_pos; }
	//glm::vec3 GetPosTarget() const { return m_pos + m_target; }

	void SetPosition(float x, float y, float z) { m_pos.x = x; m_pos.y = y; m_pos.z = z; }
	void SetSpeedMove(float speed) { m_speed = speed; }

	void OnMouse(float x, float y, bool constrainPitch = true) {
		float xpos = x, ypos = y;

		if (firstMouse) { // prevent jump mouse when first time capture the cursor
			mouseLastX = xpos;
			mouseLastY = ypos;
			firstMouse = false;
		}

		float xOffset = mouseLastX - xpos;
		float yOffset = ypos - mouseLastY; // reversed since y-coordinates range from bottom to top

		mouseLastX = xpos;
		mouseLastY = ypos;

		xOffset *= Sensitivity;
		yOffset *= Sensitivity;

		Yaw += xOffset;
		Pitch += yOffset;

		if (abs(Yaw) >= 360.0f) Yaw = 0.0f;
		if (abs(Yaw) <= -360.0f) Yaw = 0.0f;
		if (constrainPitch) {
			if (Pitch > 89.0f) Pitch = 89.0f;
			if (Pitch < -89.0f) Pitch = -89.0f;
		}

		update();
	}

	void update() {
		glm::vec3 Yaxis(0, 1, 0);

		// rotate the view vector by the horizontal angle around the vertical axis
		//my::vec3 view(1, 0, 0);
		glm::vec3 view(0, 0, -1);
		//view.rotate(Yaw, Yaxis);  // for my math
		view = rotate(view, Yaw, Yaxis);

		view = glm::normalize(view);

		// rotate the view vector by the vertical angle around the horizontal axis
		glm::vec3 U = glm::cross(Yaxis, view);
		U = glm::normalize(U);
		//view.rotate(Pitch, U); // for my math
		view = rotate(view, Pitch, U);

		m_target = view;
		m_target = glm::normalize(m_target);

		m_up = glm::cross(m_target, U);
		m_up = glm::normalize(m_up);
	}

	void OnKeyboard(unsigned int key/*, float deltaTime = 0*/) {
		switch (key) {
		case 61: //plus
			m_speed += 0.01f;
			//std::cout << "Speed changed to " << m_speed << std::endl;
			break;
		case 45: { //minus
			m_speed -= 0.01f;
			if (m_speed < 0.1f) m_speed = 0.1f;
			//std::cout << "Speed changed to " << m_speed << std::endl;
			}break;
		case 266: //page_up
			m_pos.y = m_pos.y + m_speed;
			break;
		case 267: //page_down
			m_pos.y = m_pos.y - m_speed;
			break;
		case 87: //forward
			m_pos = m_pos + (m_target * m_speed);
			break;
		case 83: //backward
			m_pos = m_pos - (m_target * m_speed);
			break;
		case 65: { //left
			glm::vec3 left = glm::cross(m_target, m_up);
			left = glm::normalize(left); // create vector point to the left
				m_pos = m_pos - (left * m_speed);
			} break;
		case 68: { //right
			glm::vec3 right = glm::cross(m_up, m_target);
			right = glm::normalize(right); // create vector point to the right
				m_pos = m_pos - (right * m_speed);
			} break;
		}
	}

	//my::mat4 GetMatrix() { return my::lookAtRH(m_pos, m_pos + m_target, m_up); }
	const glm::mat4& GetMatrix() {
		m_mView = glm::lookAtRH(m_pos, m_pos + m_target, m_up);
		return m_mView;
	}
	const glm::mat4& GetProjMatrix() {
		return m_mProj;
	}

	// left-handed z+ pint to the screen
	glm::mat4 GetMatrixLH() {
		//my::vec3 at = m_pos + m_target;
		//const my::vec3 f = normalize(at - m_pos);
		const glm::vec3 f = glm::normalize(m_target);
		const glm::vec3 r = glm::normalize(glm::cross(m_up, f));
		const glm::vec3 u = glm::cross(f, r);
		glm::mat4 m(1);
		m[0][0] = r.x; m[0][1] = u.x; m[0][2] = f.x;
		m[1][0] = r.y; m[1][1] = u.y; m[1][2] = f.y;
		m[2][0] = r.z; m[2][1] = u.z; m[2][2] = f.z;
		m[3][0] = -glm::dot(r, m_pos);
		m[3][1] = -glm::dot(u, m_pos);
		m[3][2] = -glm::dot(f, m_pos);
		return m;
	}

	// right-handed z- pint to the screen
	glm::mat4 GetMatrixRH() {
		const glm::vec3 f = glm::normalize(m_target);
		const glm::vec3 r = glm::normalize(glm::cross(f, m_up));
		const glm::vec3 u = glm::cross(r, f);
		glm::mat4 m(1);
		m[0][0] = r.x; m[0][1] = u.x; m[0][2] = -f.x;
		m[1][0] = r.y; m[1][1] = u.y; m[1][2] = -f.y;
		m[2][0] = r.z; m[2][1] = u.z; m[2][2] = -f.z;
		m[3][0] = -glm::dot(r, m_pos);
		m[3][1] = -glm::dot(u, m_pos);
		m[3][2] = glm::dot(f, m_pos);
		return m;
	}
};