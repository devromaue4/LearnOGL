#pragma once

#include "mymath.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class WorldTransform {
public:
	WorldTransform(){}

	void SetScale(float scale) { m_scale = scale; }
	void SetRotation(float angle, float x, float y, float z) { m_angle = angle; m_rotation.x = x; m_rotation.y = y; m_rotation.z = z; }
	void SetPosition(float x, float y, float z) { m_position.x = x; m_position.y = y; m_position.z = z; }
	void Rotate(float x, float y, float z) { m_rotation.x += x;  m_rotation.y += y;  m_rotation.z += z; }

	my::mat4 GetMatrix() {
		my::mat4 mScale(1), mRot(1), mTrans(1);
		mTrans = my::translate(mTrans, my::vec3(m_position.x, m_position.y, m_position.z));
		mScale = my::scale(mScale, my::vec3(m_scale, m_scale, m_scale));

		if (m_angle == 0 || (m_rotation.x == 0 && m_rotation.y == 0 && m_rotation.z == 0)) return mTrans * mScale;
		
		mRot = my::rotate(mRot, my::radians(m_angle), my::vec3(m_rotation.x, m_rotation.y, m_rotation.z));
		return mTrans * mRot * mScale;
	}

private:
	float m_scale = 1.0f;
	float m_angle = 0.0f;
	my::vec3 m_rotation = my::vec3(0, 0, 0);
	my::vec3 m_position = my::vec3(0, 0, 0);
};