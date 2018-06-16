#ifndef __ENEMY_FEEDER_H__
#define __ENEMY_FEEDER_H__

#include "Enemy.h"

#include "SelfExplode.h"

/**
 * An Enemy character: Feeder (most harmless Enemy)
 *
 * Two types:
 *    Stationary
 *    Moving (turns back if collide with Block)
 *
 * Damage methods:
 *    Self-explode if in contact with Player or Projectile
 *
 * Size:
 *	  There are multiple sizes: small, large
 *    Explosive damage and Speed are based on size of EnemyFeeder
 *
 * Default anchor point: Vec2(0.5, 0)
 */
class EnemyFeeder : public Enemy
{
public:

	enum class FEEDER_TYPE {
		NONE = 0,
		STATIONARY = 1,
		MOVING = 2
	};

	enum class FEEDER_SIZE {
		NONE = 0,
		SMALL = 1,
		LARGE = 2
	};

	~EnemyFeeder();

	// init() function
	bool init();

	// update() function
	void update(float delta) override;

	// Overrides
	bool onContactBegin(const cocos2d::PhysicsContact &contact) override;

	/** Instructs EnemyFeeder to start its action
	 *     IF stationary -> DO NOTHING
	 *     IF moving -> move in the current direction
	 */
	void startAction() override;

	// Sets type for EnemyFeeder: moving or stationary
	void setEnemyFeederType(FEEDER_TYPE type);

	// Sets size for EnemyFeeder: small or large
	void setEnemyFeederSize(FEEDER_SIZE size);

	// Sets initial direction
	void setInitialDirection(DIRECTION direction);

	// ------------------------------- Actions -------------------------------

	void move(Character::DIRECTION direction) override;

	void onDie();

	// Instead of normal attacking methods, EnemyFeeder self-explodes
	void explode();

	// -----------------------------------------------------------------------

	// Type of EnemyFeeder
	CC_SYNTHESIZE_READONLY(FEEDER_TYPE, _feederType, FeederType);

	// Size of EnemyFeeder
	CC_SYNTHESIZE_READONLY(FEEDER_SIZE, _feederSize, FeederSize);

	// EnemyFeeder is exploding or not
	CC_SYNTHESIZE(bool, _isExploding, ExplodingState);

	// (Block) Collision flag
	CC_SYNTHESIZE(bool, _blockCollision, BlockCollision);

	CREATE_FUNC(EnemyFeeder);

protected:

	// Runs on active
	virtual void onEnable() override;

	// Helper init() init fuctions
	void initSprites() override;
	void initPhysics() override;

	// Init projectile (when DIES)
	void initProjectiles();

	// Animation
	cocos2d::ActionInterval* _moveAnimation;
	cocos2d::ActionInterval* _dieAnimation;

	// Projectiles
	SelfExplode* _selfExplode;

};



#endif // !__ENEMY_FEEDER_H__
