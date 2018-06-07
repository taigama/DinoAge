#ifndef __ENEMY_H__
#define __ENEMY_H__

#include "Character.h"

#include "Zone.h"


/**
 * ABSTRACT CLASS: Holds template methods for all discrete subclasses of Enemy
 * DO NOT TRY TO CREATE OBJECT OF TYPE Enemy CLASS
 *
 * NOTE: Default anchor point for Enemy -> Vec2(0.5, 0)
 */
class Enemy : public Character
{
public:

	~Enemy();

	// init() function
	virtual bool init();

	/**
	 * Returns the content size of the object (boundingBox == contentSize)
	 *
	 * NOTE: this is the bounding box of the OBJECT (NODE), not the SPRITE in it
	 * NOTE: just use for the SIZE (since the origin is not correctly set)
	 */
	cocos2d::Rect getBoundingBox() const override;

	// Overrides
	virtual bool onContactBegin(const cocos2d::PhysicsContact &contact);

	// ------------------------ Basic Actions ---------------------------

	/**
	 * In general: instructs the object to move in the specified direction
	 * To stop -> sends DIRECTION::NONE as argument
	 * @param direct : specified direction for the object to move
	 *
	 * NOTE: this move() method sets the velocity, state, and side (x-scale)
	 *		 for Enemy character. All inherited classes have to setup their
	 *		 additional states as well as animation manually
	 */
	void move(Character::DIRECTION direct) override;

	/**
	 * Instructs the object to jump
	 *
	 * NOTE: the jump() methods set the velocity and state for obect. All animations
	 *		 required have to be implemented by the corresponding inherited classes
	 */
	void jump() override;

	/**
	 * Basic attacks functions (both by physical or projectile)
	 *
	 * Each inherited class has to write its own implementation for these two actions
	 */
	void attackPhysical() override;
	void attackProjectile() override;

	/**
	* Overrides onHit() function on Character class
	* @param dmg   : specifies the amount of damage that character has to endure
	* @param dir   : the direction in which the object faces damage source (e.g. Projectile, Source ...)
	* @param force : the force that the power source affect the character
	*
	* Note: for all subclasses of Enemy -> objects will not be knocked back on hit.
	*		Only some special cases have that kind of behaviour.
	*/
	virtual void onHit(float dmg, DIRECTION dir, float force);

	/**
	* Called by "onHit()" function when object dies
	* In general: carries out a general dying animation for all subclasses of Enemy
	* For classes that have their own animation for dying -> provide their own implementations (override this function then)
	*
	* Additional feature:
	*	Knock Enemy up when it dies
	*	Drop LEGACY (based on enemy's type) and ENERGY ORB (with rate)
	*/
	virtual void onDie();

	/**
	* Knocks the object back in the opposite of the given direction
	* @param direction		: the direction in which object is hit (object will be knocked in the opposite direction of this)
	* @param forceFactor	: the force factor to push object back (actual_force = force_factor * base_force)
	*/
	void knock(Character::DIRECTION direction, float forceFactor);

	/**
	* Turns the Enemy to the opposite of direction
	*/
	void turnAround();

	// ----------------------------------------------------------------------

	/**
	 * Instructs Enemy to start its own action
	 */
	virtual void startAction() { };

	/** Drops Legacy (Gold) when Enemy dies
	 *		Called in onDie() function
	 */
	void dropLegacy();

	// Required time for hurting animation of the object
	CC_SYNTHESIZE_READONLY(float, _hurtTime, HurtTime);

	// Required time for dying animation of the object
	CC_SYNTHESIZE_READONLY(float, _dieTime, DieTime);

	// Number of Gold dropped after Enemy dies
	CC_SYNTHESIZE_READONLY(int, _numLegacy, NumLegacy);

	// [TriggerBlockZone] Zone that manages this Enemy
	CC_SYNTHESIZE(Zone*, _zone, Zone);

	// Knocking back state
	CC_SYNTHESIZE(bool, _knockedBack, KnockedBack);

	// Hitting ground state (after being knocked back)
	CC_SYNTHESIZE(bool, _hitGroundKB, HitGroundKB);

};


#endif // !__ENEMY_H__
