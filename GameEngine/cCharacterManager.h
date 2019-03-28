#pragma once
#include "cGameObject.h"
#include <map>
#include <string>

class cCharacterManager
{
public:
	cCharacterManager(std::vector<cGameObject*> vecGO);
	
	cGameObject* getActiveChar();
	void setActiveChar(cGameObject* newActiveChar);
	void setActiveChar(std::string GoName);
	void IterateChar();

private:

	cGameObject* mActiveCharacter;
	std::map< std::string, cGameObject*> mMapCharacters;
};