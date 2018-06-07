#ifndef __ENEMY_RAMUS_H__
#define __ENEMY_RAMUS_H__

#include "Enemy.h"

#include "RollAttack.h"


/**
 * An Enemy character: Ramus
 *
 * Courses of actions:
 *    Rolling between two endpoints repeatedly
 *
 * Damage methods:
 *    Roll : Player is knocked UPWARD if is on the way of its rolling
 */
class EnemyRamus : public Enemy
{
public:

	~EnemyRamus();

	// init() function
	bool init();

	/**
	* Sets the starting point and destination point for "rolling"
	*
	* Returns true if these two points are set successfully
	*
	* @param start : the starting point (one of the two vertices)
	* @param end   : the ending point (the other one of the two vertices)
	*/
	bool setPatrolArea(cocos2d::Vec2 start, cocos2d::Vec2 end);

	void update(float delta);

	// ---------------------- Actions ----------------------

	/**
	 * EnemyRamus will not have "normal" move action
	 *
	 * Instead, it will roll from one side to another
	 * as a mean of attacking
	 *
	 * Calls move(DIRECTION::NONE) to stops rolling
	 */
	void move(Character::DIRECTION direction);

	/**
	 * EnemyRamus will patrol around two endpoints (actually, roll)
	 * and inflicts damages on Player in the way (and Player also is bounced upward)
	 */
	void attackPhysical();

	/**
	 * Helper functions for attackPhysical()
	 * rollAttack() -> rolling in the current direction
	 * prepareAttack() -> EnemyRamus wraps into rolling position; prepares for attack
	 * returnToIdle() -> EnemyRamus returns to idle state after rolling
	 */
	void rollAttack();
	void prepareAttack();
	void returnToIdle();

	// -----------------------------------------------------

	/**
	 * If EnemyRamus is on attacking stance (is rolling) -> reduces damage taken by half
	 */
	void onHit(float dmg, DIRECTION dir, float force) override;

	void onDie();

	void startAction() override;

	// Get/Set methods for attacking flag
	inline bool isAttack() { return _isAttacking; };
	inline void setAttackingState(bool state) { _isAttacking = state; };

	// Flag: turn state after attack
	CC_SYNTHESIZE(bool, _hasTurn, TurnState);

	CREATE_FUNC(EnemyRamus);

protected:

	// Helper functions for init
	void initPhysics() override;
	void initSprites() override;
	void initProjectiles();

	bool onContactBegin(const cocos2d::PhysicsContact &contact);

	// Current animation-action
	cocos2d::ActionInterval* _action;

	// Idle animation
	cocos2d::ActionInterval* _idleAnimation;
	cocos2d::ActionInterval* _returnIdleAnimation;		// after rolling -> change back to idle state

	// Attack animation(s)
	cocos2d::ActionInterval* _rollingAnimation;			// attack by rolling
	cocos2d::ActionInterval* _prepareAnimation;			// prepare before rolling

	// Two endpoints
	cocos2d::Vec2 _start;
	cocos2d::Vec2 _end;

	// Projectile for EnemyRamus -> RollAttack
	RollAttack* _rollProjectile;

	// Flag: in attack state or not
	bool _isAttacking = false;

	// Flags: collide left / right
	bool _collideLeft = false;
	bool _collideRight = false;

};

#endif // !__ENEMY_RAMUS_H__
