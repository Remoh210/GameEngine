#include "cAnimationController.h"
#include "cAnimationState.h"
#include "cGameObject.h"
#include "globalStuff.h"
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

	
	
	return "";
}

void cAnimationController::UpdateController()
{
	glm::vec3 vel = mActiveChar->rigidBody->GetVelocity();
	glm::vec3 pos = mActiveChar->rigidBody->GetPosition();
	//glm::vec3 from;
	float height = 7.0f;
	//from = glm::vec3(pos.x, pos.y -height, pos.z);

	glm::vec3 to = glm::vec3(pos.x, pos.y - height, pos.z);
	g_pDebugRendererACTUAL->addLine(pos, to, glm::vec3(1.0f, 0.0f, 0.0f));

	if (!gPhysicsWorld->RayCast(pos, to))
	{
		mActiveChar->pAniState->activeAnimation.name = "Run-jump";
		return;
	}


	if (mActiveChar->pAniState->activeAnimation.name == "Action6")
	{
		if (!mActiveChar->pAniState->activeAnimation.bExited)
		{
			glm::vec3 vel(0.0f, mActiveChar->rigidBody->GetVelocity().y, 0.0f);
			mActiveChar->rigidBody->SetVelocity(vel);
			return;
		}
		else
		{
			cGameObject* bow = findObjectByFriendlyName("bow");

			cGameObject* projectile = new cGameObject();
			projectile->meshName = "character/arrow.ply";
			projectile->friendlyName = "projectile" + std::to_string(projectile->getUniqueID());
			projectile->setDiffuseColour(glm::vec3(0.0f));
			float scale = 15.0f;
			projectile->nonUniformScale = glm::vec3(scale, scale, scale);
			projectile->m_meshQOrientation = mActiveChar->m_meshQOrientation;
			projectile->bIsWireFrame = false;
			projectile->bUseNormalMap = false;
			projectile->bDontLight = false;
			projectile->position = bow->position;
			projectile->bIsVisible = true;
			projectile->setSpecularPower(100.0f);
			projectile->bSave = false;

			vec_pObjectsToDraw.push_back(projectile);


			sTextureInfo CurModelTex1;
			CurModelTex1.name = "character/arrowTex.bmp";
			CurModelTex1.strength = 1.0f;
			projectile->vecTextures.push_back(CurModelTex1);

			sTextureInfo CurModelTex2;
			CurModelTex2.name = "testTex.bmp";
			CurModelTex2.strength = 0.0f;
			projectile->vecTextures.push_back(CurModelTex2);



			sModelDrawInfo arrowDrawInfo;
			arrowDrawInfo.meshFileName = "character/arrow.ply";
			g_pTheVAOMeshManager->FindDrawInfoByModelName(arrowDrawInfo);



			glm::vec3 halfExtents = glm::vec3(arrowDrawInfo.maxX * scale, arrowDrawInfo.maxY * scale,
				arrowDrawInfo.maxZ * scale);


			nPhysics::iShape* CurShape = NULL;
			nPhysics::sRigidBodyDef def;
			//in Radians
			def.Position = projectile->position;
			def.Mass = 50.0f;
			def.quatOrientation = projectile->m_meshQOrientation;
			def.GameObjectName = projectile->friendlyName;
			def.Velocity = projectile->getForward() * 600.0f;
			CurShape = gPhysicsFactory->CreateBoxShape(halfExtents);

			nPhysics::iRigidBody* rigidBody = gPhysicsFactory->CreateRigidBody(def, CurShape);
			projectile->rigidBody = rigidBody;
			gPhysicsWorld->AddBody(rigidBody);

			g_pSoundManager->playSound("shot");
			std::cout << "fire!" << std::endl;


			mActiveChar->currentAnimation = "Idle";


		}
	}


	if (abs(vel.x) < 1.01f && abs(vel.z) < 1.01f)
	{
		if (mActiveChar->currentAnimation != "Action2" && mActiveChar->currentAnimation != "Action6")
		{
			mActiveChar->pAniState->activeAnimation.name = "Idle";
			//return "Idle";
		}
		else
		{
			mActiveChar->pAniState->activeAnimation.name = mActiveChar->currentAnimation;
			//this->mActiveChar->currentAnimation;
		}

	}

	mActiveChar->pAniState->activeAnimation.name = mActiveChar->currentAnimation;




	//glm::vec3 vel = mActiveChar->rigidBody->GetVelocity();
	//glm::vec3 pos = mActiveChar->rigidBody->GetPosition();
	//if (pos.y > -2.0f)
	//{
	//	this->curState = inAir;
	//}
	//else if (vel.x < 0.01f, vel.z < 0.01f)
	//{
	//	this->curState = Idle;
	//}
	//if (mActiveChar->pAniState->activeAnimation.name != "Action2" && mActiveChar->currentAnimation != "Action6")
	//{
	//	return "Action6";
	//}

	
	











}
