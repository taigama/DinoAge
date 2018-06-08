#ifndef __MENU_SCENE_1_H__
#define __MENU_SCENE_1_H__

#include "cocos2d.h"

#if (CC_TARGET_PLATFORM == CC_PLATFORM_LINUX)
		#include "ui/CocosGUI.h"
#else
		#include "ui\CocosGUI.h"
#endif

#include "Projectile.h"
#include "IconSkill.h"

class MenuScene1 : public cocos2d::Layer
{
public:

	~MenuScene1();

	static cocos2d::Scene* createScene();

	virtual bool init();

	// Overrides
	void onExitTransitionDidStart() override;

	// ------------- Selector callback(s) -----------

	// For ui::Button
	void onPlayCallback(cocos2d::Ref* sender, cocos2d::ui::Widget::TouchEventType type);
	void onLevelsCallback(cocos2d::Ref* sender, cocos2d::ui::Widget::TouchEventType type);
	void onSkillsCallback(cocos2d::Ref* sender, cocos2d::ui::Widget::TouchEventType type);
	void onExitCallback(cocos2d::Ref* sender, cocos2d::ui::Widget::TouchEventType type);

	void onLevelsReturnCallback(cocos2d::Ref* sender, cocos2d::ui::Widget::TouchEventType type);
	void onSkillsReturnCallback(cocos2d::Ref* sender, cocos2d::ui::Widget::TouchEventType type);

	void onIconSkillCallback(cocos2d::Ref* sender, cocos2d::ui::Widget::TouchEventType type);
	void onViewCallback(cocos2d::Ref* sender, cocos2d::ui::Widget::TouchEventType type);
	void onUnlockCallback(cocos2d::Ref* sender, cocos2d::ui::Widget::TouchEventType type);
	void onCloseCallback(cocos2d::Ref* sender, cocos2d::ui::Widget::TouchEventType type);

	void onLevelStageCallback(cocos2d::Ref* sender, cocos2d::ui::Widget::TouchEventType type);

	// ----------------------------------------------

	/**
	 * Helper function: add title into the scene with some "effects" and "actions"
	 * @param title     : the title Node that needs to be added
	 * @param delayTime : amount of time (in seconds) the title Node has to wait before being added
	 */
	void addTitle(cocos2d::Label*& title, const float delayTime);

	// Similar to addTitle(), but now add Sprite instead
	void addTitleImage(cocos2d::Sprite*& sprite, const float delayTime);

	/**
	 *	Helper function to add Button (both template + label) into the scene with "effects" and "actions"
	 *	@param button	: button template
	 *	@param label	: button label
	 */
	void addButton(cocos2d::ui::Button*& button, cocos2d::Label*& label, const float delayTime);

	/**
	 * Hide/Show main layer with some effects:
	 *    + Top components     -> move up to hide, move down to show
	 *    + Bottom componenets -> move down to hide, move up to show
	 */
	void hideMainLayer();
	void showMainLayer();

	/**
	 * Hide/Show level selection layer with some effects:
	 *    + Top components     -> move up to hide, move down to show
	 *    + Bottom componenets -> move down to hide, move up to show 
	 */
	void hideLevelsLayer();
	void showLevelsLayer();

	/**
	 * Hide/Show upgrade-skill layer:
	 *	  + Top components	   -> move up to hide, move down to show
	 *	  + Bottom components  -> move down to hide, move up to show
	 */
	void hideSkillsLayer();
	void showSkillsLayer();

	// Needed to keep track of listener for touch gesture
	void update(float delta);

	// Saves the skills-pattern (for each Character) and Player's current legacy
	void saveValues();

	// True if is moving to another "scene"
	bool movingScene = false;

	// Main layer
	CC_SYNTHESIZE_READONLY(cocos2d::Layer*, _mainLayerTop, MainLayerTop);
	CC_SYNTHESIZE_READONLY(cocos2d::Layer*, _mainLayerBottom, MainLayerBottom);

	// Level selection layer
	CC_SYNTHESIZE_READONLY(cocos2d::Layer*, _levelsLayerTop, LevelsLayerTop);
	CC_SYNTHESIZE_READONLY(cocos2d::Layer*, _levelsLayerBottom, LevelsLayerBottom);

	// Upgrade skill layers: top and bottom
	CC_SYNTHESIZE_READONLY(cocos2d::Layer*, _skillsLayerTop, SkillsLayerTop);
	CC_SYNTHESIZE_READONLY(cocos2d::Layer*, _skillsLayerBottom, SkillsLayerBottom);
	CC_SYNTHESIZE_READONLY(cocos2d::LayerColor*, _skillsLayerView, SkillsLayerView);

	// The currently selected IconSkill (in UNLOCK-SKILL panel)
	CC_SYNTHESIZE_READONLY(IconSkill*, _selectedIcon, SelectedIcon);

	CREATE_FUNC(MenuScene1);

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

	// Selected map in level-selection panel
	std::string _selectedMap;
};

#endif // !__MENU_SCENE_1_H__
