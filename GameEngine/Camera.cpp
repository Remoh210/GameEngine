#include "Camera.h"

void Camera::ProcessJoystickMovement(float X_Axis, float Y_Axis, GLboolean constrainPitch)
{

	// Translate the axes to offsets
	float xoffset = (X_Axis / 1.000015f) * 90.0f;
	float yoffset = (Y_Axis / 1.000015f) * 180.0f;

	Yaw += xoffset;
	Pitch += yoffset;

	//if (g_isCharacterMoving)
	//{
		//if (Pitch < -20.0f)
		//	Pitch += 5.0f;
		//else if (Pitch >= -20.0f && Pitch <= -6.0f)
		//	Pitch += 0.5f;

		//else if (Pitch > -4.0f)
		//	Pitch -= 0.5f;
	//}
	//else
	//{
	//	m_pitch += yoffset;
	//}

	// Make sure that when pitch is out of bounds, screen doesn't get flipped
	if (constrainPitch)
	{
		if (Pitch > 89.0f)
			Pitch = 89.0f;
		if (Pitch < -89.0f)
			Pitch = -89.0f;
	}


	if (this->mCameraType == THIRD_PERSON)
	{
		if (Pitch > 15.0f)
		{
			Pitch = 15.0f;
		}
	}
}
