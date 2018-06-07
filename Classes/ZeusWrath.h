#ifndef _ZEUSWRATH_H_
#define _ZEUSWRATH_H_

#include "Projectile.h"

// just a projectile that fill the screen
class ZeusWrath : public Projectile
{
public:
	CREATE_FUNC(ZeusWrath);
	virtual bool init();

	virtual void update(float) override;
	virtual bool onContactBegin(const cocos2d::PhysicsContact &contact) override;

	void reset(float params[10]) override;
protected:
	void initPhysics() override;

private:
	cocos2d::Vec2 m_pointStart;
	cocos2d::Vec2 m_pointEnd;

	void drawLightning();

	GameObject* m_lightning;
	float m_heightLightning;
};
#endif // !_ZEUSWRATH_H_
