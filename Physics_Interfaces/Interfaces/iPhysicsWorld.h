#pragma once
#include "iRigidBody.h"
#include "iSoftBody.h"
namespace nPhysics {
	class iPhysicsWorld
	{
	public:
		virtual ~iPhysicsWorld() {};

		virtual void SetGravity(const glm::vec3& gravity) = 0;
		virtual bool AddBody(iRigidBody* body) = 0;
		virtual bool RemoveBody(iRigidBody* body) = 0;

		//virtual bool AddBody(iSoftBody* body) = 0;
		//virtual bool RemoveBody(iSoftBody* body) = 0;

		virtual void Update(float dt) = 0;

	protected:
		iPhysicsWorld() {}
		iPhysicsWorld(const iPhysicsWorld& other) {}
		iPhysicsWorld& operator=(const iPhysicsWorld& other) { return *this; }
	};

}