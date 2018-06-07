#ifndef __GOLD_H__
#define __GOLD_H__

#include "Item.h"

class Gold : public Item
{
public:

	~Gold();

	// init() function
	virtual bool init();

	// Overrides
	void reset(float params[10]) override;

	// Returns the correctly "scaled" sprite of Gold object
	static cocos2d::Sprite* makeSprite();

	CREATE_FUNC(Gold);

protected:

	// Inherited helper functions for Physics
	virtual void initPhysics();
	virtual void initSprites();

	// ActionInterval holding animation for Gold
	cocos2d::ActionInterval* _action;

};

#endif // !__GOLD_H__
