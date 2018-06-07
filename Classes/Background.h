#ifndef __BACKGROUND_H__
#define __BACKGROUND_H__

#include "cocos2d.h"

/* Class handling ingame-background
has 3 layer (but only 1 was implemented)
* layer _back: moving with camera (speed * 1.0f)*/
class Background : public cocos2d::Node
{
public:

	virtual ~Background();

	CREATE_FUNC(Background);
	
	/**
	 * Initializes the class ; called automatically by "create" function
	 */
	virtual bool init();

	/**
	 * Called every frame if "scheduleUpdate()" is callled and the node is "live"
	 * Overrides the "update()" function of Cocos2d
	 * @param delta : float; time in seconds between each frame
	 */
	virtual void update(float) override;

	// init new background with strings of paths
	void initLayer(std::string back, std::string mid, std::string front);

	CC_SYNTHESIZE_READONLY(cocos2d::Sprite*, _back, BackSprite);
	CC_SYNTHESIZE_READONLY(cocos2d::Sprite*, _mid, MidSprite);
	CC_SYNTHESIZE_READONLY(cocos2d::Sprite*, _front, FrontSprite);

	
/*
private:
	cocos2d::Camera* m_camera;*/
private:
	cocos2d::Vec2 m_backPos;

	cocos2d::Vec2 m_midMap;

	float m_scaleFactor;
};

#endif // !__BACKGROUND_H__
