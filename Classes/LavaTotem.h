#ifndef _LAVATOTEM_H_
#define _LAVATOTEM_H_

#include "Projectile.h"

class LavaTotem : public Projectile
{
public:

	virtual ~LavaTotem();
	CREATE_FUNC(LavaTotem);
	virtual bool init();

	virtual void update(float) override;

	virtual bool onContactBegin(const cocos2d::PhysicsContact &contact);
	void reset(float params[10]) override;

private:
	void initPhysics() override;
};
#endif // !_LAVATOTEM_H_