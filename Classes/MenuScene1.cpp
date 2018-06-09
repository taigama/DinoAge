#include "MenuScene1.h"

#include "PlayScene.h"
#include "ResourceManager.h"
#include "Gold.h"


#define TITLE_SIZE 80.0f
#define SUB_TITLE_SIZE 50.0f
#define BUTTON_TEXT_SIZE 25.0f
#define BUTTON_PRICE_TEXT_SIZE 20.0f
#define DESCRIPTION_TITLE_SIZE 20.0f
#define DESCRIPTION_SUMMARY_SIZE 14.0f
#define LEGACY_LABEL_SIZE 18.0f
#define LEVEL_STAGE_FONT_SIZE 16.0f
#define LEVEL_PANEL_QUESTION_FONT_SIZE 19.0f

#define DESCRIPTION_TITLE_HEIGHT_PERCENT 0.3f
#define DESCRIPTION_SUMMARY_HEIGHT_PERCENT 0.7f

#define SCALE_IMAGE_TITLE 2.0f
#define SCALE_BUTTON 1.0f
#define SCALE_UNLOCK_BUTTON 1.4f
#define SCALE_CLOSE_BUTTON 0.6f
#define SCALE_BACK_BUTTON 0.9f
#define SCALE_CHARACTER_AVATAR 1.4f
#define SCALE_LEGACY_BUTTON_ICON 0.3f
#define SCALE_LEGACY_INDICATION_ICON 0.5f
#define SCALE_LEVEL_MAP_X 1.5f
#define SCALE_LEVEL_MAP_Y 0.8f
#define SCALE_SMALL_BUTTON_LEVEL 0.65f
#define SCALE_LEVEL_QUESTION_BUTTON 0.9f

#define SKILL_TOP_LAYER_Z_ORDER 1
#define SKILL_BOTTOM_LAYER_Z_ORDER 5
#define SKILL_VIEW_LAYER_Z_ORDER 3
#define LEVEL_TOP_LAYER_Z_ORDER 1
#define LEVEL_BOTTOM_LAYER_Z_ORDER 3
#define LEVEL_QUESTION_LAYER_Z_ORDER 5

#define PROLOGUE_TUTORIAL_TAG 0	
#define PROLOGUE_STAGE1_TAG 1
#define PROLOGUE_STAGE2_TAG 2	
#define PROLOGUE_STAGE3_TAG 3

#define DELAY_TIME_FOR_TITLE_UP 0.0f
#define DELAY_TIME_FOR_TITLE_DOWN 0.5f
#define DELAY_TIME_FOR_TITLE_IMAGE 1.0f
#define DELAY_TIME_FOR_BUTTON 0.5f
#define DELAY_TIME_FOR_BUTTON_LISTENERS 0.5f

#define DELAY_TIME_HIDE_MAIN_LAYER 0.5f
#define DELAY_TIME_SHOW_MAIN_LAYER 0.5f
#define DELAY_TIME_HIDE_LEVELS_LAYER 0.4f
#define DELAY_TIME_SHOW_LEVELS_LAYER 0.4f
#define DELAY_TIME_SHOW_SKILLS_LAYER 0.5f
#define DELAY_TIME_HIDE_SKILLS_LAYER 0.5f

#define DELAY_TIME_SWIPE_LEFT 0.5f
#define DELAY_TIME_SWIPE_RIGHT 0.5f

#define TRANSITION_TIME_PLAY_CALLBACK 0.5f
#define TRANSITION_TIME_LEVEL_STAGE_CALLBACK 0.6f

#define NUMBER_OF_SKILLS 5

USING_NS_CC;

MenuScene1::~MenuScene1()
{
	auto resManager = ResourceManager::getInstance();
	if (resManager->readyDelete)
	{
		delete resManager;
	}
	else
	{
		resManager->readyDelete = true;
	}
	CC_SAFE_RELEASE_NULL(_skillsLayerView);
}

void MenuScene1::onExitTransitionDidStart()
{
	// Super
	Layer::onExitTransitionDidStart();

	saveValues();
}

Scene* MenuScene1::createScene()
{
	auto scene = Scene::create();

	auto layer = MenuScene1::create();

	scene->addChild(layer);

	return scene;
}

bool MenuScene1::init()
{
	//////////////////////////
	// 1. Super init first
	if (!Layer::init())
	{
		return false;
	}

	scheduleUpdate();


	// Load all the sprites
	/*auto resourceManager = */
	//resourceManager->loadSprites();

	auto visibleSize = Director::getInstance()->getVisibleSize();
	auto visibleOrigin = Director::getInstance()->getVisibleOrigin();

	auto center = visibleOrigin + Vec2(visibleSize.width / 2, visibleSize.height / 2);
	auto titleAltitude = visibleOrigin + Vec2(visibleSize.width / 2, visibleSize.height - visibleSize.height / 3.0f);
	auto buttonAltitude = visibleOrigin + Vec2(visibleSize.width / 2, visibleSize.height * 0.3f);

	auto scaleFactor = Director::getInstance()->getContentScaleFactor();

	auto titleSize = TITLE_SIZE / scaleFactor;


	_skillsLayerTop = nullptr;
	_skillsLayerBottom = nullptr;
	_skillsLayerView = nullptr;

	_levelsLayerTop = nullptr;
	_levelsLayerBottom = nullptr;

	_currentLegacy = UserDefault::getInstance()->getIntegerForKey("player_legacy", 0);
	_redDinoSkillsPattern = UserDefault::getInstance()->getIntegerForKey("red_dino_skills_set", 31);		// default value = 0x11000
	_yellowDinoSkillsPattern = UserDefault::getInstance()->getIntegerForKey("yellow_dino_skills_set", 31);		// default value = 0x11000
	_whiteDinoSkillsPattern = UserDefault::getInstance()->getIntegerForKey("white_dino_skills_set", 31);		// default value = 0x11000

	
	// Initializes main layer

	// TOP
	_mainLayerTop = Layer::create();
	_mainLayerTop->setPosition(Vec2::ZERO);

	this->addChild(_mainLayerTop);

	// BOTTOM
	_mainLayerBottom = Layer::create();
	_mainLayerBottom->setPosition(Vec2::ZERO);

	this->addChild(_mainLayerBottom);


	///////////////
	// 2. Title
	auto titleUp = Label::createWithTTF("DINO", "Bakesaurus.ttf", titleSize);
	auto titleDown = Label::createWithTTF("AGE", "Bakesaurus.ttf", titleSize);
	auto titleImage = Sprite::create("skull_inverted.png");

	titleUp->setName("titleUp");
	titleDown->setName("titleDown");
	titleImage->setName("titleImage");

	titleImage->setScale(SCALE_IMAGE_TITLE);

	// Sets position for title(s)
	titleUp->setPosition(Vec2(titleAltitude.x - titleUp->getContentSize().width * 0.50f, titleAltitude.y));
	titleDown->setPosition(Vec2(titleAltitude.x + titleDown->getContentSize().width * 0.80f, titleAltitude.y));
	titleImage->setPosition(titleAltitude);

	// Adds titleUp into the scene
	addTitle(titleUp, DELAY_TIME_FOR_TITLE_UP);

	// Add titleDown into the scene, after ... seconds
	addTitle(titleDown, DELAY_TIME_FOR_TITLE_DOWN);		// Note: titleUp finished being added after 1.5f seconds

	// Add image "between" title
	addTitleImage(titleImage, DELAY_TIME_FOR_TITLE_IMAGE);


	////////////////
	// 3. Buttons

	auto paddingX = visibleSize.width / 5.0f;
	auto paddingY = visibleSize.height / 30.0f;

	// -------------- Play button ---------------

	auto playButton = ui::Button::create("button_active_template.png", "button_click_template.png", "button_disable_template.png");
	auto playLabel = Label::createWithTTF("PLAY", "ArcadeClassic.ttf", BUTTON_TEXT_SIZE / scaleFactor);

	playButton->setName("playButton");
	playButton->setAnchorPoint(Vec2(0.5f, 0.5f));
	playButton->setScale(SCALE_BUTTON);
	playButton->setPosition(buttonAltitude + Vec2(-paddingX, 0));

	playLabel->setName("playLabel");
	playLabel->setAnchorPoint(Vec2(0.5, 0.5));
	playLabel->setPosition(Vec2(playButton->getBoundingBox().getMidX(), playButton->getBoundingBox().getMidY()));

	// Adds to the scene with "effects"
	addButton(playButton, playLabel, DELAY_TIME_FOR_BUTTON);

	
	// -------------- Exit button ---------------

	auto exitButton = ui::Button::create("button_active_template.png", "button_click_template.png", "button_disable_template.png");
	auto exitLabel = Label::createWithTTF("EXIT", "ArcadeClassic.ttf", BUTTON_TEXT_SIZE / scaleFactor);

	exitButton->setName("exitButton");
	exitButton->setAnchorPoint(Vec2(0.5f, 0.5f));
	exitButton->setScale(SCALE_BUTTON);
	exitButton->setPosition(buttonAltitude + Vec2(paddingX, 0));

	exitLabel->setName("exitLabel");
	exitLabel->setAnchorPoint(Vec2(0.5, 0.5));
	exitLabel->setPosition(Vec2(exitButton->getBoundingBox().getMidX(), exitButton->getBoundingBox().getMidY()));

	// Adds to the scene with "effects"
	addButton(exitButton, exitLabel, DELAY_TIME_FOR_BUTTON);


	/////////////////////////////////////
	// 4. Add background image
	auto spriteBackground = Sprite::create("menu_background_1280x720.jpg");

	// Scales background image
	auto scaleBG = MAX(visibleSize.width / spriteBackground->getContentSize().width, visibleSize.height / spriteBackground->getContentSize().height);
	
	spriteBackground->setScale(scaleBG);

	spriteBackground->setOpacity(150);

	// Positions background: center of the screen
	spriteBackground->setPosition(center);

	// Adds background node to the scene
	this->addChild(spriteBackground, -1);		// layer -1





	// Schedules to add EventListener to all BOTTOM Layer components
	scheduleOnce([&](float delta) {

		auto play = dynamic_cast<ui::Button*>(getMainLayerBottom()->getChildByName("playButton"));
		auto exit = dynamic_cast<ui::Button*>(getMainLayerBottom()->getChildByName("exitButton"));

		play->addTouchEventListener(CC_CALLBACK_2(MenuScene1::onPlayCallback, this));
		exit->addTouchEventListener(CC_CALLBACK_2(MenuScene1::onExitCallback, this));

	}, DELAY_TIME_FOR_BUTTON_LISTENERS, "button_touch_listener");



	// init sound
	//AudioEngine
	//experimental::AudioEngine::


//	//auto audio = SimpleAudioEngine::getInstance();
//	AUDIO::preload("Button.ogg");
//	AUDIO::preload("challenge.ogg");
//	AUDIO::preload("player_select.ogg");
//	AUDIO::preload("PointError.ogg");
//	AUDIO::preload("ST0E_U1_00001.ogg");
//	AUDIO::preload("ST01_00_00002.ogg");
//
//#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)
//	AUDIO::preload("the-only-friend.mp3");
//	AUDIO::preload("digimon-rumble-arena_mid_stress.mp3");
//#else// for android
//	AUDIO::preload("the-only-friend.ogg");
//	AUDIO::preload("digimon-rumble-arena_mid_stress.ogg");
//#endif
//	
//
//
//
//
//


	return true;
}

int foo = 0;
void MenuScene1::addTitle(Label*& title, const float delayTime)
{
	// Sets up title for the effect/action
	title->setTextColor(Color4B::WHITE);
	title->setOpacity(0);
	title->setRotation(180);
	title->setScale(3);

	// Actions for title
	auto delay = DelayTime::create(delayTime);
	auto rotate = RotateBy::create(0.6f, Vec3(0, 0, 180));
	auto scale = ScaleBy::create(0.6f, 1.0f / 3.0f);
	auto fadeIn = FadeIn::create(0.7f);

	auto easeOut = EaseOut::create(rotate->clone(), 0.5f);

	auto spawn = Spawn::create(easeOut, scale, fadeIn, nullptr);

	auto slam = CallFunc::create([]() {
		AUDIO::play2d("ONPARE7_00002.ogg");
	});

	auto sequence = Sequence::create(delay, spawn, slam, nullptr);

	char a[8];
	sprintf(a, "%d", (++foo));

	//from K: nah, I hate to do this
	scheduleOnce([title](float delta) {
		AUDIO::play2d("wind.ogg");
	}, delayTime, a);


	// Executes actions
	title->runAction(sequence);

	// Adds to TOP layer
	_mainLayerTop->addChild(title, 0);		// layer 0
}

void MenuScene1::addTitleImage(Sprite*& sprite, const float delayTime)
{
	// Initial setup for the effect(s)
	sprite->setOpacity(0);

	// Actions
	auto delay = DelayTime::create(delayTime);
	auto fadeTo = FadeTo::create(0.5f, 255);
	auto scale = ScaleBy::create(0.5f, 1.0f / 6.0f);

	auto spawn = Spawn::create(fadeTo, scale, nullptr);

	auto slam = CallFunc::create([]() {
		AUDIO::play2d("roar.ogg");
	});

	auto sequence = Sequence::create(delay, slam, spawn, nullptr);

	// Executes actions
	sprite->runAction(sequence);

	// Adds to TOP layer
	_mainLayerTop->addChild(sprite, 1);		// layer 1

	char a[8];
	sprintf(a, "%d", (++foo));
	//from K: nah, I hate to do this
	scheduleOnce([](float delta) {
		AUDIO::play2d("wind.ogg");
	}, delayTime, a);
}

void MenuScene1::addButton(ui::Button*& button, Label*& label, const float delayTime)
{
	// Initial setup
	button->setOpacity(0);
	label->setOpacity(0);

	// Actions
	auto delay = DelayTime::create(delayTime);
	auto fadeTo = FadeTo::create(0.5f, 255);

	auto spawn = Spawn::create(fadeTo, nullptr);

	auto sequence = Sequence::create(delay, spawn, nullptr);

	// Executes actions
	button->runAction(sequence->clone());
	label->runAction(sequence->clone());

	// Adds to BOTTOM layer
	_mainLayerBottom->addChild(button, 0);		// layer 0
	_mainLayerBottom->addChild(label, 1);		// layer 1 -> on top of button template
}

void MenuScene1::onExitCallback(Ref* sender, cocos2d::ui::Widget::TouchEventType type)
{
	if (movingScene)
	{
		//SimpleAudioEngine::getInstance()->playEffect("PointError.ogg");
		AUDIO::play2d("PointError.ogg");
		return;
	}

	// Changing scene only if user release ON button
	if (type != ui::Widget::TouchEventType::ENDED)
	{
		//SimpleAudioEngine::getInstance()->playEffect("PointError.ogg");
		return;
	}

	AUDIO::play2d("challenge.ogg");
	//SimpleAudioEngine::getInstance()->playEffect("challenge.ogg");

	// Sets status -> transition between scene
	movingScene = true;

	// Hides main layer with some "effects"
	hideMainLayer();

	scheduleOnce([&](float delta) {

		movingScene = false;

		_mainLayerTop->setVisible(false);

		_mainLayerBottom->setVisible(false);
		_mainLayerBottom->pause();


		// Unload sprites
		//delete ResourceManager::getInstance();

		// Exit
		Director::getInstance()->end();

#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
		exit(0);
#endif

	}, DELAY_TIME_HIDE_MAIN_LAYER, "hide_main_on_moving_to_levels");
}


void MenuScene1::onPlayCallback(cocos2d::Ref* sender, cocos2d::ui::Widget::TouchEventType type)
{
	if (movingScene)
	{
		//SimpleAudioEngine::getInstance()->playEffect("PointError.ogg");
		AUDIO::play2d("PointError.ogg");
		return;
	}

	// Changing scene only if user release ON button
	if (type != ui::Widget::TouchEventType::ENDED)
	{
		//SimpleAudioEngine::getInstance()->playEffect("PointError.ogg");
		return;
	}

	// Sets status -> transition between scene
	movingScene = true;



	// Sound
	AUDIO::play2d("Button.ogg");


	auto visibleSize = Director::getInstance()->getVisibleSize();
	auto visibleOrigin = Director::getInstance()->getVisibleOrigin();

	auto paddingX = visibleSize.width / 20;
	auto paddingY = visibleSize.height / 20;

	auto subTitleAltitude = visibleOrigin + Vec2(visibleSize.width / 2, visibleSize.height - paddingY * 3);
	auto mapAltitude = visibleOrigin + Vec2(visibleSize.width / 2, visibleSize.height * 0.4f);

	auto scaleFactor = Director::getInstance()->getContentScaleFactor();

	auto subTitleSize = SUB_TITLE_SIZE / scaleFactor;


	// Hides main layer with some "effects"
	hideMainLayer();

	scheduleOnce([&](float delta) {

		_mainLayerTop->setVisible(false);

		_mainLayerBottom->setVisible(false);
		_mainLayerBottom->pause();

		movingScene = false;

	}, DELAY_TIME_HIDE_MAIN_LAYER, "hide_main_on_moving_to_levels");


	// Creates level layers TOP + BOTTOM (if not already)
	if (_levelsLayerTop == nullptr || _levelsLayerBottom == nullptr)
	{
		// --------------------------------- TOP LAYER ------------------------------------------
		_levelsLayerTop = Layer::create();

		this->addChild(_levelsLayerTop, LEVEL_TOP_LAYER_Z_ORDER);

		_levelsLayerTop->setPosition(Vec2::ZERO);


		// COMPONENTS

		// (Sub-)Title
		auto subTitle = Label::createWithTTF("CHOOSE  LEVEL", "ArcadeClassic.ttf", subTitleSize);

		subTitle->setName("subTitle");
		subTitle->setAnchorPoint(Vec2(0.5, 0.5));
		subTitle->setPosition(subTitleAltitude);

		_levelsLayerTop->addChild(subTitle);

		// Return button
		auto returnButton = ui::Button::create("back_button.png");

		returnButton->setName("returnButton");
		returnButton->setScale(SCALE_BACK_BUTTON);
		returnButton->setAnchorPoint(Vec2(0, 1));
		returnButton->setPosition(Vec2(visibleOrigin.x + paddingX / 8, visibleOrigin.y + visibleSize.height - paddingY / 4));
		returnButton->addTouchEventListener(CC_CALLBACK_2(MenuScene1::onLevelsReturnCallback, this));

		_levelsLayerTop->addChild(returnButton);


		// INITAL setup
		_levelsLayerTop->setVisible(false);
		for (auto child : _levelsLayerTop->getChildren())
		{
			child->setOpacity(0);
		}
		_levelsLayerTop->setPosition(_levelsLayerTop->getPosition() + Vec2(0, visibleSize.height / 2));


		// ----------------------------------------- BOTTOM LAYER -----------------------------------------

		_levelsLayerBottom = Layer::create();

		this->addChild(_levelsLayerBottom, LEVEL_BOTTOM_LAYER_Z_ORDER);

		_levelsLayerBottom->setPosition(Vec2::ZERO);


		// COMPONENTS

		// Background map
		auto backgroundMap = Sprite::create("map.png");

		backgroundMap->setName("background_map");

		backgroundMap->setScaleX(SCALE_LEVEL_MAP_X);
		backgroundMap->setScaleY(SCALE_LEVEL_MAP_Y);

		backgroundMap->setAnchorPoint(Vec2(0.5, 0.5));
		backgroundMap->setPosition(mapAltitude);

		_levelsLayerBottom->addChild(backgroundMap, -1);			// background layer


		// "Button" for each Level-Stage (map) for Player to choose
		
		// Small button TEMPLATE
		auto smallButtonTemplate = ui::Button::create("small_button_active_template.png", "small_button_click_template.png", "small_button_disable_template.png");

		smallButtonTemplate->setAnchorPoint(Vec2(0.5, 0.5));
		smallButtonTemplate->setScale(SCALE_SMALL_BUTTON_LEVEL);


		// Level: Prologue - Stage: Tutorial (x : 44% - y : 13%)
		auto prologueTutorialButton = dynamic_cast<ui::Button*>(smallButtonTemplate->clone());

		prologueTutorialButton->setName("Prologue-Tutorial");
		prologueTutorialButton->setTag(PROLOGUE_TUTORIAL_TAG);
		prologueTutorialButton->setPosition(Vec2(backgroundMap->getBoundingBox().getMinX() + backgroundMap->getBoundingBox().size.width * 0.44f,
			backgroundMap->getBoundingBox().getMinY() + backgroundMap->getBoundingBox().size.height * 0.13f));
		prologueTutorialButton->addTouchEventListener(CC_CALLBACK_2(MenuScene1::onLevelStageCallback, this));

		_levelsLayerBottom->addChild(prologueTutorialButton, 0);		// on top of map


		auto prologueTutorialLabel = Label::createWithTTF("0-0", "6809 chargen.ttf", LEVEL_STAGE_FONT_SIZE / scaleFactor);

		prologueTutorialLabel->setName("prologue_tutorial_label");
		prologueTutorialLabel->setAnchorPoint(Vec2(0.5, 0.5));
		prologueTutorialLabel->setPosition(prologueTutorialButton->getBoundingBox().getMidX(), prologueTutorialButton->getBoundingBox().getMidY());

		_levelsLayerBottom->addChild(prologueTutorialLabel, 1);			// on top of (small) button template


		// Level: Prologue - Stage: 1 (x : 27% - y : 39%)
		auto prologueStage1Button = dynamic_cast<ui::Button*>(smallButtonTemplate->clone());

		prologueStage1Button->setName("Prologue-Stage 1");
		prologueStage1Button->setTag(PROLOGUE_STAGE1_TAG);
		prologueStage1Button->setPosition(Vec2(backgroundMap->getBoundingBox().getMinX() + backgroundMap->getBoundingBox().size.width * 0.27f,
			backgroundMap->getBoundingBox().getMinY() + backgroundMap->getBoundingBox().size.height * 0.39f));
		prologueStage1Button->addTouchEventListener(CC_CALLBACK_2(MenuScene1::onLevelStageCallback, this));

		_levelsLayerBottom->addChild(prologueStage1Button, 0);			// on top of map

		auto prologueStage1Label = Label::createWithTTF("0-1", "6809 chargen.ttf", LEVEL_STAGE_FONT_SIZE / scaleFactor);

		prologueStage1Label->setName("prologue_stage1_label");
		prologueStage1Label->setAnchorPoint(Vec2(0.5, 0.5));
		prologueStage1Label->setPosition(Vec2(prologueStage1Button->getBoundingBox().getMidX(), prologueStage1Button->getBoundingBox().getMidY()));

		_levelsLayerBottom->addChild(prologueStage1Label, 1);			// on top of (small) button template


		// Level: Prologue - Stage: 2 (x : 44% - y : 60%)
		auto prologueStage2Button = dynamic_cast<ui::Button*>(smallButtonTemplate->clone());

		prologueStage2Button->setName("Prologue-Stage 2");
		prologueStage2Button->setTag(PROLOGUE_STAGE2_TAG);
		prologueStage2Button->setPosition(Vec2(backgroundMap->getBoundingBox().getMinX() + backgroundMap->getBoundingBox().size.width * 0.44f,
			backgroundMap->getBoundingBox().getMinY() + backgroundMap->getBoundingBox().size.height * 0.60f));
		prologueStage2Button->addTouchEventListener(CC_CALLBACK_2(MenuScene1::onLevelStageCallback, this));

		_levelsLayerBottom->addChild(prologueStage2Button, 0);			// on to of map

		auto prologueStage2Label = Label::createWithTTF("0-2", "6809 chargen.ttf", LEVEL_STAGE_FONT_SIZE / scaleFactor);

		prologueStage2Label->setName("prologue_stage2_label");
		prologueStage2Label->setAnchorPoint(Vec2(0.5, 0.5));
		prologueStage2Label->setPosition(Vec2(prologueStage2Button->getBoundingBox().getMidX(), prologueStage2Button->getBoundingBox().getMidY()));

		_levelsLayerBottom->addChild(prologueStage2Label, 1);			// on top of (small) button template


		// Level: Prologue - Stage: 3
		auto prologueStage3Button = dynamic_cast<ui::Button*>(smallButtonTemplate->clone());

		prologueStage3Button->setName("Prologue-Stage 3");
		prologueStage3Button->setTag(PROLOGUE_STAGE3_TAG);
		prologueStage3Button->setPosition(Vec2(backgroundMap->getBoundingBox().getMinX() + backgroundMap->getBoundingBox().size.width * 0.62f,
			backgroundMap->getBoundingBox().getMinY() + backgroundMap->getBoundingBox().size.height * 0.32f));
		prologueStage3Button->addTouchEventListener(CC_CALLBACK_2(MenuScene1::onLevelStageCallback, this));

		_levelsLayerBottom->addChild(prologueStage3Button, 0);			// on to of map

		auto prologueStage3Label = Label::createWithTTF("0-3", "6809 chargen.ttf", LEVEL_STAGE_FONT_SIZE / scaleFactor);

		prologueStage3Label->setName("prologue_stage3_label");
		prologueStage3Label->setAnchorPoint(Vec2(0.5, 0.5));
		prologueStage3Label->setPosition(Vec2(prologueStage3Button->getBoundingBox().getMidX(), prologueStage3Button->getBoundingBox().getMidY()));

		_levelsLayerBottom->addChild(prologueStage3Label, 1);			// on top of (small) button template


		// INTIAL setup
		_levelsLayerBottom->setVisible(false);
		for (auto child : _levelsLayerBottom->getChildren())
		{
			child->setOpacity(0);
		}
		_levelsLayerBottom->setPosition(_levelsLayerBottom->getPosition() - Vec2(0, visibleSize.height / 2));

	}



	// Shows level selection layer with some "effects"

	scheduleOnce([&](float delta) {		// wait for main layer to hide completely

		// Turns on visibility
		_levelsLayerTop->setVisible(true);
		//_levelsLayerTop->resume();

		_levelsLayerBottom->setVisible(true);
		//_levelsLayerBottom->resume();


		showLevelsLayer();		// show layer

	}, DELAY_TIME_HIDE_MAIN_LAYER, "show_levels_on_moving_from_main");
}

void MenuScene1::onLevelsReturnCallback(cocos2d::Ref* sender, cocos2d::ui::Widget::TouchEventType type)
{
	if (movingScene) return;

	// Changing scene only if user release ON button
	if (type != ui::Widget::TouchEventType::ENDED)
	{
		return;
	}

	// Sets status -> transition between scene
	movingScene = true;



	// Sound
	AUDIO::play2d("challenge.ogg");

	// First, hides level selection layer
	hideLevelsLayer();

	scheduleOnce([&](float delta) {

		movingScene = false;


		// Hides and pauses level-selection layer's components
		_levelsLayerTop->setVisible(false);
		//_levelsLayerTop->pause();

		_levelsLayerBottom->setVisible(false);
		//_levelsLayerBottom->pause();

	}, DELAY_TIME_HIDE_LEVELS_LAYER, "hide_levels_on_return_to_main");

	// Then, shows main layer
	scheduleOnce([&](float delta) {

		_mainLayerTop->setVisible(true);

		_mainLayerBottom->setVisible(true);
		_mainLayerBottom->resume();

		showMainLayer();	// show layer

	}, DELAY_TIME_HIDE_LEVELS_LAYER, "show_main_on_return_from_levels");
}

void MenuScene1::onSkillsReturnCallback(cocos2d::Ref* sender, cocos2d::ui::Widget::TouchEventType type)
{
	if (movingScene) return;

	// Changing scene only if user release ON button
	if (type != ui::Widget::TouchEventType::ENDED)
	{
		return;
	}

	// Sets status -> transition between scene
	movingScene = true;



	// Sound
	AUDIO::play2d("challenge.ogg");


	// Saves all values into UserDefault (skills-pattern and Player's legacy)
	saveValues();

	// Erases (currently) selected icon
	if (_selectedIcon != nullptr)
	{
		_selectedIcon->disableGlowEffect();
		_selectedIcon = nullptr;
	}


	// First, hides upgrade-skills layer
	hideSkillsLayer();

	scheduleOnce([&](float delta) {

		_skillsLayerTop->setVisible(false);

		_skillsLayerBottom->setVisible(false);
		_skillsLayerBottom->pause();

		movingScene = false;

	}, DELAY_TIME_HIDE_SKILLS_LAYER, "hide_skills_on_return_to_main");


	// Then, shows main layer
	scheduleOnce([&](float delta) {

		_mainLayerTop->setVisible(true);

		_mainLayerBottom->setVisible(true);
		_mainLayerBottom->resume();

		showMainLayer();	// show layer

	}, DELAY_TIME_HIDE_SKILLS_LAYER, "show_main_on_return_from_skills");
}


// ---------------------------------- MAIN LAYER HELPER FUNCTIONS --------------------------------------

void MenuScene1::hideMainLayer()
{
	auto visibleSize = Director::getInstance()->getVisibleSize();
	
	// Actions for TOP + BOTTOM components
	auto moveUp = MoveBy::create(DELAY_TIME_HIDE_MAIN_LAYER, Vec3(0, visibleSize.height / 2, 0));

	auto topAction = EaseBackIn::create(moveUp->clone());
	auto botAction = EaseBackIn::create(moveUp->reverse());


	// Executes action
	_mainLayerTop->runAction(topAction);
	_mainLayerBottom->runAction(botAction);
}

void MenuScene1::showMainLayer()
{
	auto visibleSize = Director::getInstance()->getVisibleSize();
	
	// Actions for TOP + BOTTOM components
	auto moveDown = MoveBy::create(DELAY_TIME_SHOW_MAIN_LAYER, Vec3(0, -visibleSize.height / 2, 0));

	auto topAction = EaseBackOut::create(moveDown->clone());
	auto botAction = EaseBackOut::create(moveDown->reverse());


	// Executes action
	_mainLayerTop->runAction(topAction);
	_mainLayerBottom->runAction(botAction);
}

// ----------------------------------- END OF SECTION -------------------------------------------



// ------------------------------------ LEVEL SELECTION LAYER HELPER FUNCTIONS ------------------------------------

void MenuScene1::hideLevelsLayer()
{
	auto visibleSize = Director::getInstance()->getVisibleSize();


	// Actions for TOP + BOTTOM components
	auto fadeOut = FadeOut::create(DELAY_TIME_HIDE_LEVELS_LAYER);
	auto moveUp = MoveBy::create(0, Vec3(0, visibleSize.height / 2, 0));

	auto topAction = Sequence::create(fadeOut->clone(), moveUp->clone(), nullptr);
	auto botAction = Sequence::create(fadeOut->clone(), moveUp->reverse(), nullptr);


	// Executes action
	for (auto child : _levelsLayerTop->getChildren())
	{
		child->runAction(topAction->clone());
	}

	for (auto child : _levelsLayerBottom->getChildren())
	{
		child->runAction(botAction->clone());
	}
}

void MenuScene1::showLevelsLayer()
{
	auto visibleSize = Director::getInstance()->getVisibleSize();


	// Actions for TOP + BOTTOM components
	auto fadeIn = FadeIn::create(DELAY_TIME_HIDE_LEVELS_LAYER);
	auto moveDown = MoveBy::create(0, Vec3(0, -visibleSize.height / 2, 0));

	auto topAction = Sequence::create(moveDown->clone(), fadeIn->clone(), nullptr);
	auto botAction = Sequence::create(moveDown->reverse(), fadeIn->clone(), nullptr);

	// Executes action
	for (auto child : _levelsLayerTop->getChildren())
	{
		child->runAction(topAction->clone());
	}

	for (auto child : _levelsLayerBottom->getChildren())
	{
		child->runAction(botAction->clone());
	}
}

void MenuScene1::onLevelStageCallback(cocos2d::Ref* sender, cocos2d::ui::Widget::TouchEventType type)
{
	// Handles BEGAN click only
	if (type != ui::Widget::TouchEventType::BEGAN)
	{
		return;
	}

	// Extract the selected map
	auto mapTag = dynamic_cast<ui::Button*>(sender)->getTag();

	switch (mapTag)
	{
	case PROLOGUE_TUTORIAL_TAG:
		_selectedMap = "prologue_tutorial.tmx";
		break;
	case PROLOGUE_STAGE1_TAG:
		_selectedMap = "prologue_stage1.tmx";
		break;
	case PROLOGUE_STAGE2_TAG:
		_selectedMap = "prologue_stage2.tmx";
		break;
	case PROLOGUE_STAGE3_TAG:
		_selectedMap = "prologue_stage3.tmx";
		break;
	default:
		break;
	}

	// ============  Moves to the selected map  ================
	auto nextScene = CharacterSelectionScene::createScene(_selectedMap);

	// notify that "do not delete the res manager"
	ResourceManager::getInstance()->readyDelete = false;

	Director::getInstance()->replaceScene(TransitionFade::create(TRANSITION_TIME_LEVEL_STAGE_CALLBACK, nextScene));
}

// ----------------------------------------- END OF SECTION -----------------------------------------




// -------------------------------------- UPGRADE SKILLS LAYER HELPER FUNCTIONS ------------------------------

void MenuScene1::showSkillsLayer()
{
	auto visibleSize = Director::getInstance()->getVisibleSize();


	// Actions for TOP + BOTTOM components
	auto moveDown = MoveBy::create(DELAY_TIME_SHOW_SKILLS_LAYER, Vec3(0, -visibleSize.height / 2, 0));

	auto topAction = EaseBackOut::create(moveDown->clone());
	auto botAction = EaseBackOut::create(moveDown->reverse());


	// Executes action
	_skillsLayerTop->runAction(topAction);
	_skillsLayerBottom->runAction(botAction);
}

void MenuScene1::hideSkillsLayer()
{
	auto visibleSize = Director::getInstance()->getVisibleSize();


	// Actions for TOP + BOTTOM components
	auto moveUp = MoveBy::create(DELAY_TIME_HIDE_SKILLS_LAYER, Vec3(0, visibleSize.height / 2, 0));

	auto topAction = EaseBackIn::create(moveUp->clone());
	auto botAction = EaseBackIn::create(moveUp->reverse());


	// Additions: hides the description box IMMEDIATELY
	_skillsLayerBottom->setVisible(false);


	// Executes action
	_skillsLayerTop->runAction(topAction);
	_skillsLayerBottom->runAction(botAction);
}

void MenuScene1::onIconSkillCallback(cocos2d::Ref* sender, cocos2d::ui::Widget::TouchEventType type)
{
	// Handles BEGAN click only
	if (type != ui::Widget::TouchEventType::BEGAN)
	{
		return;
	}



	// SOUND
	AUDIO::play2d("ST01_00_00002.ogg");

	// DISABLES effect for the old IconSkill
	if (_selectedIcon != nullptr)
	{
		_selectedIcon->disableGlowEffect();
	}


	// RETRIEVES the (newly) selected IconSkill
	auto buttonName = dynamic_cast<ui::RadioButton*>(sender)->getName();
	_selectedIcon = dynamic_cast<IconSkill*>(_skillsLayerTop->getChildByName("skills_set")->getChildByName(buttonName));

	auto selectedSkill = _selectedIcon->getSkillType();


	// ENABLES effect
	_selectedIcon->enableGlowEffect();


	// SHOWS title of the skill
	auto descTitle = dynamic_cast<Label*>(_skillsLayerBottom->getChildByName("description_title_box"));
	descTitle->setString(_selectedIcon->getTitle());

	// Color for TITLE
	int skillIndex;
	if (selectedSkill >= Projectile::PROJECTILE_TYPE::FIRE0 && selectedSkill <= Projectile::PROJECTILE_TYPE::FIRE4)
	{
		descTitle->setTextColor(Color4B::RED);

		// In addition => compute skill index
		skillIndex = (int)selectedSkill - (int)Projectile::PROJECTILE_TYPE::FIRE0;
	}
	else if (selectedSkill >= Projectile::PROJECTILE_TYPE::ICE0 && selectedSkill <= Projectile::PROJECTILE_TYPE::ICE4)
	{
		descTitle->setTextColor(Color4B::BLUE);

		// In addition => compute skill index
		skillIndex = (int)selectedSkill - (int)Projectile::PROJECTILE_TYPE::ICE0;
	}
	else if (selectedSkill >= Projectile::PROJECTILE_TYPE::ELECTRIC0 && selectedSkill <= Projectile::PROJECTILE_TYPE::ELECTRIC4)
	{
		descTitle->setTextColor(Color4B::ORANGE);

		// In addition => compute skill index
		skillIndex = (int)selectedSkill - (int)Projectile::PROJECTILE_TYPE::ELECTRIC0;
	}


	// SHOWS description summary of the skill
	auto descSummary = dynamic_cast<Label*>(_skillsLayerBottom->getChildByName("description_summary_box"));
	descSummary->setString(_selectedIcon->getSummary());


	// SETS description panel to VISIBLE
	_skillsLayerBottom->setVisible(true);
	_skillsLayerBottom->resume();


	// SETS price to unlock the skill
	auto price = dynamic_cast<Label*>(_skillsLayerBottom->getChildByName("unlock_price_label"));

	char priceStr[10];
	sprintf(priceStr, "%d", _selectedIcon->getUnlockPrice());
	price->setString(priceStr);


	// IF the skill is already unlocked => DISABLES the unlock button
	// IF Player's current LEGACY is not enough to unlock the skill => DISABLES the unlock button
	auto unlockButton = dynamic_cast<ui::Button*>(_skillsLayerBottom->getChildByName("unlock_button_template"));

	//auto skillIndex = buttonName.at(buttonName.size() - 1) - '0';		// index of the selected skill
	auto key = 16;		// 0x10000
	while (skillIndex-- > 0)
	{
		key = key >> 1;
	}

	// Fire skills
	if (selectedSkill >= Projectile::PROJECTILE_TYPE::FIRE0 && selectedSkill <= Projectile::PROJECTILE_TYPE::FIRE4)
	{
		// ALREADY unlocked => disable
		if (key & _redDinoSkillsPattern)
		{
			unlockButton->setBright(false);
		}

		// NOT unlocked
		else
		{
			// ENOUGH LEGACY to unlock => enable
			if (_currentLegacy >= _selectedIcon->getUnlockPrice())
			{
				unlockButton->setBright(true);
				unlockButton->resume();
			}
			else
			{
				unlockButton->setBright(false);
				unlockButton->pause();
			}			
		}
	}

	// Ice skills
	else if (selectedSkill >= Projectile::PROJECTILE_TYPE::ICE0 && selectedSkill <= Projectile::PROJECTILE_TYPE::ICE4)
	{
		// ALREADY unlocked => disable
		if (key & _whiteDinoSkillsPattern)
		{
			unlockButton->setBright(false);
		}

		// NOT unlocked
		else
		{
			// ENOUGH LEGACY to unlock => enable
			if (_currentLegacy >= _selectedIcon->getUnlockPrice())
			{
				unlockButton->setBright(true);
				unlockButton->resume();
			}
			else
			{
				unlockButton->setBright(false);
				unlockButton->pause();
			}
		}
	}

	// Electric skills
	else if (selectedSkill >= Projectile::PROJECTILE_TYPE::ELECTRIC0 && selectedSkill <= Projectile::PROJECTILE_TYPE::ELECTRIC4)
	{
		// ALREADY unlocked => disable
		if (key & _yellowDinoSkillsPattern)
		{
			unlockButton->setBright(false);
		}
		
		// NOT unlocked
		else
		{
			// ENOUGH LEGACY to unlock => enable
			if (_currentLegacy >= _selectedIcon->getUnlockPrice())
			{
				unlockButton->setBright(true);
				unlockButton->resume();
			}
			else
			{
				unlockButton->setBright(false);
				unlockButton->pause();
			}
		}
	}
}

void MenuScene1::onViewCallback(cocos2d::Ref* sender, cocos2d::ui::Widget::TouchEventType type)
{
	// Handles ENDED click only
	if (type != ui::Widget::TouchEventType::ENDED)
	{
		return;
	}


	
	// Disables view button
	auto viewButton = dynamic_cast<ui::Button*>(sender);

	viewButton->setBright(false);
	viewButton->pause();


	// Changes description summary into button description
	auto descriptionSummaryBox = dynamic_cast<Label*>(_skillsLayerBottom->getChildByName("description_summary_box"));

	descriptionSummaryBox->setString(_selectedIcon->getButtonInstruction());


	// Creates new layer for tutorial
	if (_skillsLayerView == nullptr)
	{
		auto visibleSize = Director::getInstance()->getVisibleSize();
		auto visibleOrigin = Director::getInstance()->getVisibleOrigin();

		auto paddingY = visibleSize.height / 20;

		auto descBackground = _skillsLayerBottom->getChildByName("description_background");


		// Cover layer
		_skillsLayerView = LayerColor::create(Color4B::BLACK);

		_skillsLayerView->setOpacity(200);

		this->addChild(_skillsLayerView, SKILL_VIEW_LAYER_Z_ORDER);


		// EventListener to swallow touches
		auto listener = EventListenerTouchOneByOne::create();

		listener->setSwallowTouches(true);
		listener->onTouchBegan = [](Touch*, Event*)->bool {

			return true;

		};

		_eventDispatcher->addEventListenerWithSceneGraphPriority(listener, _skillsLayerView);


		// Image tutorial
		auto width = _skillsLayerBottom->getChildByName("description_background")->getBoundingBox().size.width;
		auto height = visibleSize.height - descBackground->getBoundingBox().getMaxY() - paddingY * 2;

		auto imageTutorial = _selectedIcon->getImageTutorial();

		imageTutorial->setName("image_tutorial");

		imageTutorial->setScaleX(width / imageTutorial->getContentSize().width);
		imageTutorial->setScaleY(height / imageTutorial->getContentSize().height);

		imageTutorial->setAnchorPoint(Vec2(0.5, 0));
		imageTutorial->setPosition(Vec2(visibleOrigin.x + visibleSize.width / 2, descBackground->getBoundingBox().getMaxY() + paddingY));

		_skillsLayerView->addChild(imageTutorial, 1);


		// Close button
		auto closeButton = ui::Button::create("close.png");

		closeButton->setName("close_image_tutorial_button");
		closeButton->setScale(SCALE_CLOSE_BUTTON);
		closeButton->setAnchorPoint(Vec2(1, 1));
		closeButton->setPosition(Vec2(imageTutorial->getBoundingBox().getMaxX(), imageTutorial->getBoundingBox().getMaxY()));
		closeButton->addTouchEventListener(CC_CALLBACK_2(MenuScene1::onCloseCallback, this));

		_skillsLayerView->addChild(closeButton, 2);


		CC_SAFE_RETAIN(_skillsLayerView);
	}
	else
	{
		auto visibleSize = Director::getInstance()->getVisibleSize();
		auto visibleOrigin = Director::getInstance()->getVisibleOrigin();

		auto paddingY = visibleSize.height / 20;

		auto descBackground = _skillsLayerBottom->getChildByName("description_background");


		this->addChild(_skillsLayerView, SKILL_VIEW_LAYER_Z_ORDER);


		// Re-sets image tutorial
		_skillsLayerView->removeChildByName("image_tutorial");

		auto width = _skillsLayerBottom->getChildByName("description_background")->getBoundingBox().size.width;
		auto height = visibleSize.height - descBackground->getBoundingBox().getMaxY() - paddingY * 2;

		auto imageTutorial = _selectedIcon->getImageTutorial();

		imageTutorial->setName("image_tutorial");

		imageTutorial->setScaleX(width / imageTutorial->getContentSize().width);
		imageTutorial->setScaleY(height / imageTutorial->getContentSize().height);

		imageTutorial->setAnchorPoint(Vec2(0.5, 0));
		imageTutorial->setPosition(Vec2(visibleOrigin.x + visibleSize.width / 2, descBackground->getBoundingBox().getMaxY() + paddingY));

		_skillsLayerView->addChild(imageTutorial, 1);
	}
}

void MenuScene1::onUnlockCallback(cocos2d::Ref* sender, cocos2d::ui::Widget::TouchEventType type)
{
	// Handles ENDED click only
	if (type != ui::Widget::TouchEventType::ENDED)
	{
		return;
	}



	// UNLOCKS the selected skill
	_selectedIcon->unlockSkill();


	// UPDATES the skills pattern

	int skillIndex, *selectedPattern = nullptr;
	auto selectedSkill = _selectedIcon->getSkillType();
	if (selectedSkill >= Projectile::PROJECTILE_TYPE::FIRE0 && selectedSkill <= Projectile::PROJECTILE_TYPE::FIRE4)
	{
		// Index
		skillIndex = (int)selectedSkill - (int)Projectile::PROJECTILE_TYPE::FIRE0;

		// Pattern
		selectedPattern = &_redDinoSkillsPattern;
	}
	else if (selectedSkill >= Projectile::PROJECTILE_TYPE::ICE0 && selectedSkill <= Projectile::PROJECTILE_TYPE::ICE4)
	{
		// Index
		skillIndex = (int)selectedSkill - (int)Projectile::PROJECTILE_TYPE::ICE0;

		// Pattern
		selectedPattern = &_whiteDinoSkillsPattern;
	}
	else if (selectedSkill >= Projectile::PROJECTILE_TYPE::ELECTRIC0 && selectedSkill <= Projectile::PROJECTILE_TYPE::ELECTRIC4)
	{
		// Index
		skillIndex = (int)selectedSkill - (int)Projectile::PROJECTILE_TYPE::ELECTRIC0;

		// Pattern
		selectedPattern = &_yellowDinoSkillsPattern;
	}

	auto key = 16;		// 0x10000
	while (skillIndex-- > 0)
	{
		key = key >> 1;
	}

	*selectedPattern = *selectedPattern | key;


	// DISABLES the Unlock button for the current skill
	auto unlockButton = dynamic_cast<ui::Button*>(_skillsLayerBottom->getChildByName("unlock_button_template"));

	unlockButton->setBright(false);
	unlockButton->pause();


	// UPDATES Player's legacy
	_currentLegacy -= _selectedIcon->getUnlockPrice();

	auto legacyLabel = dynamic_cast<Label*>(_skillsLayerTop->getChildByName("legacy_indication_label"));

	char legacyStr[10];
	sprintf(legacyStr, "%06d", _currentLegacy);

	legacyLabel->setString(legacyStr);
}

void MenuScene1::onCloseCallback(cocos2d::Ref* sender, cocos2d::ui::Widget::TouchEventType type)
{
	// Enables view button
	auto viewButton = dynamic_cast<ui::Button*>(_skillsLayerBottom->getChildByName("view_button_template"));

	viewButton->setBright(true);
	viewButton->resume();


	// Resets button instructions into description summary
	auto descriptionSummaryBox = dynamic_cast<Label*>(_skillsLayerBottom->getChildByName("description_summary_box"));

	descriptionSummaryBox->setString(_selectedIcon->getSummary());


	// Removes image tutorial
	this->removeChild(_skillsLayerView);
}

// --------------------------------------------- END OF SECTION -----------------------------------------------



void MenuScene1::update(float delta)
{

}

void MenuScene1::saveValues()
{
	// The values has been saved or not?
	if (!_hasSaved)
	{
		// Saves the skills pattern
		UserDefault::getInstance()->setIntegerForKey("red_dino_skills_set", _redDinoSkillsPattern);
		UserDefault::getInstance()->setIntegerForKey("white_dino_skills_set", _whiteDinoSkillsPattern);
		UserDefault::getInstance()->setIntegerForKey("yellow_dino_skills_set", _yellowDinoSkillsPattern);

		// Saves Player's legacy
		UserDefault::getInstance()->setIntegerForKey("player_legacy", _currentLegacy);

		UserDefault::getInstance()->flush();


		// Turns off the flag
		_hasSaved = true;
	}
}
