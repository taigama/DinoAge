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


protected:


private:
	
	std::vector<SpeechModel*>* m_currentModel;
	cocos2d::Camera* m_camera;

};

#endif // !__SPEECHER_H__
