#ifndef _FIRE_H_
#define _FIRE_H_

#include "Projectile.h"

// a fire projectile fly forward
class Fire : public Projectile
{
public:

	virtual ~Fire();
	CREATE_FUNC(Fire);
	virtual bool init();

	virtual void update(float) override;


	void reset(float params[10]) override;
protected:
	void onDie() override;

	void initSprites() override;
	void initPhysics() override;

	//virtual void onEnable() override;
	//virtual void onDisable() override;

	void attack(Character* target, Character::DIRECTION) override;

	cocos2d::ActionInterval* act_animate;
};
#endif // !_FIRE_H_
