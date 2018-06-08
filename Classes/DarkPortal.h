#ifndef __DARKPORTAL_H__
#define __DARKPORTAL_H__

#include "Enemy.h"


/**
 * ABSTRACT CLASS: Holds template methods for all discrete subclasses of Enemy
 * DO NOT TRY TO CREATE OBJECT OF TYPE Enemy CLASS
 *
 * NOTE: Default anchor point for Enemy -> Vec2(0.5, 0)
 */
class DarkPortal : public Enemy
{
public:

	~DarkPortal();

	// init() function
	bool init();

	void update(float) override;

	// Overrides
	void onHit(float dmg, DIRECTION dir, float force);
	void onDie();

	CREATE_FUNC(DarkPortal);

protected:

	// Helper functions for init()
	virtual void initSprites() override;
	virtual void initPhysics() override;

private:

	void endGame();

	// Timer to spawn Enemy (Feeder / Werewolf)
	float m_timerSpawn;

	// Timer to cast (NOT implemented)
	float m_timerCast;

	// NOT implemented
	bool m_isRage;

	cocos2d::Vec2 m_posSpawn;
};


#endif // !__DARKPORTAL_H__
