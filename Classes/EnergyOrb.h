#ifndef __ENERGY_ORB_H__
#define __ENERGY_ORB_H__

#include "Item.h"


class EnergyOrb : public Item
{
public:

	~EnergyOrb();

	// init() function
	virtual bool init();

	/**
	 * Executes disappearing animation
	 *
	 * Cool effect -> move the orb to the energy bar, and increase number of orbs obtained
	 */
	void disappear();

	// Overrides
	void reset(float params[10]) override;

	CC_SYNTHESIZE_READONLY(bool, _isDisappearing, DisappearState);

	CREATE_FUNC(EnergyOrb);

protected:

	virtual void initPhysics();
	virtual void initSprites();

	// Animation
	cocos2d::ActionInterval* _normalAction;
	cocos2d::ActionInterval* _disappearAction;

};


#endif // !__ENERGY_ORB_H__
