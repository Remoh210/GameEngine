#include "Camera.h"

void Camera::ProcessJoystickMovement(float X_Axis, float Y_Axis, GLboolean constrainPitch)
{


	float xoffset = X_Axis * 90.0f;
	float yoffset = -Y_Axis * 180.0f;

	Yaw += xoffset;
	Pitch += yoffset;


	if (constrainPitch)
	{
		if (Pitch > 89.0f)
			Pitch = 89.0f;
		if (Pitch < -89.0f)
			Pitch = -89.0f;
	}


	//if (this->mCameraType == THIRD_PERSON)
	//{
	//	if (Pitch > 15.0f)
	//	{
	//		Pitch = 15.0f;
	//	}
	//}
}
