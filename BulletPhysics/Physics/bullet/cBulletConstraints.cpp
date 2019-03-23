#include "cBulletConstraints.h"



nPhysics::cBulletHingeConstraint::cBulletHingeConstraint(cBulletRigidBody * rb, const btVector3 & pivot, const btVector3 & axis)
	: iConstraint(CONSTRAINT_TYPE_HINGE)
{
	mConstraint = new btHingeConstraint(*rb->GetBulletBody(), pivot, axis);
}

nPhysics::cBulletHingeConstraint::cBulletHingeConstraint(cBulletRigidBody* rbA, cBulletRigidBody* rbB, const btVector3& pivotInA, const btVector3& pivotInB, const btVector3& axisInA, const btVector3& axisInB)
	: iConstraint(CONSTRAINT_TYPE_HINGE)
{
	mConstraint = new btHingeConstraint(*rbA->GetBulletBody(), *rbB->GetBulletBody(), pivotInA, pivotInB, axisInA, axisInB);
}

nPhysics::cBulletHingeConstraint::~cBulletHingeConstraint()
{
}


