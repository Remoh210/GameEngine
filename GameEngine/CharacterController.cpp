#include "CharacterController.h"



cCharacterController::cCharacterController(cGameObject* ActiveChar)
{
	mActiveChar = ActiveChar;
}

void cCharacterController::walkForward()
{
	if (mActiveChar->currentAnimation != "Run-jump") {
		mActiveChar->currentAnimation = "Walk-forward";
	}
}

void cCharacterController::JumpForward()
{
}

std::string cCharacterController::GetCurrentAnimation()
{

	glm::vec3 vel = mActiveChar->rigidBody->GetVelocity();
	glm::vec3 pos = mActiveChar->rigidBody->GetPosition();
	if (pos.y > 10.51f)
	{
		return "Run-jump";
	}
	if (abs(vel.x) < 1.01f, abs(vel.z) < 1.01f)
	{
		return "Idle";
	}
	
	
	return this->mActiveChar->currentAnimation;
	
	
}

void cCharacterController::UpdateController()
{

	glm::vec3 vel = mActiveChar->rigidBody->GetVelocity();
	glm::vec3 pos = mActiveChar->rigidBody->GetPosition();
	if (pos.y > -2.0f)
	{
		this->curState = inAir;
	}
	else if (vel.x < 0.01f, vel.z < 0.01f)
	{
		this->curState = Idle;
	}

}
