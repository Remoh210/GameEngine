#pragma once

#include <vector>
#include <Interfaces/iConstraint.h>
#include <Interfaces/iRigidBody.h>
#include "cBulletRigidBody.h"

namespace nPhysics
{
	//class cBallAndSocketConstraint : public iConstraint
	//{
	//public:
	//	cBallAndSocketConstraint(cBulletRigidBody* rb, const btVector3& pivot);
	//	cBallAndSocketConstraint(cBulletRigidBody* rbA, cBulletRigidBody* rbB, const btVector3& pivotInA, const btVector3& pivotInB);
	//	virtual ~cBallAndSocketConstraint();

	//	// From iConstraint
	//	virtual iRigidBody* GetRigidBodyA();
	//	virtual iRigidBody* GetRigidBodyB();

	//	virtual btTypedConstraint* GetTypedConstraint() { return mConstraint; }

	//private:
	//	btPoint2PointConstraint * mConstraint;
	//	cBallAndSocketConstraint(const cBallAndSocketConstraint& other) {}
	//	cBallAndSocketConstraint& operator=(const cBallAndSocketConstraint& other) { return *this; }

	//private:
	//	eConstraintType mConstraintType;
	//};

	class cBulletHingeConstraint : public iConstraint
	{
	public:
		cBulletHingeConstraint(cBulletRigidBody* rb, const btVector3& pivot, const btVector3& axis);
		cBulletHingeConstraint(cBulletRigidBody* rbA, cBulletRigidBody* rbB, const btVector3& pivotInA, const btVector3& pivotInB, const btVector3& axisInA, const btVector3& axisInB);
		virtual ~cBulletHingeConstraint();


		virtual btTypedConstraint* GetTypedConstraint() { return mConstraint; }

	private:
		btHingeConstraint* mConstraint;

		//cBulletHingeConstraint( eConstraintType constraintType ) : mConstraintType( constraintType ) {}
		cBulletHingeConstraint(const cBulletHingeConstraint& other) {}
		cBulletHingeConstraint& operator=(const iConstraint& other) { return *this; }

	private:
		eConstraintType mConstraintType;
	};

}