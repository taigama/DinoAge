#ifndef __ROLL_ATTACK_H__
#define __ROLL_ATTACK_H__

#include "Projectile.h"


/**
 * RollAttack - Means of attack of EnemyRamus
 *
 * On contact with team Player -> inflicts damage and pushes Player UPWARD
 *
 * Attached to EnemyRamus -> DO NOT need to use PoolManage to handle
 */
class RollAttack : public Projectile
{
public:

	~RollAttack();

	// init() function
	bool init() override;

	/**
	 * Called manually by EnemyRamus to initialize PhysicsBody for this Projectile
	 */
	void setPhysics();

	CREATE_FUNC(RollAttack);

protected:

	/**
	 * Implements its own method of attack:
	 *    Regardless of the direction that Player hit this Projectil
	 *    -> Always pushes Player UPWARD
	 */
	void attack(Character* target, Character::DIRECTION direct) override;

};


#endif // ! __ROLL_ATTACK_H__
