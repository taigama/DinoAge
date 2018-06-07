#ifndef __HUD_LAYER_H__
#define __HUD_LAYER_H__

#include "cocos2d.h"

#include "Player.h"

#if (CC_TARGET_PLATFORM == CC_PLATFORM_LINUX)
		#include "ui/CocosGUI.h"
#else
		#include "ui\CocosGUI.h"
#endif

#define PLAYER_HEALTH_THRESHOLD_HURT 0.5f
#define PLAYER_HEALTH_THRESHOLD_SERIOUS_HURT 0.2f


class HUDLayer : public cocos2d::Layer
{
public:
	~HUDLayer();

	// Returns a HUDLayer object
	static cocos2d::Layer* createLayer();

	// Called in "create()" function
	virtual bool init();

	// ------------------------------- Selector Callbacks -------------------------------------

	// For Keyboard EventListener
	void onKeyPressed(cocos2d::EventKeyboard::KeyCode keycode, cocos2d::Event* event) override;
	void onKeyReleased(cocos2d::EventKeyboard::KeyCode keycode, cocos2d::Event* event) override;

	// For ui::Button
	void onPauseCallback(cocos2d::Ref*, cocos2d::ui::Widget::TouchEventType type);
	void onResumeCallback(cocos2d::Ref*, cocos2d::ui::Widget::TouchEventType type);
	void onMenuCallback(cocos2d::Ref*, cocos2d::ui::Widget::TouchEventType type);

	// ----------------------------------------------------------------------------------------------

	// Overrides the "update()" function (each frame)
	virtual void update(float delta) override;

	// Helper function to increase and reset _counterClick
	inline void increaseCounterClick() { ++_counterClick; };
	inline void resetCounterClick() { _counterClick = 0; };

	/**
	 * Shows the given message floating above Player (and gradually disappear)
	 * Used to notify Player of some events or behaviors
	 * @param message : the given string to display
	 */
	void showNotification(std::string message);

	// Pauses / Resumes EventListener of all HUDLayer's components
	void pauseAllEventListener();
	void resumeAllEventListener();

	// [Temporarily] Hides/Unhides HUDLayer's components
	void hideHUDLayer();
	void unhideHUDLayer();

	// Extract Player (from World) and also its skills-unlock pattern
	void extractPlayer();

	/**
	 *	Determines whether a skill is unlocked or not
	 *	Returns true if the skill has already been unlocked. False otherwise
	 *	@param index	: index of the skill (5 skills in total => from index 0 to 4)
	 */
	bool isSkillUnlocked(int index);

	// Flag -> indicates whether a calback is being triggered or not
	CC_SYNTHESIZE(bool, _triggerCallback, TriggerCallbackState);

	// Flags that help execute advanced skills
	CC_SYNTHESIZE(bool, _advancedSkill01, AdvancedSkill01);
	CC_SYNTHESIZE(bool, _advancedSkill02, AdvancedSkill02);

	// Player
	CC_SYNTHESIZE(Player*, _player, Player);

	// Flag: state { moving , stop } of Player
	CC_SYNTHESIZE(bool, _playerMovingState, PlayerMovingState);

	// Counter to help activate ultimate skill
	CC_SYNTHESIZE_READONLY(int, _counterClick, CounterClick);

	// Implements the "static create()" method manually
	CREATE_FUNC(HUDLayer);

protected:

	// "Virtual" buttons
	cocos2d::ui::Button* _leftButton;
	cocos2d::ui::Button* _rightButton;
	cocos2d::ui::Button* _jumpButton;
	cocos2d::ui::Button* _physicalAttackButton;
	cocos2d::ui::Button* _projectileAttackButton;

	// Components for pausing the game
	cocos2d::LayerColor* _shadowLayer;
	cocos2d::ui::Button* _resumeButton;
	cocos2d::ui::Button* _menuButton;
	cocos2d::ui::Button* _pauseButton;

	// Skills unlock pattern for selected Character
	int _unlockSkillPattern = 0;

private:

	cocos2d::Camera* m_camera;

};

#endif // !__HUD_LAYER_H__
