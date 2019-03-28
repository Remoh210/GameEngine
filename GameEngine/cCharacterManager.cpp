#include "cCharacterManager.h"
#include <iostream>

cCharacterManager::cCharacterManager(std::vector<cGameObject*> vecGO)
{
	for (int i = 0; i < vecGO.size(); i++)
	{
		if (vecGO[i]->bIsPlayer)
		{
			mMapCharacters[vecGO[i]->friendlyName] = vecGO[i];
		}
		
	}
}

cGameObject* cCharacterManager::getActiveChar()
{
	std::cout <<" about toooo" << std::endl;
	return mActiveCharacter;
}

void cCharacterManager::setActiveChar(cGameObject * newActiveChar)
{
	mActiveCharacter = newActiveChar;
}

void cCharacterManager::setActiveChar(std::string GoName)
{
	std::map<std::string, cGameObject*>::iterator it = mMapCharacters.find(GoName);

	if (it == mMapCharacters.end())
	{
		std::cout << GoName << " character not found" << std::endl;
		return;
	}
	else 
	{
		mActiveCharacter = it->second; 
	}
	
}
