#ifndef _WHITEDINO_H_
#define _WHITEDINO_H_

#include "Player.h"


class WhiteDino : public Player
{
public:
	// initilize
	virtual bool init();
	CREATE_FUNC(WhiteDino);

	// destructor
	virtual ~WhiteDino();

	// main update function
	virtual void update(float) override;

	// info function
	virtual cocos2d::Rect getBoundingBox() const override;

	// red dino has double jump
	//void jump() override;


	void attackPhysical() override;
	void attackProjectile() override;

	
	void mysticDragon();
	void iceWall();
	void glacialBlade();

	//// Callback function for when the collision begins
	//virtual bool onContactBegin(const cocos2d::PhysicsContact &contact) override;
	//// Callback function during the contact, but before collision is resolved
	//virtual bool onContactPresolve(const cocos2d::PhysicsContact &contact) override;
	//// Callback function after separate
	//virtual bool onContactSeparate(const cocos2d::PhysicsContact &contact) override;

	bool wallRaycastCallback(cocos2d::PhysicsWorld& world, const cocos2d::PhysicsRayCastInfo& info, void* data);

private:
	// called when GameObject::init()
	void initSprites() override;
	void initPhysics() override;

	//bool m_isRunAttack;

	void endAttack() override;
	//void knock(DIRECTION dir, float force) override;

	cocos2d::Vec2 m_offsetMouth;

	int m_countWallLeft;
	int m_countWallRight;
};
#endif // !_REDDINO_H_
