#ifndef __CLAW_ATTACK_H__
#define __CLAW_ATTACK_H__

#include "Projectile.h"


/**
 * ClawAttack for EnemyWerewolf
 *
 * Type: Physical Attack
 */
class ClawAttack : public Projectile
{
public:

	~ClawAttack();

	// init() function
	bool init() override;

	// Manipulates effect for ClawAttack
	void showEffects();
	void hideEffects();

	CREATE_FUNC(ClawAttack);

protected:

	// Helper functions for init()
	void initSprites() override;
	void initPhysics() override;

	// Inflicts damage when hits Character and executes some effects (if any)
	void attack(Character* target, Character::DIRECTION direction) override;

};


#endif // !__CLAW_ATTACK_H__
