#ifndef _THUNDERBODY_H_
#define _THUNDERBODY_H_

#include "Projectile.h"

class ThunderBody : public Projectile
{
public:
	CREATE_FUNC(ThunderBody);
	virtual bool init();

	virtual void update(float) override;


	void reset(float params[10]) override;
protected:
	void initPhysics() override;

	void attack(Character* target, Character::DIRECTION) override;

private:
	cocos2d::ActionInterval* act_animate;

	float m_heightLightning;

	void drawLightning(cocos2d::Vec2);
};
#endif // !_THUNDERBODY_H_
