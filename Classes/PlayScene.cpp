#include "PlayScene.h"

//#include "SimpleAudioEngine.h"
#include "MenuScene1.h"
#include "ResourceManager.h"
#include "Speecher.h"

#if (CC_TARGET_PLATFORM == CC_PLATFORM_LINUX)
		#include "ui/CocosGUI.h"
#else
		#include "ui\CocosGUI.h"
#endif

USING_NS_CC;

#define WORLD_GRAVITY -1000

#define WORLD_Z_ORDER 1
#define HUD_Z_ORDER 2
#define SPEECH_Z_ORDER 3

#define GAME_OVER_FONT_SIZE 40.0f
#define GAME_OVER_LAYER_Z_ORDER 3
#define GAME_OVER_COMPONENT_Z_ORDER 5
#define LOADING_SCREEN_Z_ORDER 8

#define SCALE_FACTOR_RESTART_BUTTON 0.672f
#define SCALE_FACTOR_MENU_BUTTON 1.4f

#define TIME_TRANSITION_MAIN_MENU 0.5f
#define TIME_LOADING_SCREEN_FADE 0.5f
#define DELAY_TIME_LOADING 2.0f

#define LEVEL_TITLE_FONT_SIZE 60.0f
#define STAGE_TITLE_FONT_SIZE 30.0f



#include "AudioEngine.h"
#define AUDIO experimental::AudioEngine


CharacterSelectionScene::SELECTION PlayScene::_playerSelection = CharacterSelectionScene::SELECTION::RED_DINO;


PlayScene::~PlayScene()
{
	CC_SAFE_RELEASE(World::getCurrent());
	World::setCurrent(nullptr);

	CC_SAFE_RELEASE_NULL(_hud);
}

Scene* PlayScene::createScene()
{
	// Game scene with PhysicsWorld
	auto scene = Scene::createWithPhysics();

	//scene->getPhysicsWorld()->setDebugDrawMask(PhysicsWorld::DEBUGDRAW_ALL);
	
	// Gravity
	float gravity = (1 / Director::getInstance()->getContentScaleFactor()) * WORLD_GRAVITY;
	scene->getPhysicsWorld()->setGravity(Vec2(0, gravity));

	scene->getPhysicsWorld()->setAutoStep(false);

	// Attachs PlayScene layer to the main scene
	auto layer = PlayScene::create();
	layer->setName("play_scene");

	scene->addChild(layer);


	return scene;
}

Scene* PlayScene::createScene(CharacterSelectionScene::SELECTION playerSelection, std::string startMap, std::string startLevel, std::string startStage)
{
	// Sets value for _playerSelection
	PlayScene::_playerSelection = playerSelection;

	// Creates the corresponding scene
	auto scene = PlayScene::createScene();

	// Sets current map + level + stage
	auto playScene = (PlayScene*)(scene->getChildByName("play_scene"));

	playScene->setCurrentMap(startMap);
	playScene->setLevelTitle(startLevel);
	playScene->setStageTitle(startStage);


	return scene;
}

bool PlayScene::init()
{
	if (!Layer::init())
	{
		return false;
	}


	// First map (for new game) => Level: Prologue - Stage: Tutorial
	_currentMap = "prologue_tutorial.tmx";
	_levelTitle = "Prologue";
	_stageTitle = "Tutorial";

	return true;
}

void PlayScene::onEnterTransitionDidFinish()
{
	// Calls super
	Layer::onEnterTransitionDidFinish();


	// Creates World layer -> add to current scene
	auto world = World::createLayer(_currentMap);

	// Sets character
	(dynamic_cast<World*>(world))->setPlayer((CharacterSelectionScene::SELECTION)(PlayScene::_playerSelection));
	CC_SAFE_RETAIN(world);


	// Creates HUD layer -> add to current scene
	_hud = dynamic_cast<HUDLayer*>(HUDLayer::createLayer());
	CC_SAFE_RETAIN(_hud);


	///////////////////////////////////////////////////
	// Speecher
	_speecher = Speecher::create();
	this->addChild(_speecher, SPEECH_Z_ORDER);

	// Starts map
	startMap();
}

void PlayScene::onExitTransitionDidStart()
{
	// Calls super
	Layer::onExitTransitionDidStart();

	// Immediatelys drops World and HUDLayer when transition starts
	this->removeChild(World::getCurrent(), false);

	this->removeChild(_hud, false);
}

void PlayScene::gameOver()
{
	auto scaleFactor = Director::getInstance()->getContentScaleFactor();
	auto visibleOrigin = Director::getInstance()->getVisibleOrigin() + Vec2(_hud->getBoundingBox().getMinX(), _hud->getBoundingBox().getMinY());
	auto visibleSize = Director::getInstance()->getVisibleSize();

	auto paddingY = visibleSize.height / 20;
	auto paddingX = visibleSize.width / 20;


	// Underlying Layer
	auto layer = LayerColor::create(Color4B::BLACK);

	layer->setName("game_over_layer");
	layer->setAnchorPoint(Vec2::ZERO);
	layer->setOpacity(170);
	layer->setPosition(visibleOrigin);

	this->addChild(layer, GAME_OVER_LAYER_Z_ORDER);


	// GAME OVER title
	auto fontSize = GAME_OVER_FONT_SIZE / scaleFactor;
	auto label = Label::createWithTTF("GAME OVER", "gamecuben.ttf", fontSize);

	label->setName("game_over_title");
	label->setAnchorPoint(Vec2(0.5, 0.5));
	label->setTextColor(Color4B::RED);
	label->setPosition(Vec2(visibleOrigin.x + visibleSize.width / 2, visibleOrigin.y + visibleSize.height * 0.65f));

	this->addChild(label, GAME_OVER_COMPONENT_Z_ORDER);


	// GAME OVER buttons: restart or return to menu

	// Restart
	auto restartButton = ui::Button::create("restart.png", "restart.png", "");

	restartButton->setName("game_over_restart_button");
	restartButton->setScale(SCALE_FACTOR_RESTART_BUTTON);
	restartButton->setAnchorPoint(Vec2(1, 1));
	restartButton->setPosition(Vec2(label->getBoundingBox().getMidX() - paddingX, label->getBoundingBox().getMinY() - paddingY));
	restartButton->addTouchEventListener(CC_CALLBACK_2(PlayScene::onRestartCallback, this));

	this->addChild(restartButton, GAME_OVER_COMPONENT_Z_ORDER);

	// Menu
	auto menuButton = ui::Button::create("menu.png", "menu.png", "");

	menuButton->setName("game_over_menu_button");
	menuButton->setScale(SCALE_FACTOR_MENU_BUTTON);
	menuButton->setAnchorPoint(Vec2(0, 1));
	menuButton->setPosition(Vec2(label->getBoundingBox().getMidX() + paddingX, label->getBoundingBox().getMinY() - paddingY));
	menuButton->addTouchEventListener(CC_CALLBACK_2(PlayScene::onMenuCallback, this));

	this->addChild(menuButton, GAME_OVER_COMPONENT_Z_ORDER);


	// Pauses the game
	PlayScene::pauseRecursive(World::getCurrent());
	Director::getInstance()->getRunningScene()->getPhysicsWorld()->setSpeed(0.0f);

	// Pauses HUDLayer's components
	_hud->pauseAllEventListener();
}

void PlayScene::onMenuCallback(cocos2d::Ref*, cocos2d::ui::Widget::TouchEventType type)
{
	// Triggered on BEGAN click only
	if (type != ui::Widget::TouchEventType::BEGAN)
	{
		return;
	}

	AUDIO::setVolume(ResourceManager::getInstance()->backgroundSongID, 0.5f);

	// Unpauses the game
	PlayScene::resumeRecursive(World::getCurrent());
	Director::getInstance()->getRunningScene()->getPhysicsWorld()->setSpeed(1.0f);

	// Resumes HUDLayer's components
	_hud->resumeAllEventListener();



	// Returns to Main Menu scene
	Director::getInstance()->replaceScene(TransitionFade::create(TIME_TRANSITION_MAIN_MENU, MenuScene1::createScene()));
}

void PlayScene::onRestartCallback(cocos2d::Ref*, cocos2d::ui::Widget::TouchEventType type)
{
	// Triggered on BEGAN click only
	if (type != ui::Widget::TouchEventType::BEGAN)
	{
		return;
	}


	// Unpauses the game
	PlayScene::resumeRecursive(World::getCurrent());
	Director::getInstance()->getRunningScene()->getPhysicsWorld()->setSpeed(1.0f);

	// Resumes HUDLayer's components
	_hud->resumeAllEventListener();

	World::getCurrent()->getPlayer()->onDie();		// Player dies


	// Remove GAME OVER's components
	this->removeChildByName("game_over_layer");
	this->removeChildByName("game_over_title");
	this->removeChildByName("game_over_restart_button");
	this->removeChildByName("game_over_menu_button");

	
	// Changes to the CURRENT map (= restart)
	changeMap(_currentMap, _levelTitle, _stageTitle);
}

void PlayScene::pauseRecursive(Node* node)
{
	// Pauses the current node
	node->pause();

	// Iterates through its children
	auto children = node->getChildren();
	for (int i = 0; i < children.size(); ++i)
	{
		// Pauses recursively each one
		pauseRecursive(children.at(i));
	}
}

void PlayScene::resumeRecursive(Node* node)
{
	// Resumes the current node
	node->resume();

	// Iterates through its children
	auto children = node->getChildren();
	for (int i = 0; i < children.size(); ++i)
	{
		// Resumes recursively each one
		resumeRecursive(children.at(i));
	}
}

void PlayScene::changeMap(std::string newMap, std::string levelTitle, std::string stageTitle)
{
	// Saves the current legacy earned
	//auto currentLegacy = World::getCurrent()->getPlayer()->getLegacy();					// current
	//auto previousLegacy = UserDefault::getInstance()->getIntegerForKey("legacy");		// previous sum

	//UserDefault::getInstance()->setIntegerForKey("legacy", currentLegacy + previousLegacy);		// saves the new "sum"
	//UserDefault::getInstance()->flush();

	AUDIO::setVolume(ResourceManager::getInstance()->backgroundSongID, 0.5f);

	// Sets new values for PlayScene's properties
	_currentMap = newMap;
	_levelTitle = levelTitle;
	_stageTitle = stageTitle;


	// ------------------------------------- LOADING SCREEN ----------------------------------------------

	// Extract the actual size of whole map
	auto map = ResourceManager::getInstance()->getMap();
	auto mapSize = Size(map->getMapSize().width * map->getTileSize().width, map->getMapSize().height * map->getTileSize().height);

	// Adds LOADING SCREEN with animation fade in/out
	auto loadingScreen = LayerColor::create(Color4B::BLACK, mapSize.width, mapSize.height);

	loadingScreen->setName("loading_screen");
	loadingScreen->setOpacity(0);
	loadingScreen->setAnchorPoint(Vec2::ZERO);
	loadingScreen->setPosition(Director::getInstance()->getVisibleOrigin());

	this->addChild(loadingScreen, LOADING_SCREEN_Z_ORDER);

	// Animation FADE IN
	auto fadeIn = FadeIn::create(TIME_LOADING_SCREEN_FADE);

	loadingScreen->runAction(fadeIn->clone());



	// ------------------------------------------ NEW MAP ---------------------------------------------

	scheduleOnce([levelTitle, stageTitle, this](float delta) {

		// Retrieves current position and size of HUDLayer
		auto hudOrigin = Vec2(_hud->getBoundingBox().getMinX(), _hud->getBoundingBox().getMinY());
		auto hudSize = _hud->getBoundingBox().size;


		// Removes OLD World and HUDLayer (if exists)
		if (this->getChildByName("world_layer") != nullptr)
		{
			this->removeChildByName("world_layer");
			CC_SAFE_RELEASE(World::getCurrent());
			World::setCurrent(nullptr);
		}
		
		if (this->getChildByName("hud_layer") != nullptr)
		{
			this->removeChildByName("hud_layer");
			CC_SAFE_RELEASE_NULL(_hud);
			_hud = nullptr;
		}


		// --------------------------------------------- LEVEL + STAGE ------------------------------------------

		// Display current Level + Stage

		// Font size
		auto levelFontSize = LEVEL_TITLE_FONT_SIZE / Director::getInstance()->getContentScaleFactor();
		auto stageFontSize = STAGE_TITLE_FONT_SIZE / Director::getInstance()->getContentScaleFactor();

		// Animation
		auto fadeIn = FadeIn::create(DELAY_TIME_LOADING * 0.3f);
		auto fadeOut = FadeOut::create(DELAY_TIME_LOADING * 0.3f);
		auto delay= DelayTime::create(DELAY_TIME_LOADING * 0.3f);
		auto smallDelay = DelayTime::create(DELAY_TIME_LOADING * 0.1f);

		// Level + Stage labels
		auto level = Label::createWithTTF(levelTitle, "ArcadeClassic.ttf", levelFontSize);

		level->setName("level_title");
		level->setOpacity(0);
		level->setAnchorPoint(Vec2(0.5, 0));
		level->setPosition(hudOrigin + Vec2(hudSize.width / 2, hudSize.height * 0.55f));
		level->runAction(Sequence::create(fadeIn->clone(), delay->clone(), smallDelay->clone(), fadeOut->clone(), nullptr));

		this->addChild(level, LOADING_SCREEN_Z_ORDER + 1);


		auto stage = Label::createWithTTF(stageTitle, "ArcadeClassic.ttf", stageFontSize);

		stage->setName("stage_title");
		stage->setOpacity(0);
		stage->setAnchorPoint(Vec2(0.5, 1));
		stage->setPosition(hudOrigin + Vec2(hudSize.width / 2, hudSize.height * 0.5f));
		stage->runAction(Sequence::create(delay->clone(), fadeIn->clone(), smallDelay->clone(), fadeOut->clone(), nullptr));

		this->addChild(stage, LOADING_SCREEN_Z_ORDER + 1);


		// -------------------------------- LOADING SCREEN ----------------------------------------

		// Animation FADE OUT
		auto loadingScreen = this->getChildByName("loading_screen");

		fadeOut = FadeOut::create(TIME_LOADING_SCREEN_FADE);
		delay = DelayTime::create(DELAY_TIME_LOADING * 1.2);

		loadingScreen->runAction(Sequence::create(delay, fadeOut, nullptr));

	}, TIME_LOADING_SCREEN_FADE, "remove_world_scheduler");


	scheduleOnce([newMap, this](float delta) {

		// Creates NEW World and HUDLayer
		auto world = World::createLayer(newMap);		// World of the specified map
		(dynamic_cast<World*>(world))->setPlayer((CharacterSelectionScene::SELECTION)(PlayScene::_playerSelection));	// keeps current Character for Player
		CC_SAFE_RETAIN(world);

		_hud = dynamic_cast<HUDLayer*>(HUDLayer::createLayer());
		_hud->hideHUDLayer();		// hides HUDLayer
		CC_SAFE_RETAIN(_hud);

		this->addChild(world, WORLD_Z_ORDER);
		this->addChild(_hud, HUD_Z_ORDER);


		// Removes LEVEL + STAGE titles
		this->removeChildByName("level_title");
		this->removeChildByName("stage_title");

	}, TIME_LOADING_SCREEN_FADE + DELAY_TIME_LOADING, "renew_world_scheduler");



	// ------------------------------------------- LOADING SCREEN -------------------------------------------

	// Removes LOADING SCREEN
	scheduleOnce([&](float delta) {

		this->removeChildByName("loading_screen");

		// Shows HUDLayer
		_hud->unhideHUDLayer();

	}, TIME_LOADING_SCREEN_FADE * 2 + DELAY_TIME_LOADING * 1.2, "remove_loading_screen_scheduler");
}

void PlayScene::startMap()
{
	// ----------------------------------------- CREATE LOADING SCREEN ---------------------------------------

	// Extract the actual size of whole map
	auto map = ResourceManager::getInstance()->getMap();
	auto mapSize = Size(map->getMapSize().width * map->getTileSize().width, map->getMapSize().height * map->getTileSize().height);

	// Adds LOADING SCREEN with animation fade in/out
	auto loadingScreen = LayerColor::create(Color4B::BLACK, mapSize.width, mapSize.height);

	loadingScreen->setName("loading_screen");
	loadingScreen->setAnchorPoint(Vec2::ZERO);
	loadingScreen->setPosition(Director::getInstance()->getVisibleOrigin());

	this->addChild(loadingScreen, LOADING_SCREEN_Z_ORDER);


	// --------------------------------------------- LEVEL + STAGE TITLES ------------------------------------------------

	auto visibleOrigin = Director::getInstance()->getVisibleOrigin();
	auto visibleSize = Director::getInstance()->getVisibleSize();


	// Display current Level + Stage

	// Font size
	auto levelFontSize = LEVEL_TITLE_FONT_SIZE / Director::getInstance()->getContentScaleFactor();
	auto stageFontSize = STAGE_TITLE_FONT_SIZE / Director::getInstance()->getContentScaleFactor();

	// Animation
	auto fadeIn = FadeIn::create(DELAY_TIME_LOADING * 0.3f);
	auto fadeOut = FadeOut::create(DELAY_TIME_LOADING * 0.3f);
	auto delay = DelayTime::create(DELAY_TIME_LOADING * 0.3f);
	auto smallDelay = DelayTime::create(DELAY_TIME_LOADING * 0.1f);

	// Level + Stage labels
	auto level = Label::createWithTTF(_levelTitle, "ArcadeClassic.ttf", levelFontSize);

	level->setName("level_title");
	level->setOpacity(0);
	level->setAnchorPoint(Vec2(0.5, 0));
	level->setPosition(visibleOrigin + Vec2(visibleSize.width / 2, visibleSize.height * 0.55f));
	level->runAction(Sequence::create(fadeIn->clone(), delay->clone(), smallDelay->clone(), fadeOut->clone(), nullptr));

	this->addChild(level, LOADING_SCREEN_Z_ORDER + 1);


	auto stage = Label::createWithTTF(_stageTitle, "ArcadeClassic.ttf", stageFontSize);

	stage->setName("stage_title");
	stage->setOpacity(0);
	stage->setAnchorPoint(Vec2(0.5, 1));
	stage->setPosition(visibleOrigin + Vec2(visibleSize.width / 2, visibleSize.height * 0.5f));
	stage->runAction(Sequence::create(delay->clone(), fadeIn->clone(), smallDelay->clone(), fadeOut->clone(), nullptr));

	this->addChild(stage, LOADING_SCREEN_Z_ORDER + 1);


	// ------------------------------------- LOADING SCREEN FADE OUT -------------------------------------------

	// Animation FADE OUT
	fadeOut = FadeOut::create(TIME_LOADING_SCREEN_FADE);
	delay = DelayTime::create(DELAY_TIME_LOADING * 1.2);

	loadingScreen->runAction(Sequence::create(delay, fadeOut, nullptr));


	// ------------------------------------------ STARTING MAP ------------------------------------------

	scheduleOnce([&](float delta) {

		// Adds World and HUDLayer. NOTE: already RETAIN
		this->addChild(World::getCurrent(), WORLD_Z_ORDER);

		_hud->hideHUDLayer();
		this->addChild(_hud, HUD_Z_ORDER);


		// Removes LEVEL + STAGE titles
		this->removeChildByName("level_title");
		this->removeChildByName("stage_title");

	}, DELAY_TIME_LOADING, "renew_world_scheduler");


	// ------------------------------------------- LOADING SCREEN REMOVAL -------------------------------------------

	// Removes LOADING SCREEN
	scheduleOnce([&](float delta) {

		this->removeChildByName("loading_screen");

		// Shows HUDLayer
		_hud->unhideHUDLayer();

	}, TIME_LOADING_SCREEN_FADE + DELAY_TIME_LOADING * 1.2, "remove_loading_screen_scheduler");
}
