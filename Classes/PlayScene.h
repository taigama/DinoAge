#ifndef __PLAYSCENE_H__
#define __PLAYSCENE_H__

#include "cocos2d.h"

#include "WorldScene.h"
#include "HUDLayer.h"
#include "CharacterSelectionScene.h"

class Speecher;

class PlayScene : cocos2d::Layer
{
public:

	~PlayScene();

	/**
	 * Creates a play scene for the game with user
	 * playing as a Tyrannosaurus (object of RedDino class)
	 */
	static cocos2d::Scene* createScene();

	/**
	 * Creates a play scene for the game with 
	 * main character as user has selected
	 * @param playerSelection : the type of character user wants to play as { Tyrannosaurus, Trinocerous, Pterodactyl }
	 * @param startMap		  : the first map to start with
	 * @param startLevel	  : the first level to begin with
	 * @param startStage	  : the first stage to begin with
	 */
	static cocos2d::Scene* createScene(CharacterSelectionScene::SELECTION playerSelection, std::string startMap = "prologue_tutorial.tmx",
		std::string startLevel = "Prologue", std::string startStage = "Tutorial");

	/** Transition from the current map to the specified map
	 *		Used when advance to next Level/Stage or to Restart the current map
	 *
	 *	Order of actions:
	 *		Display the loading screen with Level title and Stage title
	 *		Removes the (current) old World + HUDLayer => Creates new ones instead
	 *		Removes the loading screen => show new map
	 *
	 * @param newMap     : the string specifies name of the new tile map (.tmx file)
	 * @param levelTitle : the Level of the given map
	 * @param stageTitle : the Stage of the given map
	 */
	void changeMap(std::string newMap, std::string levelTitle, std::string stageTitle);

	/** Start the scene with the specified map and level + stage
	 *		Build a LOADING SCREEN to show Level + Stage
	 */
	void startMap();

	/**
	 * Overrides onEnter and onExit functions with Transition in replacing scenes
	 */
	void onEnterTransitionDidFinish() override;
	void onExitTransitionDidStart() override;

	virtual bool init();

	// Selected character (chosen by Player on previous scene)
	// Note: value stored as int, but the actual meaning is valuated as CharacterSelectionScene::SELECTION
	static CharacterSelectionScene::SELECTION _playerSelection;

	/** Displays GAME OVER Scene
	 *		Called when Player dies
	 */
	void gameOver();

	/**
	* Helper function to pause / resume the game
	* Iterates through all nodes recursively and pause each one of them
	*/
	static void pauseRecursive(cocos2d::Node* node);
	static void resumeRecursive(cocos2d::Node* node);

	// Current Map + Level + Stage
	CC_SYNTHESIZE(std::string, _currentMap, CurrentMap);
	CC_SYNTHESIZE(std::string, _levelTitle, LevelTitle);
	CC_SYNTHESIZE(std::string, _stageTitle, StageTitle);

	// implement the "static create()" method manually
	CREATE_FUNC(PlayScene);

	CC_SYNTHESIZE_READONLY(Speecher*, _speecher, Speecher);

	HUDLayer* getHUD() { return _hud; };
protected:

	// Callback function
	void onMenuCallback(cocos2d::Ref*, cocos2d::ui::Widget::TouchEventType type);
	void onRestartCallback(cocos2d::Ref*, cocos2d::ui::Widget::TouchEventType type);


	// HUDLayer
	HUDLayer* _hud;


	bool _isClose;
};

#endif // !__PLAYSCENE_H__
