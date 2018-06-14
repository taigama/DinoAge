#ifndef __ENEMY_HOPPER_H__
#define __ENEMY_HOPPER_H__

#include "Enemy.h"

/**
 * An Enemy character: Hopper
 *
 * It's always jumping
 *
 * Default anchor point: Vec2(0.5, 0)
 */
class EnemyHopper : public Enemy
{
public:

	enum class FEEDER_TYPE {
		NONE = 0,
		STATIONARY = 1,
		MOVING = 2
	};

	~EnemyHopper();

	// init() function
	bool init();

	// update() function
	void update(float delta) override;

	// Overrides
	bool onContactBegin(const cocos2d::PhysicsContact &contact) override;



	void jump();

	// -----------------------------------------------------------------------


	// EnemyFeeder is exploding or not
	CC_SYNTHESIZE(bool, _isExploding, ExplodingState);

	CC_SYNTHESIZE_READONLY(cocos2d::Vec2, _vecMove, VecMove);

	void onDie() override;

	CREATE_FUNC(EnemyHopper);

protected:

	// Helper init() init fuctions
	void initSprites() override;
	void initPhysics() override;
	float _duration;

	// Animation
	cocos2d::ActionInterval* _jumpAnimation;
	cocos2d::ActionInterval* _dieAnimation;


};



#endif // !__ENEMY_HOPPER_H__