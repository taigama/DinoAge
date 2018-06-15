#ifndef __ENEMY_MUSHROOM_H__
#define __ENEMY_MUSHROOM_H__

#include "Enemy.h"

/**
 * An Enemy character: Hopper
 *
 * It's always jumping
 *
 * Default anchor point: Vec2(0.5, 0)
 */
class EnemyMushroom : public Enemy
{
public:

	~EnemyMushroom();

	void setScaleMushroom(float scale);

	// init() function
	bool init();

	// Overrides
	bool onContactBegin(const cocos2d::PhysicsContact &contact) override;


	// -----------------------------------------------------------------------
	void onHit(float dmg, DIRECTION dir, float force) override;

	// EnemyFeeder is exploding or not
	CC_SYNTHESIZE(bool, _isExploding, ExplodingState);

	void onDie() override;

	CREATE_FUNC(EnemyMushroom);

protected:

	// Helper init() init fuctions
	void initSprites() override;
	void initPhysics() override;

	// Animation
	cocos2d::ActionInterval* _hittedAnimation;
	cocos2d::ActionInterval* _dieAnimation;
	float _scaleMushroom;

};



#endif // !__ENEMY_MUSHROOM_H__