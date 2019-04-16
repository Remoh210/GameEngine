#include "WanderBehaviour.h"
//#include <GLFW/glfw3.h>
#include <iostream>
#include "globalStuff.h"

glm::vec3 oldTarget;
bool pusuePlayer = false;
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
	
	


	if (glm::distance(mAgent->position, mCurTarget) > 5.0f) {

		

		
		//
		if (glm::distance(targetPlayer->position, mAgent->position) < 60.0f)
		{
			glm::vec3 desired = targetPlayer->position - mAgent->position;
			//normalize it and scale by mMaxSpeed
			desired = glm::normalize(desired) * mMaxSpeed;

			//steering = glm::normalize(desired) * mMaxForce;
			mAgent->rigidBody->SetVelocity(desired);

			glm::vec3 lookDirection = mAgent->position - targetPlayer->position;
			glm::mat4 rot = glm::inverse(glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), lookDirection, glm::vec3(0.0f, 1.0f, 0.0f)));
			mAgent->currentAnimation = "Walk-forward";
			mAgent->m_meshQOrientation = glm::quat(rot);
		}
		else
		{

			glm::vec3 desired = mCurTarget - mAgent->position;
			//normalize it and scale by mMaxSpeed
			desired = glm::normalize(desired) * mMaxSpeed;
			float d = glm::length(desired);
			if (d < 10.0f) {
				float m = map(d, 0, 10, 0, mMaxSpeed);
				desired = glm::normalize(desired) * m;
			}
			else {
				desired = glm::normalize(desired) * mMaxSpeed;
			}

			glm::vec3 steering = desired - mAgent->rigidBody->GetVelocity();

			if (glm::length(steering) > mMaxForce) {
				steering = glm::normalize(steering) * mMaxForce;
			}
			//steering = glm::normalize(desired) * mMaxForce;
			mAgent->rigidBody->ApplyImpulse(steering);

			glm::vec3 lookDirection = mAgent->position - mCurTarget;
			glm::mat4 rot = glm::inverse(glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), lookDirection, glm::vec3(0.0f, 1.0f, 0.0f)));
			mAgent->currentAnimation = "Walk-forward";
			mAgent->m_meshQOrientation = glm::quat(rot);

		}



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
