#ifndef __ENERGY_BAR_H__
#define __ENERGY_BAR_H__

#include "cocos2d.h"


/**
 * Indicates the current energy level of Player. The energy
 * bar is displayed as number of energy orbs obtained
 *
 * Maximum capacity: FIVE (5) energy orbs
 *
 * Player can obtain energy orb through killing Enemy, 
 * or through Item distributed across the map
 *
 * NOTE: For easier retrieval -> implement EnergyBar as a SINGLETON
 */
class EnergyBar : public cocos2d::Node
{
public:

	~EnergyBar();

	// Retrieves the Singleton instance of EnergyBar
	static EnergyBar* createEnergyBar();

	// init() function
	bool init();

	/**
	 * Increases / Descrease energy bar
	 * @param nums : number of orbs being added / removed
	 */

	/**
	 * Increases the energy level
	 *
	 * In addition, shows an effect of the energy ball running to the energy bar
	 *
	 * @param position : current position of the energy ball being obtained
	 */
	void increaseEnergy(cocos2d::Vec2 position);

	/**
	 * Descreases the energy level by the specified amount of orbs
	 * @param nums : number of orbs to be excluded from the current energy level
	 *
	 * NOTE: Helper functions for activateAdvancedSkill() and activateUltimateSkill()
	 */
	void decreaseEnergy(int nums);

	/**
	 * Decreases the number of orbs required to activate the corresponding skill
	 * Returns true if activate successfully; false otherwise
	 */
	bool activateAdvancedSkill();
	bool activateUltimateSkill();

	bool isFull();

	// The current number of energy orbs
	CC_SYNTHESIZE(int, _currentOrbs, CurrentOrbs);

	// Current state of EnergyBar: IF is updating -> delay all incoming increase/decrease
	CC_SYNTHESIZE(bool, _isUpdating, UpdatingState);

	// The maximum number of energy orbs
	CC_SYNTHESIZE_READONLY(int, _maxOrbs, MaxOrbs);

	CREATE_FUNC(EnergyBar);

private:

	// Animation when increase energy level
	cocos2d::ActionInterval* _updateAnimation;

};



#endif // !__ENERGY_BAR_H__
