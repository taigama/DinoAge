#ifndef _GROUNDSPRITE_H_
#define _GROUNDSPRITE_H_

#include "Block.h"

// a block that only visible on touch
class GroundSprite : public Block
{
public:

	virtual ~GroundSprite();

	virtual bool init();
	virtual void update(float delta) override;

	CREATE_FUNC(GroundSprite);

	/*virtual bool onContactBegin(const cocos2d::PhysicsContact &contact) override;
	virtual bool onContactSeparate(const cocos2d::PhysicsContact &contact) override;*/
	void init(bool isHidden, bool isOneWay
		, int x1, int y1
		, int x2, int y2
		, int zone_ID);
private:
	//void initPhysics() override;
	void initSprites() override;

	bool m_isFlash;
	float m_timerFLash;

	int m_countCharTouch;
};
#endif // !_GROUNDSPRITE_H_
