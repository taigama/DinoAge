#ifndef __CHARGE_H__
#define __CHARGE_H__

#include "Projectile.h"

/**
 * Attack for EnemyApeDino
 */
class Charge : public Projectile
{
public:

	~Charge();

	// init() function
	bool init() override;

	// Called manually by EnemyApeDino, AFTER setting the size for this projectile
	void setPhysics();

	CREATE_FUNC(Charge);

protected:

	// Inflicts damage to character + carries out effect
	void attack(Character* target, Character::DIRECTION direct) override;
};

#endif // !__CHARGE_H__