#ifndef _JAWATTACK_H_
#define _JAWATTACK_H_

#include "Projectile.h"

class JawAttack : public Projectile
{
public:
	CREATE_FUNC(JawAttack);
	virtual bool init();

	virtual void update(float) override;


	void reset(float params[10]) override;
protected:
	void initPhysics() override;

	void attack(Character* target, Character::DIRECTION) override;

private:
	cocos2d::ActionInterval* act_animate;
};
#endif // !_FIRE_H_
