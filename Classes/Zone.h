#ifndef __ZONE_H__
#define __ZONE_H__

#include "GameObject.h"
#include "Character.h"

// this is a special type of GameOject, for making event
//*ex: kill player, spawn enemies, triggering block
class Zone : public GameObject
{
public:
	enum ZONE_TYPE
	{
		NONE = 0,
		TELEPORTER = 1,
		SPAWN_ENEMY = 2,
		SPAWN_ITEM = 3,
		DEATH = 4,

		// Activated when Player enters
		//   Spawns Enemy (once)
		//   Trigger moving block when all Enemy dies
		TRIGGERBLOCK = 5,

		NEXTSTAGE = 6
	};

	virtual ~Zone();

	virtual bool init();

	virtual void update(float) override;
	
	// Returns a Zone object with the given size and position
	// * type : enum ZONE_TYPE ; type of the Zone object
	// * x : position x
	// * y : position y
	// * width : width of zone
	// * height : height of zone
	// #### based on type of zone ####
	// * x1 : the trigger X (may be teleport location)
	// * y1 : the trigger Y
	// * value : dmg of death_zone, type of item to be spawned
	// * end_index, start_index: for the zone that hold many enemy, index is for binding enemies
	// * ID is for binding
	// * direction? ex: death zone direction
	static Zone* createZone(ZONE_TYPE type
		, float x, float y, float width, float heigh
		, float x1, float y1
		, int start_index, int end_index
		, int i_value, int ID, int direction);

	// initialize object, based on inherited object
	virtual void init(float x1, float y1
		, int start_index, int end_index
		, int i_value, int ID, int direction);


	// Callback function for when the collision begins
	virtual bool onContactBegin(const cocos2d::PhysicsContact &contact) override;
	//// Callback function during the contact, but before collision is resolved
	//virtual bool onContactPresolve(const cocos2d::PhysicsContact &contact) override;
	// Callback function after separate
	virtual bool onContactSeparate(const cocos2d::PhysicsContact &contact) override;


	CREATE_FUNC(Zone);

	CC_SYNTHESIZE_READONLY(ZONE_TYPE, _zoneType, ZoneType);
	CC_SYNTHESIZE_READONLY(int, _ID, ID);
protected:
	
	// zone and block have no "initPhysics", they init in createZone & createBlock
	// zone doesn't need sprite

	// using in createZone()
	virtual void setColliderBitMask();

	
	// when player enter the zone
	virtual void onPlayerEntered(Character*);
	//// when player keeping inside the zone
	//virtual void onPlayerKeepInside(Character*);
	// when player exit the zone
	virtual void onPlayerExit(Character*);

	//virtual void reset(float params[10]) override;

};
#endif // !__ZONE_H__
