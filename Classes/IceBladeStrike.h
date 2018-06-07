#ifndef _ICEBLADESTRIKE_H_
#define _ICEBLADESTRIKE_H_

#include "Projectile.h"

// this is, Ice Blade explode
class IceBladeStrike : public Projectile
{
public:

	virtual ~IceBladeStrike();
	CREATE_FUNC(IceBladeStrike);
	virtual bool init();

	virtual void update(float) override;

	virtual bool onContactBegin(const cocos2d::PhysicsContact &contact);
	void reset(float params[10]) override;

private:
	void initPhysics() override;

	bool m_needSetPosition;
	cocos2d::Vec2 m_posReset;
};
#endif // !_ICEBLADESTRIKE_H_