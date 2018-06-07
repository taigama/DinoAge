#ifndef __MEAT_H__
#define __MEAT_H__

#include "Item.h"

class Meat : public Item
{
public:

	~Meat();

	// init() function
	virtual bool init();

	CREATE_FUNC(Meat);

protected:

	// Inherited helper functions for Physics
	virtual void initPhysics();
	virtual void initSprites();

};

#endif // !__MEAT_H__
