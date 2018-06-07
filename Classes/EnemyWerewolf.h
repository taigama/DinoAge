#ifndef __ENEMY_WEREWOLF_H__
#define __ENEMY_WEREWOLF_H__

#include "Enemy.h"

#include "ClawAttack.h"


/**
 * An Enemy character : Werewolf
 *
 * Courses of actions:
 *    Stands idly at one place
 *    If Player is within its sight -> moves towards and attacks
 *    If do not see Player for a while -> returns to initial position
 *    [TBC] EnemyWerewolf can only be damaged from behind
 *
 * Damage methods:
 *    Claw : attacks physically when gets near Player
 *
 * Default anchor point (DO NOT CHANGE THIS UNLESS VERY NECESSARY): Vec2(0.5, 0)
 */
class EnemyWerewolf : public Enemy
{
public:

	~EnemyWerewolf();

	// init() function
	bool init();

	void update(float delta) override;

	// Sets (initial) pivot position and facing direction
	void setPivotPosition(cocos2d::Vec2 pos, Character::DIRECTION initialDirection = DIRECTION::LEFT);

	// -------------------------- Actions ----------------------------

	/**
	 * Instructs EnemyWerewolf to move toward the specified direction
	 *
	 * If direction == DIRECTION::NONE => STOPS moving
	 *
	 * @param direction : the direction character has to move in
	 */
	void move(Character::DIRECTION direction) override;

	/**
	 * EnemyWerewolf when attacks (physically) when gets near Player
	 */
	void attackPhysical() override;

	// Overrides
	void onDie() override;
	void onHit(float dmg, DIRECTION dir, float force) override;

	// ---------------------------------------------------------------

	// Helper function to manipulate number of turn around actions that have been made
	void increaseTurnAction();
	void resetTurnActions();

	// Helper function to reset start action (used when EnemyWerewolf returns to pivot position)
	void resetStartAction();

	// Found Player status
	CC_SYNTHESIZE(bool, _foundPlayer, FoundPlayer);

	// Turn_around_scheduler has been setup or not
	CC_SYNTHESIZE(bool, _hasTurnScheduler, TurnAroundScheduler);

	// Attacking state
	CC_SYNTHESIZE(bool, _isAttack, AttackingState);

	// Indicates whether EnemyWerewolf is returning to its pivot position
	CC_SYNTHESIZE(bool, _isReturning, ReturningState);

	// Number of turn around actions can be made before returning to pivot position
	CC_SYNTHESIZE_READONLY(int, _numTurns, NumTurns);

	CREATE_FUNC(EnemyWerewolf);

protected:

	// Helper init() functions
	void initSprites() override;
	void initPhysics() override;
	void initProjectiles();

	// Helper function used with raycast (in physics engine)
	bool physicsRayCastFunc(cocos2d::PhysicsWorld& world, const cocos2d::PhysicsRayCastInfo& info, void* data);

	// Pivot position: EnemyWerewolf will return here if NOT seeing Player for a while
	// Pivot direction: EnemyWerewolf will face this direction when returns to pivot position
	cocos2d::Vec2 _pivotPosition;
	Character::DIRECTION _pivotDirection;

	// Animations
	cocos2d::ActionInterval* _walkAnimation;
	cocos2d::ActionInterval* _attackAnimation;

	// Properties for raycasting
	cocos2d::Vec2 _startRayHigh;
	cocos2d::Vec2 _startRayLow;
	cocos2d::Vec2 _distance;

	// List of PhysicsBodies deteced by raycast
	cocos2d::Vector<cocos2d::PhysicsBody*> _bodies;

	// Flag: starts EnemyWerewolf action? (see Player for the first time -> start action)
	bool _startAction = false;

private:

	// Projectile
	ClawAttack* m_clawAttack;

};



#endif // !__ENEMY_WEREWOLF_H__
