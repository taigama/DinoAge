#ifndef _LIGHTNINGSTRIKE_H_
#define _LIGHTNINGSTRIKE_H_

#include "Projectile.h"

class LightningStrike : public Projectile
{
public:

	virtual ~LightningStrike();
	CREATE_FUNC(LightningStrike);
	virtual bool init();

	virtual void update(float) override;


	void reset(float params[10]) override;
protected:
	void initPhysics() override;

	cocos2d::ActionInterval* act_animate;
};
#endif // !_LIGHTNINGSTRIKE_H_
