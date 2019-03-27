#pragma once
#include "cGameObject.h"


enum eCharState
{
	Idle = 0,
	inAir = 1

};

class cCharacterController
{
public:
	cCharacterController(cGameObject* ActiveChar);

	std::string GetCurrentAnimation();
	//SetCharacter;

	//Not using yet
	void walkForward();
	void JumpForward();
	void UpdateController();
	

private:

	cGameObject* mActiveChar;
	eCharState curState;
};