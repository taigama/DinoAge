// just like fire, but falling from sky

#ifndef _FIREBALL_H_
#define _FIREBALL_H_

#include "Projectile.h"

// a fire projectile that fall down
class FireBall : public Projectile
{
public:

	virtual ~FireBall();
	CREATE_FUNC(FireBall);
	virtual bool init();

	virtual void update(float) override;

	//virtual bool onContactBegin(const cocos2d::PhysicsContact &contact) override;


	void reset(float params[10]) override;
private:
	void onDie() override;

	void initSprites() override;
	void initPhysics() override;

	//virtual void onEnable() override;
	//virtual void onDisable() override;

	void attack(Character* target, Character::DIRECTION) override;

	cocos2d::ActionInterval* act_animate;

	float m_speedY;
};
#endif // !_FIRE_H_
