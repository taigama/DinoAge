#ifndef __SPIKE_H__
#define __SPIKE_H__

#include "Projectile.h"

/**
 * Attack for EnemyApeDino (attached to upper body)
 *
 *
 * Note: DO NOT need to use pool to handle this
 */
class Spike : public Projectile
{
public:

	~Spike();

	// init() function
	bool init() override;

	// Called manually by EnemyApeDino, AFTER setting the size for this projectile
	void setPhysics();

	CREATE_FUNC(Spike);

protected:

	// Inflicts damage to character + carries out effect
	void attack(Character* target, Character::DIRECTION direct) override;

};

#endif // !__SPIKE_H__
