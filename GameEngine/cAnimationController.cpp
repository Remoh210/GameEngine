#include "cAnimationController.h"
#include "cGameObject.h"
#include <iostream>


cAnimationController::cAnimationController(cGameObject* ActiveChar)
{
	mActiveChar = ActiveChar;
}

void cAnimationController::walkForward()
{
	if (mActiveChar->currentAnimation != "Run-jump") {
		mActiveChar->currentAnimation = "Walk-forward";
	}
}

void cAnimationController::JumpForward()
{
}

std::string cAnimationController::GetCurrentAnimation()
{

	glm::vec3 vel = mActiveChar->rigidBody->GetVelocity();
	glm::vec3 pos = mActiveChar->rigidBody->GetPosition();
	//std::cout << mActiveChar->bHadCollision << std::endl;
	if (!mActiveChar->bHadCollision)
	{
		return "Run-jump";
	}
	if (abs(vel.x) < 1.01f, abs(vel.z) < 1.01f)
	{
		return "Idle";
	}
	
	
	return this->mActiveChar->currentAnimation;
	
	
}

void cAnimationController::UpdateController()
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
