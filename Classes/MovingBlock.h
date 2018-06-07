// moving block 2.0
// moving between start & end point

#ifndef _MOVINGBLOCK_H_
#define _MOVINGBLOCK_H_

#include "Block.h"


// 1 type of moving block, always moves between A and B, whatever start_location
// - move to end_position first
//* dynamic but very strong
class MovingBlock : public Block
{
public:

	virtual ~MovingBlock();

	virtual bool init();

	CREATE_FUNC(MovingBlock);


	// x1, y1 : point_start
	// x2, y2 :point_end
	virtual void init(bool isHidden, bool isOneWay
		, int x1, int y1
		, int x2, int y2
		, int zone_ID);

	/* --- It's time! ---
	* called by zone*/
	void triggered();

	void update(float delta) override;
private:
	void initSprites();

	void updateArrow();

	void updateVelocity();

	cocos2d::Vec2 m_startPoint;
	cocos2d::Vec2 m_endPoint;

	bool m_isToEnd;
	cocos2d::Vec2 m_vecMove;
	float m_speed;

	float m_timer;
	bool m_isOn;

	cocos2d::Sprite* m_arrowSprite;
	cocos2d::SpriteFrame* m_frameUp;
	cocos2d::SpriteFrame* m_frameDown;
	cocos2d::SpriteFrame* m_frameLeft;
	cocos2d::SpriteFrame* m_frameRight;

};
#endif // !_MOVINGBLOCK_H_
