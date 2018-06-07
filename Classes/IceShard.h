#ifndef _ICESHARD_H_
#define _ICESHARD_H_

#include "Projectile.h"

// shard of ice, fired by White Dino
class IceShard : public Projectile
{
public:

	virtual ~IceShard();
	CREATE_FUNC(IceShard);
	virtual bool init();

	virtual void update(float) override;


	void reset(float params[10]) override;
protected:
	void onDie() override;

	void initSprites() override;
	void initPhysics() override;

	//virtual void onEnable() override;
	//virtual void onDisable() override;

	void attack(Character* target, Character::DIRECTION) override;

	cocos2d::ActionInterval* act_animate;
};
#endif // !_ICESHARD_H_
