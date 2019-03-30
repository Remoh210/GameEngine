#include "cGameObject.h"

cGameObject::cGameObject()
{
	this->position = glm::vec3(0.0f);
	this->nonUniformScale = glm::vec3(1.0f);
	this->setMeshOrientationEulerAngles(glm::vec3(0.0f, 0.0f, 0.0f));
	this->bIsDebug = false;
	this->bIsVisible = true;
	this->bIsWireFrame = false;
	this->bFBO = false;
	this->pAnimController = new cAnimationController(this);
	this->bHadCollision = false;
	this->bIsPlayer = false;
	
	// Set unique ID
	this->m_uniqueID = cGameObject::m_NextID;
	// Increment
	cGameObject::m_NextID++;	// 32 bit - 4 billion

	this->materialDiffuse = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
	this->materialSpecular = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);

	this->bUseVertexColour = false;

	this->bDontLight = false;

	this->velocity = glm::vec3(0.0f);
	this->accel = glm::vec3(0.0f);
	this->bIsUpdatedByPhysics = false;	// physics ignores by default
	this->bIsUpdatedByPhysics = false;


	this->pDebugRenderer = NULL;
	this->pTheShape = NULL;
	this->shapeType = cGameObject::UNKOWN_SHAPE;
	this->rigidBody = NULL;
	this->softBody = NULL;



	this->pSimpleSkinnedMesh = NULL;
	this->pAniState = NULL;

	return;
}

void cGameObject::setDiffuseColour(glm::vec3 newDiffuse)
{
	this->materialDiffuse = glm::vec4(newDiffuse, this->materialDiffuse.a);
	return;
}

void cGameObject::setAlphaTransparency(float newAlpha)
{
	this->materialDiffuse.a = newAlpha;
	return;
}

void cGameObject::setSpecularColour(glm::vec3 colourRGB)
{
	this->materialSpecular = glm::vec4(colourRGB, this->materialSpecular.a);
	return;
}

void cGameObject::setSpecularPower(float specPower)
{
	this->materialSpecular.a = specPower;
	return;
}


glm::vec3 cGameObject::getForward(glm::vec3 forwardModelSpace)
{
	//glm::vec4 vecForwardDirection_ModelSpace = glm::vec4(forwardModelSpace, 1.0f);
	//glm::quat qPlayerRotation = this->getQOrientation();
	//glm::mat4 matPlayerRotation = glm::mat4(qPlayerRotation);
	//glm::vec4 vecForwardDirection_WorldSpace = matPlayerRotation * vecForwardDirection_ModelSpace;
	//vecForwardDirection_WorldSpace = glm::normalize(vecForwardDirection_WorldSpace);
	//return vecForwardDirection_WorldSpace;

	glm::vec4 vecForwardDirection_ModelSpace = glm::vec4(0.0f, 0.0f, 1.0f, 1.0f);

	// Now orientation
	glm::quat qMig29Rotation = this->m_meshQOrientation;

	glm::mat4 matQMig29rotation = glm::mat4(qMig29Rotation);

	glm::vec4 vecForwardDirection_WorldSpace = matQMig29rotation * vecForwardDirection_ModelSpace;

	// optional normalize
	vecForwardDirection_WorldSpace = glm::normalize(vecForwardDirection_WorldSpace);
	glm::vec3 newve(vecForwardDirection_WorldSpace.x, vecForwardDirection_WorldSpace.y, vecForwardDirection_WorldSpace.z);
	return newve;

}

void cGameObject::setUniformScale(float scale)
{
	this->nonUniformScale = glm::vec3(scale, scale, scale);
	return;
}



//static 
unsigned int cGameObject::m_NextID = cGameObject::STARTING_ID_VALUE;


void cGameObject::Update(double deltaTime)
{
	if (this->bIsUpdatedByPhysics)
	{
		// Figure out a line showing the velocityz
		glm::vec3 vVel = this->velocity + this->position;
		glm::vec3 vAcc = this->accel + this->position;

		if (this->pDebugRenderer)		// != NULL
		{
			this->pDebugRenderer->addLine(this->position, vVel,
				glm::vec3(0.0f, 1.0f, 0.0f), 0.0f);

			this->pDebugRenderer->addLine(this->position, vAcc,
				glm::vec3(0.0f, 1.0f, 1.0f), 0.0f);
		}//if ( this->pDebugRenderer )	
	}//if ( this->bIsUpdatedByPhysics )

	return;
}
