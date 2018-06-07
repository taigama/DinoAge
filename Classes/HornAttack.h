#ifndef _HORNATTACK_H_
#define _HORNATTACK_H_

#include "Projectile.h"

class HornAttack : public Projectile
{
public:
	CREATE_FUNC(HornAttack);
	virtual bool init();

	virtual void update(float) override;


	void reset(float params[10]) override;
protected:
	void initPhysics() override;

	void attack(Character* target, Character::DIRECTION) override;

private:
	cocos2d::ActionInterval* act_animate;
};
#endif // !_HORNATTACK_H_
