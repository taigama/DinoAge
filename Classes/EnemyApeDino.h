#ifndef __ENEMY_APE_DINO_H__
#define __ENEMY_APE_DINO_H__

#include "Enemy.h"

#include "Charge.h"

#define MIN_ANGLE_RAD_EYESIGHT 0.0f
#define MAX_ANGLE_RAD_EYESIGHT_LEFT 1.3f
#define MAX_ANGLE_RAD_EYESIGHT_RIGHT -1.3f


/**
 * An Enemy character: ape-like dinosaur
 *
 * Courses of actions:
 *    Patrols around two endpoints, constantly looking for Player
 *    If found Player -> charges in that direction
 *    After attacking, stands at place and looks for Player
 *         See nothing -> return back to patrol area
 *         Found Player again -> attack again
 *
 * Damage methods:
 *    Spike  : on its back, inflicts damage when Player jumps on it
 *    Charge : Player is knocked back if being on the way of its charging
 */
class EnemyApeDino : public Enemy
{
public:

	// Default destructor
	~EnemyApeDino();

	// init() function
	bool init() override;

	/**
	 * Sets the starting point and destination point for this object
	 * This enemy will then patrol back and forth around these two points
	 * 
	 * Returns true if these two points are set successfully
	 *
	 * @param start : the starting point (one of the two vertices)
	 * @param end   : the ending point (the other one of the two vertices)
	 */
	bool setPatrolArea(cocos2d::Vec2 start, cocos2d::Vec2 end);

	/**
	 * Instructs object to move back into the previously specified patrol area, then starts the patrol also
	 * Called after object had done looking around for Player (after chargeHit())
	 */
	void moveToPatrolArea();

	// ------------------------ Character action functions -----------------------

	/**
	 * move() function. When this enemy moves, it is attacking at the same time
	 * If player hits it during this period -> lose HP and bounce back
	 *
	 * @param direct : specifies the direction the object is moving
	 *					=> LEFT -> instructs object to move LEFT
	 *					=> RIGHT -> instructs object to move RIGHT
	 *					=> NONE -> instructs object to stops moving
	 */
	void move(Character::DIRECTION direct) override;

	/**
	 * This enemy will attack Player if he/she is in its line of eyesight (raycast)
	 * Method of attacking = Charge attack
	 *
	 * Note: Called only when this object finds Player
	 */
	void attackPhysical() override;

	/**
	 * Makes this enemy patrol back and forth a mong two points,
	 * with a delay time when it reaches each endpoint
	 */
	void patrol();

	/**
	 * Executes actions and effects when object DIES
	 */
	void onDie() override;

	// ----------------------------------------------------------------------

	// Overrides
	void update(float delta) override;
	void startAction() override;

	// Called when object gets hit, or hits something (e.g. by Block, Projectile ...)
	void getHit();

	CREATE_FUNC(EnemyApeDino);

	// Delay time when object reaches each end point
	CC_SYNTHESIZE(float, _delayTime, DelayTime);

	// Flag to check state of moving
	//     True -> still moving
	//     False -> is waiting (at one of two endpoints)
	CC_SYNTHESIZE(bool, _movingFlag, MovingFlag);

	// Attack speed
	CC_SYNTHESIZE(float, _attackSpeed, AttackSpeed);

	// Indicates whether object is "on-the-air" (due to being knocked back)
	CC_SYNTHESIZE(bool, _isFlying, FlyingState);

	// State: looking around finding Player
	CC_SYNTHESIZE(bool, _lookingAround, LookingAroundState);

	// Sprites for question-mark and exclamation-mark
	CC_SYNTHESIZE_READONLY(cocos2d::Sprite*, _questionMark, QuestionMark);
	CC_SYNTHESIZE_READONLY(cocos2d::Sprite*, _exclamationMark, ExclamationMark);

protected:

	// Runs on active
	virtual void onEnable() override;


	// Helper init() functions
	void initPhysics() override;
	void initSprites() override;
	void initProjectiles();

	// Overrides onContactBegan()
	bool onContactBegin(const cocos2d::PhysicsContact &contact);

	// Helper functions for attacking
	void chargeAttack();

	// Helper function for PhysicsRayCastFunc
	bool physicsRayCastFunc(cocos2d::PhysicsWorld& world, const cocos2d::PhysicsRayCastInfo& info, void* data);

private:

	// Animation(s), and action
	cocos2d::Animation* m_animation;
	cocos2d::ActionInterval* m_action;

	// Two endpoints
	cocos2d::Vec2 m_start;
	cocos2d::Vec2 m_end;

	// Projectiles
	Charge* m_charge;

	// Starting and End points for RayCast
	cocos2d::Vec2 m_startRay;
	cocos2d::Vec2 m_endRay;
	float m_angleRad = MIN_ANGLE_RAD_EYESIGHT;	// angle to control the "sweeping radius" of this enemy's eyesight
	int m_factorDeltaAngle = +1;		// Two values: -1 or +1

	// Flag: find Player
	bool m_noticePlayer = false;

	// Flag: in attacking stance
	bool m_attackStance = false;

};

#endif // !__ENEMY_APE_DINO_H__
