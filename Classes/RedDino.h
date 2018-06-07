#ifndef _REDDINO_H_
#define _REDDINO_H_

#include "Player.h"


class RedDino : public Player
{
public:
	// initilize
	virtual bool init();
	CREATE_FUNC(RedDino);

	// destructor
	virtual ~RedDino();

	// main update function
	virtual void update(float) override;

	// info function
	virtual cocos2d::Rect getBoundingBox() const override;

	// red dino has double jump
	//void jump() override;


	void attackPhysical() override;
	void attackProjectile() override;

	void meteourRain();
	void lavaTotem();
	void dragonNova();
	

	//// Callback function for when the collision begins
	//virtual bool onContactBegin(const cocos2d::PhysicsContact &contact) override;
	//// Callback function during the contact, but before collision is resolved
	//virtual bool onContactPresolve(const cocos2d::PhysicsContact &contact) override;
	//// Callback function after separate
	//virtual bool onContactSeparate(const cocos2d::PhysicsContact &contact) override;


private:
	// called when GameObject::init()
	void initSprites() override;
	void initPhysics() override;

	cocos2d::Vec2 m_offsetMouth_bite;
	cocos2d::Vec2 m_offsetMouth_fire;

	cocos2d::ActionInterval* act_nova;
};
#endif // !_REDDINO_H_
