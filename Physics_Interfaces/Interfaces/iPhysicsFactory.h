#pragma once
#include <glm/glm.hpp>
#include "iRigidBody.h"
#include "iPhysicsWorld.h"
#include "sRigidBodyDef.h"
#include "sSoftBodyDef.h"
#include "shapes.h"
#include "iSoftBody.h"
#include "iWreckingBallPhysics.h"


namespace nPhysics
{
	class iPhysicsFactory
	{
	public:
		virtual ~iPhysicsFactory() {};
		virtual iPhysicsWorld* CreatePhysicsWorld() = 0;
		virtual iRigidBody* CreateRigidBody(const sRigidBodyDef& def, iShape* shape) = 0;
		virtual iSphereShape* CreateSphereShape(float radius) = 0;
		virtual iPlaneShape* CreatePlaneShape(const glm::vec3& normal, float constant) = 0;
		virtual iCylinderShape* CreateCylinderShape(const glm::vec3& halfExtent, int axis) = 0;
		virtual iCapsuleShape* CreateCapsuleShape(float height, float radius, int axis) = 0;
		//virtual iSoftBody* CreateSoftBody(const sSoftBodyDef& sbDef) = 0;

		//virtual iWreckingBallPhysics* CreateWreckingBall(std::vector<sWreckingBallPNodeDef>& wreckingDef) = 0;

	};

}