#ifndef BEHAVIOUR_H
#define BEHAVIOUR_H
//#include "HelpFuctions.h"

class Behaviour {
public:
	virtual ~Behaviour(void) { }
	virtual void update(float dt) = 0;

protected:
	Behaviour(void) { }
};

#endif
