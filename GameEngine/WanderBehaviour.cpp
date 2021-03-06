#include "WanderBehaviour.h"
//#include <GLFW/glfw3.h>
#include <iostream>
#include "globalStuff.h"

glm::vec3 oldTarget;
;
float map(float value, float minA, float maxA, float minB, float maxB) {
	return (1 - ((value - minA) / (maxA - minA))) * minB + ((value - minA) / (maxA - minA)) * maxB;
}

float RandomFloat(float a, float b);
WanderBehaviour::WanderBehaviour(cGameObject* agent, float maxSpeed, float maxForce, float timeToWait, glm::vec3 RelPos, float upLim, float dwLim, cGameObject* player)
	: mAgent(agent)
	//, mTarget(target)
	, mMaxSpeed(maxSpeed)
	, mMaxForce(maxForce)
	, mTimeToWait(timeToWait)
	, mRelPos(RelPos)
	, mUpLim(upLim)
	, mDwnLim(dwLim)
	, targetPlayer(player)
	, pusuePlayer(false)
{
	mStart = true;
	mCurTarget = glm::vec3(mAgent->position.x + RandomFloat(mDwnLim, mUpLim), 0.0f, mAgent->position.z + RandomFloat(mDwnLim, mUpLim));
	mTimeWaitedSoFar = 0.0f;
}

WanderBehaviour::~WanderBehaviour(void)
{
}

void WanderBehaviour::update(float dt)
{
	//float curTime = 3.1f;
	if (mAgent == 0) return;
	
	if (mAgent->isDead) return;
	

	if (glm::distance(targetPlayer->position, mAgent->position) < 100.0f)
	{
		glm::vec3 desired = targetPlayer->position - mAgent->position;
		//normalize it and scale by mMaxSpeed
		desired = glm::normalize(desired) * mMaxSpeed;

		//steering = glm::normalize(desired) * mMaxForce;
		mAgent->rigidBody->SetVelocity(desired * 2.0f);

		glm::vec3 lookDirection = mAgent->position - targetPlayer->position;
		glm::mat4 rot = glm::inverse(glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), lookDirection, glm::vec3(0.0f, 1.0f, 0.0f)));
		mAgent->currentAnimation = "Run-forward";
		mAgent->m_meshQOrientation = glm::quat(rot);
		pusuePlayer = true;


	}
	else
	{
		pusuePlayer = false;
	}


	if (glm::distance(mAgent->position, mCurTarget) > 15.0f && !pusuePlayer) {

		

		
		//
	


		glm::vec3 desired = mCurTarget - mAgent->position;
		//normalize it and scale by mMaxSpeed
		desired = glm::normalize(desired) * mMaxSpeed;
		float d = glm::length(desired);

		desired = glm::normalize(desired) * mMaxSpeed;
		

		glm::vec3 steering = desired - mAgent->rigidBody->GetVelocity();

		if (glm::length(steering) > mMaxForce) {
			steering = glm::normalize(steering) * mMaxForce;
		}
		//steering = glm::normalize(desired) * mMaxForce;
		//mAgent->rigidBody->ApplyImpulse(steering);
		mAgent->rigidBody->SetVelocity(desired);

		glm::vec3 lookDirection = mAgent->position - mCurTarget;
		glm::mat4 rot = glm::inverse(glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), lookDirection, glm::vec3(0.0f, 1.0f, 0.0f)));
		mAgent->currentAnimation = "Walk-forward";
		mAgent->m_meshQOrientation = glm::quat(rot);

		



	}
	else if (mTimeToWait != 0.0f && !pusuePlayer)
	{ 
		mAgent->rigidBody->SetVelocity(glm::vec3(0.0f));
		if (mStart) {
			mTimeWaitedSoFar = 0.0f;
			mStart = false;
		}
		//mAgent->velocity = glm::vec3(0.0f);
		//mAgent->accel = glm::vec3(0.0f);
		mTimeWaitedSoFar += (float)dt;
		mAgent->currentAnimation = "Idle";
		if (mTimeWaitedSoFar > mTimeToWait)
		{
			mCurTarget = glm::vec3(mAgent->position.x + RandomFloat(mDwnLim, mUpLim), 0.0f, mAgent->position.z + RandomFloat(mDwnLim, mUpLim));
			mStart = true;
		}
	}
	else
	{
		if (!pusuePlayer) {
			mCurTarget = glm::vec3(mAgent->position.x + RandomFloat(mDwnLim, mUpLim), 0.0f, mAgent->position.z + RandomFloat(mDwnLim, mUpLim));
		}
	}
	
	cGameObject* pDebugSph = findObjectByFriendlyName("DebugSphere");
	pDebugSph->position = mCurTarget;
	pDebugSph->setUniformScale(2.0f);
	pDebugSph->setDiffuseColour(glm::vec3(1.0f, 0.0f, 0.0f));
	glm::mat4 iden = glm::mat4(1.0f);
	DrawObject(pDebugSph, iden, program);
	
}


float RandomFloat(float a, float b) {
	//srand(glfwGetTime());
	float random = ((float)rand()) / (float)RAND_MAX;
	float diff = b - a;
	float r = random * diff;
	return a + r;
}
