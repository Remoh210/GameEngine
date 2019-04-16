#ifndef BEHAVIOUR_MANAGER_H
#define BEHAVIOUR_MANAGER_H

#include <map>
#include "cGameObject.h"
#include "Behaviour.h"

class BehaviourManager {
public:
	BehaviourManager(void);
	~BehaviourManager(void);

	void SetBehaviour(cGameObject* agent, Behaviour* behaviour);
	void RemoveAgent(cGameObject* agent);

	void update(float dt);

private:
	std::map<cGameObject*, Behaviour*> mBehaviourMap;
};

typedef std::map<cGameObject*, Behaviour*>::iterator behaviour_iterator;


#endif
