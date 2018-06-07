#ifndef __CHARACTER_SELECTION_SCENE_H__
#define __CHARACTER_SELECTION_SCENE_H__

#include "cocos2d.h"
#include "Character.h"
#include "IconSkill.h"

#if (CC_TARGET_PLATFORM == CC_PLATFORM_LINUX)
		#include "ui/CocosGUI.h"
#else
		#include "ui\CocosGUI.h"
#endif

#include "IconSkill.h"

#define TIME_TRANSITION_PLAY_SCENE 1.0f


class CharacterSelectionScene : public cocos2d::Layer
{

public:

	~CharacterSelectionScene();

	enum SELECTION { RED_DINO, WHITE_DINO, YELLOW_DINO };

	/**
	 *	Creates Character-Selection scene with default starting map: prologue_tutorial.tmx
	 */
	static cocos2d::Scene* createScene();

	/**
	 *	Creates Character-Selection scene with the specified starting map
	 *	@param startMap	: specified starting map
	 */
	static cocos2d::Scene* createScene(std::string startMap);
	

	virtual bool init();

	virtual void update(float delta);

	// --------------------- Selector callback(s) ----------------------

	// For ui::Button
	void onStartCallback(cocos2d::Touch* touch, cocos2d::Event* event);
	void onMenuCallback(cocos2d::Ref* sender, cocos2d::ui::Widget::TouchEventType type);

	// -----------------------------------------------------------------

	// Helps avoid multiple clicks on a button -> trigger same action multiple times
	bool movingScene = false;

	// Helper function
	void hideCharSelectionScene();

	/**
	 * Changes the displayed skillset based on the currently selected character
	 */
	void changeSkillSet();

	/**
	 * Displays the description of the given skill
	 * @param skill : selected skill
	 */
	void displayDescription(IconSkill* skill);

	/**
	 *	Hides the description panel
	 */
	void hideDescription();

	/**
	 *	Sets starting map
	 */
	void setStartMap(std::string startMap);

	// Currently selected IconSkill
	CC_SYNTHESIZE_READONLY(IconSkill*, _selectedIcon, SelectedIcon);

	CREATE_FUNC(CharacterSelectionScene);

protected:

	// character that Player chooses to play
	SELECTION _playerSelection = RED_DINO;

	// ParallaxNode
	cocos2d::Sprite* _background00;
	cocos2d::Sprite* _background01;

	// Animations for each character "button"
	cocos2d::Animate* _tyroAnimation;
	cocos2d::Animate* _trinoAnimation;
	cocos2d::Animate* _pteroAnimation;

	// Starting map (plus level + stage)
	std::string _startMap = "prologue_tutorial.tmx";
	std::string _startLevel = "Prologue";
	std::string _startStage = "Tutorial";

};

#endif // !__CHARACTER_SELECTION_SCENE_H__
