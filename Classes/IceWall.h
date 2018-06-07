#ifndef _ICEWALL_H_
#define _ICEWALL_H_

#include "Projectile.h"

class IceWall : public Projectile
{
public:

	virtual ~IceWall();
	CREATE_FUNC(IceWall);
	virtual bool init();

	virtual void update(float) override;

	virtual bool onContactBegin(const cocos2d::PhysicsContact &contact);
	void reset(float params[10]) override;

private:
	void initSprites() override;
	void initPhysics() override;

	cocos2d::ActionInterval* act_animate;

	bool m_isNew;

	GameObject* m_brick;
};
#endif // !_FIRE_H_