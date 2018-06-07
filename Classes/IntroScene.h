#ifndef __INTRO_SCENE_H__
#define __INTRO_SCENE_H__

#include "cocos2d.h"

#if (CC_TARGET_PLATFORM == CC_PLATFORM_LINUX)
		#include "ui/CocosGUI.h"
#else
		#include "ui\CocosGUI.h"
#endif

#include "Projectile.h"

class IntroScene : public cocos2d::Layer
{
public:

	~IntroScene();

	static cocos2d::Scene* createScene();

	virtual bool init();


	bool loading();


	void changeScene();

	// For ui::Button
	void onSkipCallback(cocos2d::Ref* sender, cocos2d::ui::Widget::TouchEventType type);

	

	CREATE_FUNC(IntroScene);

protected:

	// List holding all available levels
	cocos2d::Vector<cocos2d::Sprite*> _listLevels;

	// Index to keep track of the current level shown (in level selection panel)
	// Note: this index is reset everytime Level Selection layer is shown
	int _currentLevel = 0;

	// Needed for touch event listener
	bool _isTouchDown = false;
	float startTouch[2];
	float endTouch[2];

	// Skills pattern for each Character
	int _redDinoSkillsPattern = 24;				// default = 0x11000
	int _whiteDinoSkillsPattern = 24;
	int _yellowDinoSkillsPattern = 24;

	// Current LEGACY of Player
	int _currentLegacy = 0;

	// Flag for saving state: whether the skills-pattern + player's legacy
	// are saved into UserDefault or not
	bool _hasSaved = false;

private:
	cocos2d::Vec2 m_center;
	cocos2d::Vec2 m_origin;

	float m_scaleFactor;

	cocos2d::Size m_visibleSize;
	int m_soundID;
};

#endif // !__INTRO_SCENE_H__
