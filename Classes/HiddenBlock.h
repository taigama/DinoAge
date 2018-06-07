#ifndef _HIDDENBLOCK_H_
#define _HIDDENBLOCK_H_

#include "Block.h"

// a block that only visible on touch
class HiddenBlock : public Block
{
public:

	virtual ~HiddenBlock();

	virtual bool init();
	virtual void update(float delta) override;

	void init(bool isHidden, bool isOneWay
		, int x1, int y1
		, int x2, int y2
		, int zone_ID);

	CREATE_FUNC(HiddenBlock);

	virtual bool onContactBegin(const cocos2d::PhysicsContact &contact) override;
	virtual bool onContactSeparate(const cocos2d::PhysicsContact &contact) override;

private:
	void initPhysics() override;
	void initSprites() override;

	bool m_isFlash;
	float m_timerFLash;

	int m_countCharTouch;
};
#endif // !_HIDDENBLOCK_H_
