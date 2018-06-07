#ifndef _ICEDRAGON_H_
#define _ICEDRAGON_H_

#include "Projectile.h"

class IceDragon : public Projectile
{
public:
	

	virtual ~IceDragon();
	CREATE_FUNC(IceDragon);
	virtual bool init();

	virtual void update(float) override;


	void reset(float params[10]) override;

private:
	// function run in update
	typedef void (IceDragon::*Update)(float);
	Update doNow;

	// when dragon new strike up
	void doNew(float);
	// when dragon moving
	void doGo(float);
	// when dragon end attack
	void doEnd(float);


	void initSprites() override;
	void initPhysics() override;


	/*enum DRAGON_STATE
	{
		NEW,
		MOVING,
		ENDED
	};
	DRAGON_STATE m_state;*/
	int m_direction;

	cocos2d::ActionInterval* act_animateNew;
	cocos2d::ActionInterval* act_animateGo;
	float timer;
	cocos2d::ActionInterval* act_animateEnd;
};
#endif // !_ICEDRAGON_H_
