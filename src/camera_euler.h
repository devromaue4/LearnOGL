#pragma once

#include "mymath.h"

enum EcamMovements {
	FORWARD,
	BACKWARD,
	LEFT,
	RIGHT
};

class CameraEuler {
public:
	my::vec3 Pos;
	my::vec3 Forward;
	my::vec3 Right;
	my::vec3 Up;
	my::vec3 WorldUp;

	my::mat4 mView = my::mat4(1);
	my::mat4 mProj = my::mat4(1);

	float Yaw, Pitch;
	float Speed = 2.5f;
	float Sensitivity = 0.1f;
	float FovY = 45.0f; // for a realistic view it is usually set to 45 degrees

	float mouseLastX;
	float mouseLastY;
	bool firstMouse = true;

	CameraEuler(int Width, int Height, my::vec3 pos = my::vec3(0.0f, 0.0f, 0.0f), my::vec3 up = my::vec3(0.0f, 1.0f, 0.0f), float yaw = -90.0f, float pitch = 0.0f)
		: Pos(pos), Forward(my::vec3(0.0f, 0.0f, -1.0f)), WorldUp(up), Yaw(yaw), Pitch(pitch) {

		mouseLastX = (float)Width / 2;
		mouseLastY = (float)Height / 2;
		calcVectors();

		mProj = my::perspectiveRH(45.0f, (float)Width / Height, .1f, 1000.0f);
	}

	void setProj(float fAspect = 1.0f, float fovy = 45.0f, float zNear = 0.1f, float zFar = 1000.0f) {
		mProj = my::perspectiveRH(fovy, fAspect, zNear, zFar);
	}

	const my::mat4& getMat() { 
		mView = my::lookAtRH(Pos, Pos + Forward, Up);
		return mView;
	}
	//my::mat4 getMat() { return my::lookAtLH(Pos, Pos + Forward, Up); }
	const my::mat4& getProj() const { return mProj; }

	void processKeyboard(EcamMovements moveDir, float deltaTime) {
		float velocity =  Speed * deltaTime;
		if (moveDir == FORWARD) Pos = Pos + Forward * velocity;
		if (moveDir == BACKWARD) Pos = Pos - Forward * velocity;	
		if (moveDir == LEFT) Pos = Pos - Right * velocity;
		if (moveDir == RIGHT) Pos = Pos + Right * velocity;
	}

	void processMouse(float xposIn, float yposIn, GLboolean constrainPitch = true) {
		float xpos = (float)xposIn, ypos = (float)yposIn;

		if (firstMouse) { // prevent jump mouse when first time capture the cursor
			mouseLastX = xpos;
			mouseLastY = ypos;
			firstMouse = false;
		}

		float xOffset = xpos - mouseLastX;
		float yOffset = mouseLastY - ypos; // reversed since y-coordinates range from bottom to top

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

		calcVectors();
	}

	void processScroll(float yoffset) {
		FovY -= (float)yoffset;
		if (FovY < 1.0f) FovY = 1.0f;
		//if (gFovY > 45.0f) gFovY = 45.0f;
		if (FovY > 90.0f) FovY = 90.0f;
	}

private:
	void calcVectors() {
		// Euler angles
		Forward.x = cos(my::radians(Yaw)) * cos(my::radians(Pitch));
		Forward.y = sin(my::radians(Pitch));
		Forward.z = sin(my::radians(Yaw)) * cos(my::radians(Pitch));

		// Quaternion
		//my::vec3 Yaxis(0, 1, 0);
		//// rotate the view vector by the horizontal angle around the vertical axis
		//my::vec3 view(1, 0, 0);
		////my::vec3 view(0, 0, -1);
		//view.rotate(-Yaw, Yaxis);
		//view.normalize();
		//// rotate the view vector by the vertical angle around the horizontal axis
		//my::vec3 U = my::crossProd(Yaxis, view);
		//U.normalize();
		//view.rotate(-Pitch, U);
		//Forward = view;

		///////////////// common /////////////////////
		Forward.normalize();
		Right = my::normalize(my::crossProd(Forward, WorldUp));
		Up = my::normalize(my::crossProd(Right, Forward));
	}
};