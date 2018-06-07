#ifndef __SELF_EXPLODE_H__
#define __SELF_EXPLODE_H__

#include "Projectile.h"


/**
 * Attack for EnemyFeeder
 * 
 * When EnemyFeeder dies / on contact with Player -> self-explode
 */
class SelfExplode : public Projectile
{
public:

	~SelfExplode();

	// init() function
	bool init() override;

	/**
	 * Sets type of explosion: small or large
	 *    1 - small
	 *    2 - large
	 */
	void setExplosionType(int size);

	// Helper function. Called after explosion type has been set
	void setPhysics();

	CREATE_FUNC(SelfExplode);

protected:

	// Inflicts damage
	void attack(Character* target, Character::DIRECTION direction);

};


#endif // !__SELF_EXPLODE_H__
