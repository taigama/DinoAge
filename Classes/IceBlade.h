// ice blades falling from sky, killing spree

#ifndef _ICEBLADE_H_
#define _ICEBLADE_H_

#include "Projectile.h"

// a projectile that fall down
class IceBlade : public Projectile
{
public:

	virtual ~IceBlade();
	CREATE_FUNC(IceBlade);
	virtual bool init();

	virtual void update(float) override;

	virtual bool onContactBegin(const cocos2d::PhysicsContact &contact) override;


	void reset(float params[10]) override;
private:
	void initSprites() override;
	void initPhysics() override;

	//virtual void onEnable() override;
	//virtual void onDisable() override;

	void attack(Character* target, Character::DIRECTION) override;

	cocos2d::ActionInterval* act_animate;

	float m_speedY;
	int m_direction;
};
#endif // !_ICEBLADE_H_
