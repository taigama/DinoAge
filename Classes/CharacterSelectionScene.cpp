#include "CharacterSelectionScene.h"

#include "PlayScene.h"
#include "MenuScene1.h"
#include "Projectile.h"

#include "ResourceManager.h"

//#include "AudioEngine.h"
//#define AUDIO experimental::AudioEngine

#define SCALE_START_BUTTON 0.16f
#define SCALE_MENU_BUTTON 1.2f
#define SCALE_BACKGROUND_IMAGE 1.5f
#define SCALE_YELLOW_DINO_SPRITE 0.8f
#define SCALE_WHITE_DINO_SPRITE 1.1f

#define DELAY_TIME_COME_ONLINE 0.5f
#define DELAY_TIME_HIDE 0.5f
#define DURATION_SHOW 0.5f

#define TYRO_ANIMATION_TAG 1
#define TRINO_ANIMATION_TAG 2
#define PTERO_ANIMATION_TAG 3

#define NUMBER_OF_SKILLS 5

#define DESCRIPTION_SUMMARY_FONT_SIZE 14.0f
#define DESCRIPTION_TITLE_FONT_SIZE 20.0f

#define DESCRIPTION_TITLE_SIZE_PERCENT 0.4f
#define DESCRIPTION_SUMMARY_SIZE_PERCENT 0.6f

#define START_BUTTON_ACTION_TAG 5

USING_NS_CC;


CharacterSelectionScene::~CharacterSelectionScene()
{

	//CC_SAFE_RELEASE_NULL(_background00);
	//CC_SAFE_RELEASE_NULL(_background01);

	CC_SAFE_RELEASE_NULL(_tyroAnimation);
	CC_SAFE_RELEASE_NULL(_trinoAnimation);
	CC_SAFE_RELEASE_NULL(_pteroAnimation);
}

Scene* CharacterSelectionScene::createScene()
{
	auto scene = Scene::create();

	auto layer = CharacterSelectionScene::create();

	scene->addChild(layer);

	return scene;
}

Scene* CharacterSelectionScene::createScene(std::string startMap)
{
	auto scene = Scene::create();

	auto layer = CharacterSelectionScene::create();

	layer->setStartMap(startMap);

	scene->addChild(layer);

	return scene;
}

void CharacterSelectionScene::setStartMap(std::string startMap)
{
	if (startMap.compare("prologue_tutorial.tmx") == 0)
	{
		_startMap = startMap;
		_startLevel = "Prologue";
		_startStage = "Tutorial";
	}
	else if (startMap.compare("prologue_stage1.tmx") == 0)
	{
		_startMap = startMap;
		_startLevel = "Prologue";
		_startStage = "Stage 1";
	}
	else if (startMap.compare("prologue_stage2.tmx") == 0)
	{
		_startMap = startMap;
		_startLevel = "Prologue";
		_startStage = "Stage 2";
	}
	else if (startMap.compare("prologue_stage3.tmx") == 0)
	{
		_startMap = startMap;
		_startLevel = "Prologue";
		_startStage = "Stage 3";
	}
	else
	{
		CCASSERT(1 == 0, "Invalid map name passed in CharacterSelectionScene");
	}
}

bool CharacterSelectionScene::init()
{
	//////////////////////////////
	// 1. Super init
	if (!Layer::init())
	{
		return false;
	}

	scheduleUpdate();


	auto visibleSize = Director::getInstance()->getVisibleSize();
	auto visibleOrigin = Director::getInstance()->getVisibleOrigin();

	auto center = visibleOrigin + Vec2(visibleSize.width / 2, visibleSize.height / 2);

	float padding = visibleSize.width / 20;


	//////////////////////////
	// 2. Background image
	_background00 = Sprite::create("background00.png");
	_background01 = Sprite::create("background01.png");

	_background00->setAnchorPoint(Vec2(0.5, 0.5));
	_background00->setPosition(center);
	_background00->setScale(SCALE_BACKGROUND_IMAGE);

	_background01->setAnchorPoint(Vec2(0.5, 0.5));
	_background01->setPosition(center - Vec2(_background00->getBoundingBox().size.width, 0));
	_background01->setScale(SCALE_BACKGROUND_IMAGE);

	// Moves these two backgrounds
	auto moveBy = MoveBy::create(2.0f, Vec3(visibleSize.width * 0.8f, 0, 0));

	_background00->runAction(RepeatForever::create(moveBy->clone()));
	_background01->runAction(RepeatForever::create(moveBy->clone()));

	// Adds to scene
	this->addChild(_background00, -1);
	this->addChild(_background01, -1);


	//////////////////////////////////////////////////////
	// 3. Display representing image for each character
	auto selectionPos = Vec2(center.x, _background00->getBoundingBox().getMaxY() - padding / 3);

	// Only ONE character can be chosen (at at time) to proceed -> use RadioButtonGroup to handle
	auto characterGroup = ui::RadioButtonGroup::create();

	characterGroup->setName("selection_group");
	characterGroup->setPosition(Vec2::ZERO);

	this->addChild(characterGroup, 0);

	// "Background" button for each character
	auto tyroButton = ui::RadioButton::create("button_normal.png", "button_selected.png");
	auto trinoButton = dynamic_cast<ui::RadioButton*>(tyroButton->clone());
	auto pteroButton = dynamic_cast<ui::RadioButton*>(tyroButton->clone());

	auto spritecache = SpriteFrameCache::getInstance();

	// Sprite image for each character, and their animations
	auto tyroSprite = Sprite::createWithSpriteFrameName("tyro_idle0.png");		// initial sprite
	auto trinoSprite = Sprite::createWithSpriteFrameName("trino_idle0.png");
	auto pteroSprite = Sprite::createWithSpriteFrameName("yellow_dino_headup0.png");


	// Animations
	char str[50];
	
	Vector<SpriteFrame*> animFrames;
	Animation* animation;

	// Tyro animation
	for (int i = 3; i < 7; ++i)			
	{
		sprintf(str, "tyro_attack%1d.png", i);
		animFrames.pushBack(spritecache->getSpriteFrameByName(str));
	}
	animFrames.pushBack(spritecache->getSpriteFrameByName("tyro_idle0.png"));
	animation = Animation::createWithSpriteFrames(animFrames, 0.1f);

	_tyroAnimation = Animate::create(animation);		// animation for tyro (RedDino)
	_tyroAnimation->setTag(TYRO_ANIMATION_TAG);
	CC_SAFE_RETAIN(_tyroAnimation);


	// Trino animation
	animFrames.clear();
	for (int i = 1; i < 7; ++i)
	{
		sprintf(str, "trino_attack%1d.png", i);
		animFrames.pushBack(spritecache->getSpriteFrameByName(str));
	}
	animFrames.pushBack(spritecache->getSpriteFrameByName("trino_idle0.png"));
	animation = Animation::createWithSpriteFrames(animFrames, 0.1f);

	_trinoAnimation = Animate::create(animation);		// animation for trino (WhiteDino)
	_trinoAnimation->setTag(TRINO_ANIMATION_TAG);
	CC_SAFE_RETAIN(_trinoAnimation);


	// Ptero animation
	animFrames.clear();
	for (int i = 0; i < 5; ++i)
	{
		sprintf(str, "yellow_dino_headup%d.png", i);
		animFrames.pushBack(spritecache->getSpriteFrameByName(str));
	}
	animation = Animation::createWithSpriteFrames(animFrames, 0.1f);

	_pteroAnimation = Animate::create(animation);		// animation for ptero (yellowDino)
	_pteroAnimation->setTag(PTERO_ANIMATION_TAG);
	CC_SAFE_RETAIN(_pteroAnimation);



	// Setup position for RedDino
	tyroButton->setAnchorPoint(Vec2(0.5, 1));
	tyroButton->setPosition(selectionPos);
	tyroButton->setOpacity(0);
	tyroButton->addTouchEventListener([this](Ref*, ui::Widget::TouchEventType type) {

		if (type != ui::Widget::TouchEventType::BEGAN) return;

		// Retrieves sprite for RedDino
		auto sprite = this->getChildByName("selection_group")->getChildByName("tyro_sprite");

		// Executes animation
		if (sprite->getActionByTag(TYRO_ANIMATION_TAG) != nullptr)
		{
			sprite->stopActionByTag(TYRO_ANIMATION_TAG);
		}
		sprite->runAction(_tyroAnimation);

		if (_playerSelection != SELECTION::RED_DINO)
		{
			// Changes currently selected character
			_playerSelection = SELECTION::RED_DINO;

			// Changes the currenly displayed skillset
			changeSkillSet();
		}

		//SimpleAudioEngine::getInstance()->playEffect("Button.ogg");
		AUDIO::play2d("Button.ogg");
	});

	tyroSprite->setName("tyro_sprite");
	tyroSprite->setAnchorPoint(Vec2(0.5, 0.5));
	tyroSprite->setPosition(Vec2(tyroButton->getBoundingBox().getMidX(), tyroButton->getBoundingBox().getMidY()));
	tyroSprite->setOpacity(0);
	

	// Setup position for WhiteDino
	trinoButton->setAnchorPoint(Vec2(0.5, 1));
	trinoButton->setPosition(selectionPos + Vec2(tyroButton->getContentSize().width + padding, 0));
	trinoButton->setOpacity(0);
	trinoButton->addTouchEventListener([this](Ref*, ui::Widget::TouchEventType type) {

		if (type != ui::Widget::TouchEventType::BEGAN) return;

		// Retrieves sprite for WhiteDino
		auto sprite = this->getChildByName("selection_group")->getChildByName("trino_sprite");


		// Executes animation
		if (sprite->getActionByTag(TRINO_ANIMATION_TAG) != nullptr)
		{
			sprite->stopActionByTag(TRINO_ANIMATION_TAG);
		}
		sprite->runAction(_trinoAnimation);

		if (_playerSelection != SELECTION::WHITE_DINO)
		{
			// Changes currently selected character
			_playerSelection = SELECTION::WHITE_DINO;

			// Changes the currenly displayed skillset
			changeSkillSet();
		}

		//SimpleAudioEngine::getInstance()->playEffect("Button.ogg");
		AUDIO::play2d("Button.ogg");
	});

	trinoSprite->setName("trino_sprite");
	trinoSprite->setScale(SCALE_WHITE_DINO_SPRITE);
	trinoSprite->setAnchorPoint(Vec2(0.5, 0.5));
	trinoSprite->setPosition(Vec2(trinoButton->getBoundingBox().getMidX(), trinoButton->getBoundingBox().getMidY()));
	trinoSprite->setOpacity(0);


	// Setup position for YellowDino
	pteroButton->setAnchorPoint(Vec2(0.5, 1));
	pteroButton->setPosition(selectionPos - Vec2(tyroButton->getContentSize().width + padding, 0));
	pteroButton->setOpacity(0);
	pteroButton->addTouchEventListener([this](Ref*, ui::Widget::TouchEventType type) {

		if (type != ui::Widget::TouchEventType::BEGAN) return;

		// Retrieves sprite for YellowDino
		auto sprite = this->getChildByName("selection_group")->getChildByName("ptero_sprite");


		// Executes animation
		if (sprite->getActionByTag(PTERO_ANIMATION_TAG) != nullptr)
		{
			sprite->stopActionByTag(PTERO_ANIMATION_TAG);
		}
		sprite->runAction(_pteroAnimation);

		if (_playerSelection != SELECTION::YELLOW_DINO)
		{
			// Changes currently selected character
			_playerSelection = SELECTION::YELLOW_DINO;

			// Changes the currenly displayed skillset
			changeSkillSet();
		}

		//SimpleAudioEngine::getInstance()->playEffect("Button.ogg");
		AUDIO::play2d("Button.ogg");
	});

	pteroSprite->setName("ptero_sprite");
	pteroSprite->setScale(SCALE_YELLOW_DINO_SPRITE);
	pteroSprite->setAnchorPoint(Vec2(0.5, 0.5));
	pteroSprite->setPosition(Vec2(pteroButton->getBoundingBox().getMidX(), pteroButton->getBoundingBox().getMidY()));
	pteroSprite->setOpacity(0);

	
	// Adds to RadioButtonGroup
	characterGroup->addRadioButton(tyroButton);
	characterGroup->addChild(tyroButton, 0);
	characterGroup->addChild(tyroSprite, 1);

	characterGroup->addRadioButton(trinoButton);
	characterGroup->addChild(trinoButton, 0);
	characterGroup->addChild(trinoSprite, 1);

	characterGroup->addRadioButton(pteroButton);
	characterGroup->addChild(pteroButton, 0);
	characterGroup->addChild(pteroSprite, 1);

	characterGroup->setSelectedButton(tyroButton);		// default selection: RED_DINO


	/////////////////////////////////////////////
	// 4. Start button -> Use MenuItemSprite

	auto buttonPos = Vec2(visibleOrigin.x + visibleSize.width, _background00->getBoundingBox().getMaxY());

	auto startButton = Sprite::createWithSpriteFrameName("play_frame0.png");

	startButton->setName("start_button");
	startButton->setAnchorPoint(Vec2(1, 1));
	startButton->setScale(SCALE_START_BUTTON);
	startButton->setPosition(buttonPos);
	startButton->setOpacity(0);

	this->addChild(startButton, 0);


	//////////////////////////////////////////////
	// 5. Menu button -> returns to main menu

	auto menuPos = Vec2(visibleOrigin.x + padding / 8, _background00->getBoundingBox().getMaxY() - padding / 8);

	auto menu = ui::Button::create("menu.png");

	menu->setName("menu");
	menu->setScale(SCALE_MENU_BUTTON);
	menu->setOpacity(0);
	menu->setAnchorPoint(Vec2(0, 1));
	menu->setPosition(menuPos);

	this->addChild(menu);


	/////////////////////////////////////////////////////////
	// 6. Displays the skill set of the selected character
	auto skillsPos = Vec2(visibleOrigin.x + visibleSize.width / 2, tyroButton->getBoundingBox().getMinY() - padding / 1.5);

	auto skillSet = ui::RadioButtonGroup::create();

	skillSet->setName("skill_set");

	this->addChild(skillSet);

	// Retrieves the state of skill set of the user (as pattern - e.g. 0x11000)
	auto skillPattern = UserDefault::getInstance()->getIntegerForKey("red_dino_skills_set", 31);	// default value = 0x11000
	auto key = 16;			// 0x10000

	// Default selected character = RED_DINOSAUR => Default skillset = FIRE skills
	IconSkill* icon;
	for (int i = 0; i < NUMBER_OF_SKILLS; ++i)
	{
		// Checks state of unlock of the current skill
		bool unlockState = skillPattern & key;
		key = key >> 1;		// prepare the key for the next skill

		icon = IconSkill::createIconSkill((Projectile::PROJECTILE_TYPE)(i + (int)Projectile::PROJECTILE_TYPE::FIRE0), unlockState);	// enum type for FIRE skills start with 100 (FIRE0)

		auto distance = icon->getBoundingBox().size.width + padding * 1.5;
		char str[7];

		sprintf(str, "fire%d\0", i);
		
		// Registers name for IconSkill object and its RadioButton property
		icon->setName(str);
		icon->getButton()->setName(str);

		icon->setAnchorPoint(Vec2(0.5, 1));
		icon->setPosition(skillsPos + Vec2(-2 * distance + i * distance, 0));
		icon->makeInvisible();

		skillSet->addRadioButton(icon->getButton());
		skillSet->addChild(icon);

		// When icon is clicked -> show description
		icon->getButton()->addTouchEventListener([&](Ref* sender, ui::Widget::TouchEventType type) {

			if (type == ui::Widget::TouchEventType::ENDED)
			{
				// Disables effect of the OLD selected IconSkill
				if (_selectedIcon != nullptr)
				{
					_selectedIcon->disableGlowEffect();
				}

				// Sets the selected IconSkill
				auto buttonName = dynamic_cast<ui::RadioButton*>(sender)->getName();
				_selectedIcon = dynamic_cast<IconSkill*>(this->getChildByName("skill_set")->getChildByName(buttonName));

				// Shows effect
				_selectedIcon->enableGlowEffect();

				//SimpleAudioEngine::getInstance()->playEffect("ST01_00_00002.ogg");
				AUDIO::play2d("ST01_00_00002.ogg");

				// Displays description
				displayDescription(_selectedIcon);
			}

		});
	}

	// Description panel
	auto paddingY = visibleSize.height / 20;

	auto size = Size(visibleSize.width * 0.65f, icon->getBoundingBox().getMinY() - _background00->getBoundingBox().getMinY() - padding);
	auto titleSize = Size(size.width - padding, (size.height - paddingY) * DESCRIPTION_TITLE_SIZE_PERCENT);
	auto summarySize = Size(titleSize.width, (size.height - paddingY * 0.5f) * DESCRIPTION_SUMMARY_SIZE_PERCENT);

	auto descBackgroundPosition = Vec2(visibleOrigin.x + visibleSize.width / 2, icon->getBoundingBox().getMinY() - padding / 2);

	auto summaryFontSize = DESCRIPTION_SUMMARY_FONT_SIZE / Director::getInstance()->getContentScaleFactor();
	auto titleFontSize = DESCRIPTION_TITLE_FONT_SIZE / Director::getInstance()->getContentScaleFactor();
	

	// Background for textbox
	auto backgroundText = Sprite::create("text_box1.png");

	auto scaleForX = size.width / backgroundText->getContentSize().width;
	auto scaleForY = size.height / backgroundText->getContentSize().height;

	backgroundText->setName("background_text");
	backgroundText->setScale(scaleForX, scaleForY);
	backgroundText->setAnchorPoint(Vec2(0.5, 1));
	backgroundText->setPosition(descBackgroundPosition);
	backgroundText->setVisible(false);

	this->addChild(backgroundText, 0);

	// Title box
	auto titleBox = Label::createWithTTF("EXAMPLE TITLE", "6809 chargen.ttf", titleFontSize);

	titleBox->setName("title_text");

	titleBox->setContentSize(titleSize);
	titleBox->setMaxLineWidth(titleSize.width);
	titleBox->setAlignment(TextHAlignment::LEFT, TextVAlignment::CENTER);

	titleBox->setAnchorPoint(Vec2(0, 1));
	titleBox->setPosition(Vec2(backgroundText->getBoundingBox().getMinX() + padding / 2, backgroundText->getBoundingBox().getMaxY() - padding / 4));

	titleBox->setVisible(false);

	this->addChild(titleBox, 1);


	// Summary box
	auto summaryBox = Label::createWithTTF("EXAMPLE SUMMARY", "verdanab.ttf", summaryFontSize);

	summaryBox->setName("summary_text");

	summaryBox->setContentSize(summarySize);
	summaryBox->setMaxLineWidth(summarySize.width);
	summaryBox->enableWrap(true);
	summaryBox->setAlignment(TextHAlignment::LEFT, TextVAlignment::CENTER);
	summaryBox->setTextColor(Color4B::BLACK);

	summaryBox->setAnchorPoint(Vec2(0, 1));
	summaryBox->setPosition(Vec2(titleBox->getBoundingBox().getMinX(), titleBox->getBoundingBox().getMinY()));

	summaryBox->setVisible(false);

	this->addChild(summaryBox, 1);
	

	/////////////////////////////////////////////////////////////////////////////
	// 7. Schedules the scene to "come-online" after a certain amount of time

	scheduleOnce([&](float delta) {

		auto group = dynamic_cast<ui::RadioButtonGroup*>(this->getChildByName("selection_group"));
		auto menu = dynamic_cast<ui::Button*>(this->getChildByName("menu"));
		auto startButton = dynamic_cast<Sprite*>(this->getChildByName("start_button"));
		auto skillSet = dynamic_cast<ui::RadioButtonGroup*>(this->getChildByName("skill_set"));

		auto action = FadeIn::create(DURATION_SHOW);

		auto max = group->getChildren().size() > skillSet->getChildren().size() ? group->getChildren().size() : skillSet->getChildren().size();

		for (int i = 0; i < max; ++i)
		{
			// IF buttons
			if (i < group->getChildren().size())
				group->getChildren().at(i)->runAction(action->clone());

			// IF skill icons
			if (i < skillSet->getChildren().size())
			{
				auto icon = dynamic_cast<IconSkill*>(skillSet->getChildren().at(i));
				icon->makeVisible(DURATION_SHOW);
			}
		}
		menu->runAction(action->clone());
		startButton->runAction(action->clone());

		menu->addTouchEventListener(CC_CALLBACK_2(CharacterSelectionScene::onMenuCallback, this));

		// EventListener for start button
		auto listener = EventListenerTouchOneByOne::create();
		listener->onTouchBegan = [](Touch* touch, Event* event) {

			return true;

		};
		listener->onTouchEnded = CC_CALLBACK_2(CharacterSelectionScene::onStartCallback, this);

		_eventDispatcher->addEventListenerWithSceneGraphPriority(listener, startButton);

	}, DELAY_TIME_COME_ONLINE, "come_online");

	// Schedules the start button to run action
	scheduleOnce([&](float delta) {

		//auto startButton = dynamic_cast<ui::Button*>(this->getChildByName("start_button"));
		auto startButton = dynamic_cast<Sprite*>(this->getChildByName("start_button"));

		Vector<SpriteFrame*> animFrames;
		auto spritecache = SpriteFrameCache::getInstance();

		animFrames.pushBack(spritecache->getSpriteFrameByName("play_frame0.png"));
		animFrames.pushBack(spritecache->getSpriteFrameByName("play_frame1.png"));

		auto animation = Animation::createWithSpriteFrames(animFrames, 0.3f);
		auto animate = Animate::create(animation);
		auto action = RepeatForever::create(animate);

		action->setTag(START_BUTTON_ACTION_TAG);

		startButton->runAction(action);

	}, DELAY_TIME_COME_ONLINE + 0.5f, "start_button_action");


	return true;
}

void CharacterSelectionScene::onStartCallback(cocos2d::Touch* touch, cocos2d::Event* event)
{
	// Avoids multiple-clicks
	if (movingScene)
	{
		return;
	}


	auto startButton = dynamic_cast<Sprite*>(this->getChildByName("start_button"));

	// If user DO NOT touch the start button (touch location not inside sprite) -> skip
	if (!startButton->getBoundingBox().containsPoint(touch->getLocation()))
	{
		return;
	}
	AUDIO::play2d("ST0E_U1_00001.ogg");

	movingScene = true;

	// Retrieves RadioButtonGroup
	auto group = dynamic_cast<ui::RadioButtonGroup*>(this->getChildByName("selection_group"));

	// Determine the selected Character
	int index = group->getSelectedButtonIndex();

	switch (index)
	{
	case -1:
		log("ERROR: NO CHARACTER SELECTED");
		break;
	default:
		if (_playerSelection != (SELECTION)(index))
		{
			_playerSelection = (SELECTION)(index);
			log("_playerSelection NOT POINTING TO CURRENTLY SELECTED CHARACTER");
		}
		break;
	}


	// Start the game
	hideCharSelectionScene();

	scheduleOnce([&](float delta) {

		// Resets
		movingScene = false;



		auto playScene = PlayScene::createScene(_playerSelection, _startMap, _startLevel, _startStage);
		Director::getInstance()->replaceScene(TransitionFade::create(1.0f, playScene));
		//Director::getInstance()->replaceScene(playScene);

	}, DELAY_TIME_HIDE, "start_game");
}

void CharacterSelectionScene::onMenuCallback(cocos2d::Ref* sender, cocos2d::ui::Widget::TouchEventType type)
{
	if (movingScene) return;

	// Changing scene only if user release ON button
	if (type != ui::Widget::TouchEventType::ENDED)
	{
		return;
	}

	AUDIO::play2d("challenge.ogg");

	movingScene = true;

	// Returns to the main menu
	hideCharSelectionScene();

	scheduleOnce([&](float delta) {

		// Resets
		movingScene = false;


		auto menuScene = MenuScene1::createScene();
		Director::getInstance()->replaceScene(TransitionFade::create(0.5f, menuScene));

	}, DELAY_TIME_HIDE, "return_menu");
}

void CharacterSelectionScene::hideCharSelectionScene()
{
	auto visibleSize = Director::getInstance()->getVisibleSize();
	auto padding = visibleSize.height / 40;

	// Retrieves components
	auto selectionGroup = this->getChildByName("selection_group");
	auto startButton = this->getChildByName("start_button");
	auto skillSet = this->getChildByName("skill_set");
	auto menu = this->getChildByName("menu");

	startButton->stopActionByTag(START_BUTTON_ACTION_TAG);


	// Actions (for top components)
	auto moveUp = MoveBy::create(DELAY_TIME_HIDE, Vec3(0, visibleSize.height / 2, 0));
	auto topAction = EaseBackIn::create(moveUp);

	// Executes actions
	selectionGroup->runAction(topAction->clone());
	startButton->runAction(topAction->clone());
	menu->runAction(topAction->clone());
	skillSet->runAction(topAction);


	// For bottom components (description box) -> hide immmediately
	this->getChildByName("background_text")->setVisible(false);
	this->getChildByName("title_text")->setVisible(false);
	this->getChildByName("summary_text")->setVisible(false);
}

void CharacterSelectionScene::update(float delta)
{
	// Keeps the two backgrounds moving infinitely
	if (_background00 != nullptr && _background01 != nullptr)
	{
		auto visibleOrigin = Director::getInstance()->getVisibleOrigin();
		auto visibleSize = Director::getInstance()->getVisibleSize();

		if (_background00->getBoundingBox().getMinX() > visibleOrigin.x + visibleSize.width)
		{
			_background00->setPosition(_background01->getPosition() - Vec2(_background01->getBoundingBox().size.width, 0));
		}
		else if (_background01->getBoundingBox().getMinX() > visibleOrigin.x + visibleSize.width)
		{
			_background01->setPosition(_background00->getPosition() - Vec2(_background00->getBoundingBox().size.width, 0));
		}
	}
}

void CharacterSelectionScene::changeSkillSet()
{
	auto key = 16;						// 0x10000

	// Disables effect for the (old) selected IconSkill
	if (_selectedIcon != nullptr)
	{
		_selectedIcon->disableGlowEffect();
	}

	// Hides description panel
	hideDescription();
	

	// RedDino character
	if (_playerSelection == SELECTION::RED_DINO)
	{
		// Retrieves pattern for skill set
		auto skillPattern = UserDefault::getInstance()->getIntegerForKey("red_dino_skills_set", 31);	// default value = 0x11000

		// Retrieves skillset
		auto skillSet = dynamic_cast<ui::RadioButtonGroup*>(this->getChildByName("skill_set"));

		// For each child icon
		int index = 0;
		for (Node* node : skillSet->getChildren())
		{
			auto icon = dynamic_cast<IconSkill*>(node);

			// Checks unlock state
			bool unlockState = skillPattern & key;
			key = key >> 1;

			// Changes its properties accordingly
			char str[10];
			sprintf(str, "fire%d\0", index);

			icon->setName(str);							// name
			icon->getButton()->setName(str);

			icon->reloadTextures((Projectile::PROJECTILE_TYPE)(index + (int)Projectile::PROJECTILE_TYPE::FIRE0), unlockState);		// reloads textures
			icon->setSkillType((Projectile::PROJECTILE_TYPE)(index + (int)Projectile::PROJECTILE_TYPE::FIRE0));		// re-sets type of skill

			icon->setDescription();		// re-sets description

			++index;
		}
	}

	// WhiteDino character
	else if (_playerSelection == SELECTION::WHITE_DINO)
	{
		// Retrieves pattern for skill set
		auto skillPattern = UserDefault::getInstance()->getIntegerForKey("white_dino_skills_set", 31);		// default value = 0x11000

		// Retrieves skillset
		auto skillSet = dynamic_cast<ui::RadioButtonGroup*>(this->getChildByName("skill_set"));

		// For each child icon
		int index = 0;
		for (Node* node : skillSet->getChildren())
		{
			auto icon = dynamic_cast<IconSkill*>(node);

			// Checks unlock state
			bool unlockState = skillPattern & key;
			key = key >> 1;

			// Changes its properties accordingly
			char str[10];
			sprintf(str, "ice%d\0", index);

			icon->setName(str);							// name
			icon->getButton()->setName(str);

			icon->reloadTextures((Projectile::PROJECTILE_TYPE)(index + (int)Projectile::PROJECTILE_TYPE::ICE0), unlockState);		// reloads textures
			icon->setSkillType((Projectile::PROJECTILE_TYPE)(index + (int)Projectile::PROJECTILE_TYPE::ICE0));		// re-sets type of skill

			icon->setDescription();		// re-sets description

			++index;
		}
	}

	// YellowDino character
	else if (_playerSelection == SELECTION::YELLOW_DINO)
	{
		// Retrieves pattern for skill set
		auto skillPattern = UserDefault::getInstance()->getIntegerForKey("yellow_dino_skills_set", 31);		// default value = 0x11000

		// Retrieves skillset
		auto skillSet = dynamic_cast<ui::RadioButtonGroup*>(this->getChildByName("skill_set"));

		// For each child icon
		int index = 0;
		for (Node* node : skillSet->getChildren())
		{
			auto icon = dynamic_cast<IconSkill*>(node);

			// Checks unlock state
			bool unlockState = skillPattern & key;
			key = key >> 1;

			// Changes its properties accordingly
			char str[15];
			sprintf(str, "electric%d\0", index);

			icon->setName(str);							// name
			icon->getButton()->setName(str);

			icon->reloadTextures((Projectile::PROJECTILE_TYPE)(index + (int)Projectile::PROJECTILE_TYPE::ELECTRIC0), unlockState);		// reloads textures
			icon->setSkillType((Projectile::PROJECTILE_TYPE)(index + (int)Projectile::PROJECTILE_TYPE::ELECTRIC0));		// re-sets type of skill

			icon->setDescription();		// re-sets description

			++index;
		}
	}
}

void CharacterSelectionScene::displayDescription(IconSkill* skill)
{
	auto title = dynamic_cast<Label*>(this->getChildByName("title_text"));
	auto summary = dynamic_cast<Label*>(this->getChildByName("summary_text"));

	// Shows description panel
	this->getChildByName("background_text")->setVisible(true);
	title->setVisible(true);
	summary->setVisible(true);


	// Adds (or changes) description
	summary->setString(skill->getSummary());
	title->setString(skill->getTitle());


	// Sets color for title
	if (skill->getSkillType() >= Projectile::PROJECTILE_TYPE::FIRE0 && skill->getSkillType() <= Projectile::PROJECTILE_TYPE::FIRE4)
	{
		title->setTextColor(Color4B::RED);
	}
	else if (skill->getSkillType() >= Projectile::PROJECTILE_TYPE::ICE0 && skill->getSkillType() <= Projectile::PROJECTILE_TYPE::ICE4)
	{
		title->setTextColor(Color4B::BLUE);
	}
	else if (skill->getSkillType() >= Projectile::PROJECTILE_TYPE::ELECTRIC0 && skill->getSkillType() <= Projectile::PROJECTILE_TYPE::ELECTRIC4)
	{
		title->setTextColor(Color4B::ORANGE);
	}
}

void CharacterSelectionScene::hideDescription()
{
	this->getChildByName("background_text")->setVisible(false);
	this->getChildByName("title_text")->setVisible(false);
	this->getChildByName("summary_text")->setVisible(false);
}
