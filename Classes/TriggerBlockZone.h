#ifndef __TRIGGERBLOCKZONE_H__
#define __TRIGGERBLOCKZONE_H__

#include "Zone.h"

// Zone for spawning enemies at first time and triggering block
// - contain zone_ID
// * spawn enemy: the enemies must be created first, this zone just make it appear on game at the right time
// * all the creeps died, this zone trigger the tagged block
// ............................. using ..................
// for loading enemies:
// - find zone_ID in World->zones
// - call zone->addCreep
// - creep died, call zone->notifyCreepDie
// for blocks:
// - find zone_ID in World->zones
// - zone-> setBlockTrigger (the block)
class TriggerBlockZone : public Zone
{
public:
	

	virtual ~TriggerBlockZone();

	virtual bool init() override;

	CREATE_FUNC(TriggerBlockZone);

	//virtual void init(float x1, float y1, int start_index, int end_index, int i_value, int ID) override;

	/** Used to notify the Zone when an Enemy under its management dies
	 *	Called by Enemy
	 */
	void notifyCreepDie(GameObject* creep);

	/* Tell this zone: "this enemy is now under your control!"
	* called by Enemy too, when the enemy is in initialization*/
	void addCreep(GameObject* creep, float time);

	CC_SYNTHESIZE(GameObject*, _blockTrigger, BlockTrigger);





	// when player enter the zone
	virtual void onPlayerEntered(Character*) override;

	// zone no need to put on pool manager
	//virtual void reset(float params[10]) override;
private:

	// Two Vectors holding Enemy (inside the current Zone) and its delay-time-spawn
	cocos2d::Vector<GameObject*> m_creeps;
	std::vector<float> m_delayTimeSpawn;

	int m_creepCount;
	int m_creepAlive;
};
#endif // !__TRIGGERBLOCKZONE_H__
