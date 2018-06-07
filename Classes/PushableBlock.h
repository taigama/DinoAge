#ifndef _PUSHABLEBLOCK_H_
#define _PUSHABLEBLOCK_H_

#include "Block.h"

// a type of block that let the player pushing
// push-able block has 0 friction
class PushableBlock : public Block
{
public:

	virtual ~PushableBlock();

	virtual bool init();

	CREATE_FUNC(PushableBlock);

	virtual void update(float delta) override;

	void push(float x_vecPush);

	// x1, y1, x2, y2 : based on type | please see the inherited classes
	virtual void init(bool isHidden, bool isOneWay
		, int x1, int y1
		, int x2, int y2
		, int zone_ID);

protected:
	void setColliderBitMask() override;

	

private:
	//void initPhysics() override;
	void initSprites() override;

	cocos2d::Vec2 m_vecPush;
	float m_xPush;
};
#endif // !_MOVINGBLOCK_H_
