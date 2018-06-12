#ifndef __SPEECHER_H__
#define __SPEECHER_H__

#include "cocos2d.h"

#include "Player.h"

#if (CC_TARGET_PLATFORM == CC_PLATFORM_LINUX)
		#include "ui/CocosGUI.h"
#else
		#include "ui\CocosGUI.h"
#endif

#define PLAYER_HEALTH_THRESHOLD_HURT 0.5f
#define PLAYER_HEALTH_THRESHOLD_SERIOUS_HURT 0.2f

class SpeechModel;
class ResourceManager;

class Speecher : public cocos2d::Node
{
public:
	~Speecher();

	// Called in "create()" function
	virtual bool init();

	// ----------------------------------------------------------------------------------------------

	// Overrides the "update()" function (each frame)
	virtual void update(float delta) override;

	// [Temporarily] Hides/Unhides HUDLayer's components
	void hideSpeecher();
	void showSpeecher();


	// Implements the "static create()" method manually
	CREATE_FUNC(Speecher);

	void activeSpeech(const std::string& fileName);
	void loadModels();


	bool onTouch(cocos2d::Touch *touch, cocos2d::Event *event);


private:
	
	std::vector<SpeechModel*>* m_currentModels;
	std::vector<SpeechModel*>::iterator m_currentIter;
	cocos2d::Camera* m_camera;
	cocos2d::Vec2 m_visibleOrigin;
	cocos2d::Size m_visibleSize;

	void showComponent();
	void hideComponent();

	cocos2d::LayerColor* m_background;
	cocos2d::Sprite* m_avaLeft;
	cocos2d::Sprite* m_avaRight;
	cocos2d::Label* m_lbNameLeft;
	cocos2d::Label* m_lbNameRight;
	cocos2d::Label* m_txtLeft;
	cocos2d::Label* m_txtRight;

	void addTouchListener();
	void next();

	float m_duration;
	ResourceManager* m_resManager;
	cocos2d::EventListenerTouchOneByOne* m_listener;
};

#endif // !__SPEECHER_H__
