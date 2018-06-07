#ifndef __BLOCK_H__
#define __BLOCK_H__

#include "GameObject.h"

#define BLOCK_DENSITY 1.0f
#define BLOCK_FRICTION 1.0f

// the type of GameObject that prevent player from moving (may pass through in some special case)
class Block : public GameObject
{
public:
	enum BLOCK_TYPE
	{// this info is for making map
		NONE = 0,
		STATIC = 1,

		// x1, y1 : point_start, x2, y2 : point_end
		// zone_ID : tagged zone trigger this, =0 if no tagged zone
		//* dynamic but very strong
		MOVING = 2,
		// 1 type of moving block, always moves between A and B
		
		// another type of moving block, start_point is the start_location
		// x1, y1 : point_end, x2: zoneID
		//* dynamic but very strong
		//MOVING_BY_TRIGGER = 4,// point_start at the start position
		// the zone will trigger this block, to make this block move
		GROUND_SPRITE = 4,

		// player can push this
		PUSHABLE = 5,		// set dynamic = true

		// not implemented yet
		DESTRUCTIBLE = 6,

		// will flash when hitted or show when player are standing on it
		HIDDEN = 7,

		// x1 : start_delay, y1 : time_interval
		REPEAT = 8
		// this type is: blocks spawned from start_location, moving to end_location (x2, y2) and disappear; 
	};

	virtual cocos2d::Rect getBoundingBox() const override;

	virtual ~Block();

	virtual bool init();


	// x1, y1, x2, y2 : based on type | please see the inherited classes
	virtual void init(bool isHidden, bool isOneWay
		, int x1, int y1
		, int x2, int y2
		, int zone_ID);

	virtual void update(float) override;


	// Create a BLOCK from parameters
	// type : type of block
	// x, y : center position of block
	// width, height : size of the block
	// isHidden, isOneWay <bool>
	// x1, y1, x2, y2 : based on type
	static Block* createBlock(BLOCK_TYPE type
		, float x, float y, float width, float height
		, bool isHidden, bool isOneWay
		, int x1, int y1
		, int x2, int y2
		, int zone_ID);


	//static Block* createPoly(float x, float y, cocos2d::Vec2* points, int count);

	// size of block
	//cocos2d::Rect* rect;
	cocos2d::Size size;

	

	// Callback function for when the collision begins
	virtual bool onContactBegin(const cocos2d::PhysicsContact &contact) override;
	//virtual bool onContactSeparate(const cocos2d::PhysicsContact &contact) override;

	CREATE_FUNC(Block);

	// _blockType : enum BLOCK_TYPE
	// To get -> getBlockType()
	CC_SYNTHESIZE_READONLY(BLOCK_TYPE, _blockType, BlockType);

	// is it a hidden block?
	CC_SYNTHESIZE_READONLY(bool, _isHidden, Hidden);
	// is this a one way platform
	CC_SYNTHESIZE_READONLY(bool, _isOneWay, OneWay);

	CC_SYNTHESIZE(int, _team, Team);

protected:
	

	// zone and block have no "initPhysics", they init in createZone & createBlock

	// using in createBlock()
	virtual void setColliderBitMask();

	void convertTopLeftToMid(int &x, int &y, const float &width, const float &height);

};

#endif // !__BLOCK_H__
