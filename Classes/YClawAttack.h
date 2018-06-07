#ifndef _YCLAWATTACK_H_
#define _YCLAWATTACK_H_

#include "Projectile.h"

class YClawAttack : public Projectile
{
public:
	CREATE_FUNC(YClawAttack);
	virtual bool init();

	virtual void update(float) override;


	void reset(float params[10]) override;
protected:
	void initPhysics() override;

	void attack(Character* target, Character::DIRECTION) override;

private:
	cocos2d::ActionInterval* act_animate;
};
#endif // !_YCLAWATTACK_H_
