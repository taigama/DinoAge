#include "HUDLayer.h"

#include "WorldScene.h"
#include "MenuScene1.h"
#include "EnergyBar.h"
#include "Gold.h"
#include "PlayScene.h"
#include "ResourceManager.h"


#if (CC_TARGET_PLATFORM == CC_PLATFORM_LINUX)
		#include "ui/CocosGUI.h"
#else
		#include "ui\CocosGUI.h"
#endif

#define PLAYER_MAX_HEALTH_TEXT "100/100"
#define HEALTH_FONT_SIZE 15.0f
#define LEGACY_FONT_SIZE 18.0f
#define NOTIFICATION_FONT_SIZE 20.0f

#define SCALE_MOVEMENT_BUTTON 0.5f
#define SCALE_ACTION_BUTTON 2.00f
#define SCALE_PAUSE_BUTTON 1.1f
#define SCALE_RESUME_MENU_BUTTON 1.4f

#define TIME_TRANSITION_MAIN_MENU 0.5f

#define DELAY_TIME_ACTIVATE_ADVANCED_SKILL 1.0f
#define DELAY_TIME_ACTIVATE_ULTIMATE_SKILL 1.0f

#define SCALE_FACTOR_HP_BAR 2.0f
#define SCALE_FACTOR_BORDER_AVATAR 0.36f
#define SCALE_FACTOR_PLAYER_AVATAR 1.2f

#define PLAYER_STATUS_Z_ORDER 2
#define BUTTON_Z_ORDER 4
#define PAUSE_COMPONENT_Z_ORDER 6

USING_NS_CC;


HUDLayer::~HUDLayer()
{
	// Release pausing components
	CC_SAFE_RELEASE_NULL(_shadowLayer);
	CC_SAFE_RELEASE_NULL(_resumeButton);
	CC_SAFE_RELEASE_NULL(_menuButton);


	_player = nullptr;
	m_camera = nullptr;
}

Layer* HUDLayer::createLayer()
{
	auto layer = HUDLayer::create();

	layer->setName("hud_layer");

	return layer;
}

bool HUDLayer::init()
{
	if (!Layer::init())
	{
		return false;
	}

	// Note: cannot retrieve Player when initializes HUD -> has to do at runtime
	_player = nullptr;

	m_camera = nullptr;

	auto visibleSize = Director::getInstance()->getVisibleSize();
	auto visibleOrigin = Director::getInstance()->getVisibleOrigin();

	_triggerCallback = false;
	_playerMovingState = false;
	_advancedSkill01 = true;		// used to know whether the button is "held" long
	_advancedSkill02 = true;		// enough to activate the corresponding advanced skill
	_counterClick = 0;


	///////////////////////////
	// 1. Necessary variables
	float padding = visibleSize.width / 24;
	auto scaleFactor = Director::getInstance()->getContentScaleFactor();

	padding *= 1.0 / scaleFactor;


	///////////////////////////////////////////////
	// 2. Creates "virtual" button to move left

	// Loads image button, and set callback function
	_leftButton = ui::Button::create("leftButton.png", "leftButton_pressed.png");

	_leftButton->setAnchorPoint(Vec2::ZERO);
	_leftButton->setScale(SCALE_MOVEMENT_BUTTON);
	_leftButton->setPosition(Vec2(visibleOrigin.x + padding / 3.0f, visibleOrigin.y + padding / 3.0f));

	// Adds event listener to button
	_leftButton->addTouchEventListener([&](cocos2d::Ref*, cocos2d::ui::Widget::TouchEventType type) {
		this->onTouchMoveLeft(type);
	});

	// Adds to HUD layer
	this->addChild(_leftButton, BUTTON_Z_ORDER);


	//////////////////////////////////////////////
	// 3. Creates "virtual" button to move right

	// Loads image for button, and set callback function
	_rightButton = ui::Button::create("rightButton.png", "rightButton_pressed.png");

	_rightButton->setAnchorPoint(Vec2::ZERO);
	_rightButton->setScale(SCALE_MOVEMENT_BUTTON);
	_rightButton->setPosition(Vec2(_leftButton->getBoundingBox().getMaxX() + padding / 3.0f, _leftButton->getBoundingBox().getMinY()));

	// Adds event listener to button
	_rightButton->addTouchEventListener([=](cocos2d::Ref*, cocos2d::ui::Widget::TouchEventType type) {
		this->onTouchMoveRight(type);
	});

	// Adds to HUD layer
	this->addChild(_rightButton, BUTTON_Z_ORDER);


	///////////////////////////////////////////////
	// 4. Creates "virtual" button to jump

	// Loads image for button, and set callback function
	_jumpButton = ui::Button::create("jumpButton.png", "jumpButton_pressed.png");

	_jumpButton->setAnchorPoint(Vec2(1, 0));
	_jumpButton->setScale(SCALE_ACTION_BUTTON);
	_jumpButton->setPosition(Vec2(visibleOrigin.x + visibleSize.width - padding / 2.0f, visibleOrigin.y + padding / 4));

	// Adds event listener to button
	_jumpButton->addTouchEventListener([&](cocos2d::Ref*, cocos2d::ui::Widget::TouchEventType type) {
		this->onTouchJump(type);
	});

	// Adds to HUD layer
	this->addChild(_jumpButton, BUTTON_Z_ORDER);


	///////////////////////////////////////////
	// 5. Creates "virtual" button for physical attack

	// load image for button, and set callback function
	_physicalAttackButton = ui::Button::create("physicalAttack.png", "physicalAttack_pressed.png");

	_physicalAttackButton->setAnchorPoint(Vec2(1, 0.5f));
	_physicalAttackButton->setScale(SCALE_ACTION_BUTTON);
	_physicalAttackButton->setPosition(Vec2(_jumpButton->getBoundingBox().getMinX() - padding / 3.0f, _jumpButton->getBoundingBox().getMidY()));

	// Adds event listener to button
	_physicalAttackButton->addTouchEventListener([=](cocos2d::Ref*, cocos2d::ui::Widget::TouchEventType type) {
		this->onTouchPhysical(type);
	});

	// Adds to HUD layer
	this->addChild(_physicalAttackButton, BUTTON_Z_ORDER);


	///////////////////////////////////////////////////////
	// 6. Creates "virtual" button for projectile attack

	// load image for button, and set callback function
	_projectileAttackButton = ui::Button::create("projectileAttack.png", "projectileAttack_pressed.png");

	_projectileAttackButton->setAnchorPoint(Vec2(0.5f, 0));
	_projectileAttackButton->setScale(SCALE_ACTION_BUTTON);
	_projectileAttackButton->setPosition(Vec2(_jumpButton->getBoundingBox().getMidX(), _jumpButton->getBoundingBox().getMaxY()));

	// Adds event listener to button
	_projectileAttackButton->addTouchEventListener([=](cocos2d::Ref*, cocos2d::ui::Widget::TouchEventType type) {
		this->onTouchProjectile(type);
	});

	// Adds to HUD layer
	this->addChild(_projectileAttackButton, BUTTON_Z_ORDER);


	///////////////////////////////////////////////////////////////////////
	// 7. Enables EventListener for keyboard (for debugging with Windows)
	this->setKeyboardEnabled(true);


	/////////////////////////////////
	// 8. Displays Player's avatar

	// Player's avatar
	auto avatarPos = Vec2(visibleOrigin.x + padding / 3, visibleOrigin.y + visibleSize.height - padding / 3);
	//auto avatar = Sprite::createWithSpriteFrameName("tyro_status_normal.png");		// create Sprite with temporary texture for RED_DINOSAUR character
	auto avatar = ui::Button::create("tyro_status_normal.png", "tyro_status_normal.png", "tyro_status_normal.png", ui::Widget::TextureResType::PLIST);

	avatar->setAnchorPoint(Vec2(0, 1));
	avatar->setName("player_avatar");
	avatar->setPosition(avatarPos);
	avatar->setScale(SCALE_FACTOR_PLAYER_AVATAR);

	this->addChild(avatar, PLAYER_STATUS_Z_ORDER);


	// Border for avatar (when full energy)
	//auto borderAvatar = Sprite::create("border_avatar1.png");
	auto borderAvatar = Sprite::create("border_avatar3.png");
	
	borderAvatar->setName("border_avatar");
	borderAvatar->setScale(SCALE_FACTOR_BORDER_AVATAR);
	borderAvatar->setAnchorPoint(Vec2(0.5, 0.5));
	borderAvatar->setPosition(Vec2(avatar->getBoundingBox().getMidX(), avatar->getBoundingBox().getMidY()));
	borderAvatar->setVisible(false);

	this->addChild(borderAvatar, PLAYER_STATUS_Z_ORDER - 1);


	////////////////////////////////////////////////////////////////
	// 9. Displays Player's Health, Energy, and Legacy (Gold)

	// Label for HP bar
	auto hpLabelPos = Vec2(borderAvatar->getBoundingBox().getMaxX(), visibleOrigin.y + visibleSize.height - padding / 2);
	auto hpLabel = Label::createWithTTF(PLAYER_MAX_HEALTH_TEXT, "neuropol x rg.ttf", HEALTH_FONT_SIZE / scaleFactor);

	hpLabel->setName("hp_label");
	hpLabel->setAnchorPoint(Vec2(0, 1));
	hpLabel->setPosition(hpLabelPos);
	hpLabel->setTextColor(Color4B(0, 150, 0, 255));		// dark green

	this->addChild(hpLabel, PLAYER_STATUS_Z_ORDER);

	// HP bar
	auto hpBarPos = Vec2(borderAvatar->getBoundingBox().getMaxX(), hpLabel->getBoundingBox().getMinY() - padding / 6);
	auto hpBar = ui::LoadingBar::create("fullLoadingBar.png");
	hpBar->setDirection(ui::LoadingBar::Direction::LEFT);
	hpBar->setPercent(100);

	hpBar->setName("hp_bar");
	hpBar->setScaleX(SCALE_FACTOR_HP_BAR);
	hpBar->setAnchorPoint(Vec2(0, 1));
	hpBar->setPosition(hpBarPos);

	this->addChild(hpBar, PLAYER_STATUS_Z_ORDER);

	// Border for HP bar
	auto hpBorder = Sprite::create("borderLoadingBar.png");

	hpBorder->setName("hp_border");
	hpBorder->setScaleX(SCALE_FACTOR_HP_BAR);
	hpBorder->setAnchorPoint(Vec2(0, 1));
	hpBorder->setPosition(hpBarPos);

	this->addChild(hpBorder, PLAYER_STATUS_Z_ORDER - 1);


	// Energy bar
	auto energyBarPos = Vec2(hpBar->getBoundingBox().getMaxX(), hpLabel->getBoundingBox().getMidY());
	auto energyBar = EnergyBar::createEnergyBar();

	energyBar->setName("energy_bar");
	energyBar->setAnchorPoint(Vec2(1, 0.5));
	energyBar->setPosition(energyBarPos);

	this->addChild(energyBar, PLAYER_STATUS_Z_ORDER);

	// Based on the current energy level, determines the Player's avatar display
	if (energyBar->getCurrentOrbs() == energyBar->getMaxOrbs())
	{
		borderAvatar->setVisible(true);
	}


	// Gold label and icon -> specify number of gold earned

	auto accumulatedGold = UserDefault::getInstance()->getIntegerForKey("player_legacy", 0);
	char legacyStr[10];
	sprintf(legacyStr, "x%06d", accumulatedGold);

	auto legacyLabel = Label::createWithTTF(legacyStr, "neuropol x rg.ttf", LEGACY_FONT_SIZE / scaleFactor);
	auto legacyIcon = Gold::makeSprite();

	//auto legacyIconPos = Vec2(visibleOrigin.x + visibleSize.width / 2 + padding / 4, visibleOrigin.y + visibleSize.height - padding / 4);
	auto relativeScreenX = visibleOrigin.x + visibleSize.width / 2;
	auto relativeHPX = hpBar->getBoundingBox().getMaxX() + padding * 3;
	auto higherX = relativeScreenX > relativeHPX ? relativeScreenX : relativeHPX;

	auto legacyIconPos = Vec2(higherX, avatar->getBoundingBox().getMidY());

	legacyIcon->setAnchorPoint(Vec2(0, 0.5));
	legacyIcon->setPosition(legacyIconPos);

	//auto legacyLabelPos = Vec2(visibleOrigin.x + visibleSize.width / 2 + padding / 2, legacyIcon->getBoundingBox().getMidY());
	auto legacyLabelPos = Vec2(legacyIcon->getBoundingBox().getMaxX() + padding / 16, legacyIcon->getBoundingBox().getMidY());

	legacyLabel->setName("legacy_label");
	legacyLabel->setTextColor(Color4B(102, 102, 0, 255));
	legacyLabel->setAnchorPoint(Vec2(0, 0.5));
	legacyLabel->setPosition(legacyLabelPos);

	this->addChild(legacyLabel, PLAYER_STATUS_Z_ORDER);
	this->addChild(legacyIcon, PLAYER_STATUS_Z_ORDER);


	// Note: ProgressFromTo action


	// Pause button
	auto pausePos = Vec2(visibleOrigin.x + visibleSize.width - padding / 4, visibleOrigin.y + visibleSize.height - padding / 4);
	_pauseButton = ui::Button::create("pause.png", "pause.png", "");

	_pauseButton->setName("pause_button");
	_pauseButton->setScale(SCALE_PAUSE_BUTTON);
	_pauseButton->setAnchorPoint(Vec2(1, 1));
	_pauseButton->setPosition(pausePos);
	_pauseButton->addTouchEventListener(CC_CALLBACK_2(HUDLayer::onPauseCallback, this));

	this->addChild(_pauseButton, BUTTON_Z_ORDER);


	///////////////////////////////////////////////////////////////////////
	// 11. Updates the position of HUD layer based on the current Camera
	m_camera = nullptr;
	//this->scheduleUpdateWithPriority(99);		// udpates LAST
	this->scheduleUpdate();












	return true;
}

void HUDLayer::onTouchMoveLeft(cocos2d::ui::Widget::TouchEventType type)
{
	// Retrieves Player
	extractPlayer();

	// Checks type of touch -> carries out corresponding operation
	switch (type)
	{
	case ui::Widget::TouchEventType::BEGAN:
		_player->move(Character::DIRECTION::LEFT);
		setPlayerMovingState(true);
		break;
	case ui::Widget::TouchEventType::ENDED:
	case ui::Widget::TouchEventType::CANCELED:
		if (getPlayerMovingState() && (_player)->getMoveDirect() == (int)Character::DIRECTION::LEFT)
		{
			_player->move(Character::DIRECTION::RIGHT);		// to stop Player
			setPlayerMovingState(false);
		}
		break;
	case ui::Widget::TouchEventType::MOVED:

		// IF Player moves touch out of Button => STOPS
		if (!_leftButton->getBoundingBox().containsPoint(_leftButton->getTouchMovePosition()))
		{
			if (getPlayerMovingState() && (_player)->getMoveDirect() == (int)Character::DIRECTION::LEFT)
			{
				_player->move(Character::DIRECTION::RIGHT);		// to stop Player
				setPlayerMovingState(false);
			}

		}

		break;
	default:
		break;
	}
}

void HUDLayer::onTouchMoveRight(cocos2d::ui::Widget::TouchEventType type)
{
	// Retrieves Player
	extractPlayer();

	// Checks type of touch -> carries out corresponding operation
	switch (type)
	{
	case ui::Widget::TouchEventType::BEGAN:
		_player->move(Character::DIRECTION::RIGHT);
		setPlayerMovingState(true);
		break;
	case ui::Widget::TouchEventType::ENDED:
	case ui::Widget::TouchEventType::CANCELED:
		if (getPlayerMovingState() && (_player)->getMoveDirect() == (int)Character::DIRECTION::RIGHT)
		{
			_player->move(Character::DIRECTION::LEFT);		// to stop Player
			setPlayerMovingState(false);
		}
		break;
	case ui::Widget::TouchEventType::MOVED:

		// IF Player moves touch out of Button => STOPS
		if (!_rightButton->getBoundingBox().containsPoint(_rightButton->getTouchMovePosition()))
		{
			if (getPlayerMovingState() && (_player)->getMoveDirect() == (int)Character::DIRECTION::RIGHT)
			{
				_player->move(Character::DIRECTION::LEFT);		// to stop Player
				setPlayerMovingState(false);
			}

		}

		break;
	default:
		break;
	}

}

void HUDLayer::onTouchPhysical(cocos2d::ui::Widget::TouchEventType type)
{

	// Retrieves Player
	extractPlayer();

	// Checks type of touch -> carries out corresponding operation
	switch (type)
	{
	case ui::Widget::TouchEventType::BEGAN:

		// FOR ultimate skill
		if (isSkillUnlocked(4) && this->getCounterClick() == 2)
		{
			auto energyBar = dynamic_cast<EnergyBar*>(this->getChildByName("energy_bar"));

			// Executes ultimate skill
			if (energyBar->activateUltimateSkill())
			{
				CAST_SKILL(_player, 2);
			}
			else
			{
				this->showNotification("NOT ENOUGH ENERGY");
				_player->attackPhysical();
			}

			unschedule("reset_counter_ultimate_scheduler");
		}
		else
		{
			_player->attackPhysical();
		}
		resetCounterClick();		// reset counter (for ultimate) at the end of combo (regarless of failure or success)


									// FOR Advanced skill (only if had already been UNLOCKED)
		if (isSkillUnlocked(2))
		{
			setAdvancedSkill01(false);			// set activate_state to false initially
			scheduleOnce([&](float delta) {

				setAdvancedSkill01(true);		// button being held long enough -> set to true

				_player->startCharge();		// charging effect

			}, DELAY_TIME_ACTIVATE_ADVANCED_SKILL, "activate_advanced_skill01_scheduler");
		}

		break;
	case ui::Widget::TouchEventType::ENDED:
	case ui::Widget::TouchEventType::CANCELED:
		if (isSkillUnlocked(2) && getAdvancedSkill01())
		{
			auto energyBar = dynamic_cast<EnergyBar*>(this->getChildByName("energy_bar"));

			// Executes advanced skill 01
			if (_player->isReadys[2] && energyBar->activateAdvancedSkill())
			{
				CAST_SKILL(_player, 0);
			}
			else
			{
				// ON cooldown
				if (!_player->isReadys[2])
				{
					this->showNotification("ON COOLDOWN");
				}

				// NOT enough energy
				else
				{
					this->showNotification("NOT ENOUGH ENERGY");
				}

				// Stops charging effect
				_player->stopCharge();
			}

		}
		else								// button is released too soon
		{
			unschedule("activate_advanced_skill01_scheduler");

			// Resets flag
			setAdvancedSkill01(true);
		}
		break;
	default:
		break;
	}
}

void HUDLayer::onTouchProjectile(cocos2d::ui::Widget::TouchEventType type)
{
	// Retrieves Player
	extractPlayer();

	// Checks type of touch -> carries out corresponding operation
	switch (type)
	{
	case ui::Widget::TouchEventType::BEGAN:
		_player->attackProjectile();

		// FOR advanced skill 02 (only if it had already been unlocked)
		if (isSkillUnlocked(3))
		{
			setAdvancedSkill02(false);			// set activate_state to false initially
			scheduleOnce([&](float delta) {

				setAdvancedSkill02(true);		// button being held long enough -> set to true

				_player->startCharge();		// charing effects

			}, DELAY_TIME_ACTIVATE_ADVANCED_SKILL, "activate_advanced_skill02_scheduler");
		}

		break;
	case ui::Widget::TouchEventType::ENDED:
	case ui::Widget::TouchEventType::CANCELED:
		if (isSkillUnlocked(3) && getAdvancedSkill02())
		{
			auto energyBar = dynamic_cast<EnergyBar*>(this->getChildByName("energy_bar"));

			// Executes advanced skill 02
			if (_player->isReadys[3] && energyBar->activateAdvancedSkill())
			{
				CAST_SKILL(_player, 1);
			}
			else
			{
				// ON cooldown
				if (!_player->isReadys[3])
				{
					this->showNotification("ON COOLDOWN");
				}

				// NOT enough energy
				else
				{
					this->showNotification("NOT ENOUGH ENERGY");
				}

				// Stops charging effect
				_player->stopCharge();
			}
		}
		else								// button is released too soon
		{
			unschedule("activate_advanced_skill02_scheduler");

			// Resets flag
			setAdvancedSkill02(true);
		}
		break;
	default:
		break;
	}
}

void HUDLayer::onTouchJump(cocos2d::ui::Widget::TouchEventType type)
{
	// Retrieves Player
	extractPlayer();

	// Checks type of touch -> carries out corresponding operation
	switch (type)
	{
	case ui::Widget::TouchEventType::BEGAN:
		_player->jump();

		// Setup needed to activate ultimate skill (only if it had already been unlocked)
		if (isSkillUnlocked(4))
		{
			// Increases counter (=> help activate ultimate)
			increaseCounterClick();

			// Schedules to reset counter (for ultimate) if exceeds specified time
			if (getCounterClick() == 1)
			{
				scheduleOnce([&](float delta) {

					this->resetCounterClick();

				}, DELAY_TIME_ACTIVATE_ULTIMATE_SKILL, "reset_counter_ultimate_scheduler");
			}
		}

		break;
	default:
		break;
	}
}

void HUDLayer::update(float delta)
{
	//if (Director::getInstance()->getRunningScene() != this->getParent()->getParent()->getParent())
	//{
	//	// If PlayScene is not the currently running scene -> DO NOT update for EnemyApeDino
	//	return;
	//}

    auto visibleOrigin = Director::getInstance()->getVisibleOrigin();
	auto visibleSize = Director::getInstance()->getVisibleSize();

	// [If not already] Retrieves the main player -> stored in _player
	if (_player == nullptr)
	{
		extractPlayer();


		// Update Player's avatar based on the actual selected character
		char str[50];						// retrieve file name suitable for the Player's chosen character
		switch (_player->getCharType())
		{
		case Character::CHARACTER_TYPE::RED_DINOSAUR:
			sprintf(str, "tyro_status_normal.png");
			break;
		case Character::CHARACTER_TYPE::WHITE_DINOSAUR:
			sprintf(str, "trino_status_normal.png");
			break;
		case Character::CHARACTER_TYPE::YELLOW_DINOSAUR:
			sprintf(str, "ptero_status_normal.png");
			break;
		default:
			log("ERROR: INVALID CHARACTER TYPE");
			break;
		}

		//auto sprite = dynamic_cast<Sprite*>(this->getChildByName("player_avatar"));

		//sprite->initWithSpriteFrameName(str);
		//sprite->setAnchorPoint(Vec2(0, 1));

		auto avatar = dynamic_cast<ui::Button*>(this->getChildByName("player_avatar"));

		avatar->loadTextures(str, str, str, ui::Widget::TextureResType::PLIST);
		avatar->setAnchorPoint(Vec2(0, 1));
	}

	// Gets the current Camera
	if (m_camera == nullptr)
	{
		m_camera = Camera::getDefaultCamera();
	}

	// Calculate the "new" origin position for HUD layer in respect of the Camera's position
	auto myPos = m_camera->getPosition() - visibleOrigin;
	myPos.x -= visibleSize.width / 2;
	myPos.y -= visibleSize.height / 2;

	// Sets the position of HUD layer
	this->setPosition(myPos);
}

void HUDLayer::onKeyPressed(cocos2d::EventKeyboard::KeyCode keycode, cocos2d::Event* event)
{
	// [If not already] Retrieves the main player -> stored in _player
	extractPlayer();

	switch (keycode)
	{
	case cocos2d::EventKeyboard::KeyCode::KEY_LEFT_ARROW:
		_player->move(Character::DIRECTION::LEFT);
		break;
	case cocos2d::EventKeyboard::KeyCode::KEY_RIGHT_ARROW:
		_player->move(Character::DIRECTION::RIGHT);
		break;
	case cocos2d::EventKeyboard::KeyCode::KEY_UP_ARROW:
		_player->jump();
		break;

	case cocos2d::EventKeyboard::KeyCode::KEY_D:
		_player->attackPhysical();
		break;
	case cocos2d::EventKeyboard::KeyCode::KEY_F:
		_player->attackProjectile();
		break;

	case cocos2d::EventKeyboard::KeyCode::KEY_9:
		_player->startCharge(2);
		break;
	case cocos2d::EventKeyboard::KeyCode::KEY_0:
		_player->startCharge(3);
		break;
	case cocos2d::EventKeyboard::KeyCode::KEY_1:
		CAST_SKILL(_player, 0);
		break;
	case cocos2d::EventKeyboard::KeyCode::KEY_2:
		CAST_SKILL(_player, 1);
		break;
	case cocos2d::EventKeyboard::KeyCode::KEY_3:
		CAST_SKILL(_player, 2);
		break;

	case cocos2d::EventKeyboard::KeyCode::KEY_ESCAPE:
		World::getCurrent()->exitGame(nullptr);
		break;
	default:
		break;
	}
}

void HUDLayer::onKeyReleased(cocos2d::EventKeyboard::KeyCode keycode, cocos2d::Event* event)
{
	// [If not already] Retrieves the main player -> stored in _player
	extractPlayer();

	switch (keycode)
	{
	case cocos2d::EventKeyboard::KeyCode::KEY_LEFT_ARROW:
		_player->move(Character::DIRECTION::RIGHT);
		break;
	case cocos2d::EventKeyboard::KeyCode::KEY_RIGHT_ARROW:
		_player->move(Character::DIRECTION::LEFT);
		break;
	default:
		break;
	}
}

void HUDLayer::onPauseCallback(cocos2d::Ref*, cocos2d::ui::Widget::TouchEventType type)
{
	// If a callback is being triggered, or user does not release
	// touch ON the button -> skip this function
	if (_triggerCallback || type != ui::Widget::TouchEventType::BEGAN)
	{
		return;
	}
	_triggerCallback = true;

	// Add a layer on top -> "hide/shadow" the scene
	if (_shadowLayer == nullptr)
	{
		_shadowLayer = LayerColor::create(Color4B::BLACK);

		_shadowLayer->setContentSize(Director::getInstance()->getVisibleSize() * 1.5);
		_shadowLayer->setAnchorPoint(Vec2::ZERO);
		_shadowLayer->setPosition(Vec2::ZERO);
		_shadowLayer->setName("shadow_layer");
		_shadowLayer->setOpacity(170);

		CC_SAFE_RETAIN(_shadowLayer);

	}
	this->addChild(_shadowLayer, PAUSE_COMPONENT_Z_ORDER);		// NEEDS TO BE ON TOP


	// Show two buttons: Resume game or Return to main menu
	auto visibleSize = Director::getInstance()->getVisibleSize();
	auto visibleOrigin = Director::getInstance()->getVisibleOrigin();
	auto padding = visibleSize.width / 20;

	// Resume button
	if (_resumeButton == nullptr)
	{
		_resumeButton = ui::Button::create("resume.png", "resume.png", "");

		_resumeButton->setName("resume_button");
		_resumeButton->setScale(SCALE_RESUME_MENU_BUTTON);
		_resumeButton->setAnchorPoint(Vec2(1, 0.5));
		_resumeButton->setPosition(Vec2(visibleOrigin.x + visibleSize.width / 2 - padding, visibleOrigin.y + visibleSize.height / 2));
		_resumeButton->addTouchEventListener(CC_CALLBACK_2(HUDLayer::onResumeCallback, this));

		CC_SAFE_RETAIN(_resumeButton);
	}
	this->addChild(_resumeButton, PAUSE_COMPONENT_Z_ORDER + 1);	// on top of shadow layer

	// (Return to) Main menu button
	if (_menuButton == nullptr)
	{
		_menuButton = ui::Button::create("menu.png", "menu.png", "");

		_menuButton->setName("menu_button");
		_menuButton->setScale(SCALE_RESUME_MENU_BUTTON);
		_menuButton->setAnchorPoint(Vec2(0, 0.5));
		_menuButton->setPosition(Vec2(visibleOrigin.x + visibleSize.width / 2 + padding, visibleOrigin.y + visibleSize.height / 2));
		_menuButton->addTouchEventListener(CC_CALLBACK_2(HUDLayer::onMenuCallback, this));

		CC_SAFE_RETAIN(_menuButton);
	}
	this->addChild(_menuButton, PAUSE_COMPONENT_Z_ORDER + 1);		// on top of shadow layer


	// Pause the game
	//Director::getInstance()->pause();		// pause the running scene
	PlayScene::pauseRecursive(Director::getInstance()->getRunningScene()->getChildByName("play_scene")->getChildByName("world_layer"));
	Director::getInstance()->getRunningScene()->getPhysicsWorld()->setSpeed(0.0f);	// pause the PhysicsWorld


	// Pauses all EventListener(s) for button(s)
	pauseAllEventListener();

	// Turns trigger flag back
	setTriggerCallbackState(false);
}

void HUDLayer::onResumeCallback(cocos2d::Ref*, cocos2d::ui::Widget::TouchEventType type)
{
	// If a callback is being triggered, or user does not release
	// touch ON the button -> skip this function
	if (_triggerCallback || type != ui::Widget::TouchEventType::BEGAN)
	{
		return;
	}
	_triggerCallback = true;

	// Removes shadow layer and buttons (resume and return-to-main-menu ones)
	this->removeChildByName("shadow_layer", false);
	this->removeChildByName("resume_button", false);
	this->removeChildByName("menu_button", false);

	// Unpauses the game and PhysicsWorld
	//Director::getInstance()->resume();
	PlayScene::resumeRecursive(Director::getInstance()->getRunningScene()->getChildByName("play_scene")->getChildByName("world_layer"));
	Director::getInstance()->getRunningScene()->getPhysicsWorld()->setSpeed(1.0f);

	// Resumes all EventListener(s) for button(s)
	resumeAllEventListener();

	// Turns trigger flag back
	setTriggerCallbackState(false);
}

void HUDLayer::onMenuCallback(cocos2d::Ref*, cocos2d::ui::Widget::TouchEventType type)
{
	// If a callback is being triggered, or user does not release
	// touch ON the button -> skip this function
	if (_triggerCallback || type != ui::Widget::TouchEventType::BEGAN)
	{
		return;
	}
	_triggerCallback = true;

	// Unpauses the game and PhysicsWorld
	//Director::getInstance()->resume();
	PlayScene::resumeRecursive(Director::getInstance()->getRunningScene()->getChildByName("play_scene")->getChildByName("world_layer"));
	Director::getInstance()->getRunningScene()->getPhysicsWorld()->setSpeed(1.0f);

	// Turns trigger flag back
	setTriggerCallbackState(false);

	AUDIO::setVolume(ResourceManager::getInstance()->backgroundSongID, 0.5f);


	// Brings user back to Main Menu scene
	Director::getInstance()->replaceScene(TransitionFade::create(TIME_TRANSITION_MAIN_MENU, MenuScene1::createScene()));
}

void HUDLayer::showNotification(std::string message)
{
	// Checks for empty message
	if (message.empty())
	{
		return;
	}


	// IF this message has been notified before -> WAIT until finish
	if (World::getCurrent()->getChildByName(message) != nullptr)
	{
		return;
	}


	// Creates a Label node holding the message
	auto fontSize = NOTIFICATION_FONT_SIZE / Director::getInstance()->getContentScaleFactor();	// scaled font size
	auto label = Label::createWithTTF(message, "ArcadeClassic.ttf", fontSize);

	label->setName(message);
	label->setTextColor(Color4B::RED);
	label->setAnchorPoint(Vec2(0.5, 0));
	label->setPosition(_player->getPosition() + Vec2(0, _player->getBoundingBox().size.height / 2));

	auto world = World::getCurrent();
	world->addChild(label, World::WORLD_LAYER::EFFECT_BACK);


	// Actions for notification
	auto moveUp = MoveBy::create(1.5f, Vec3(0, label->getContentSize().height * 2, 0));
	auto fadeOut = FadeOut::create(1.5f);

	auto spawn = Spawn::createWithTwoActions(moveUp, fadeOut);

	label->runAction(spawn->clone());


	// Schedules to remove notification
	World::getCurrent()->scheduleOnce([message, this](float delta) {

		World::getCurrent()->removeChildByName(message);

	}, spawn->getDuration(), "remove_noti_scheduler");
}

void HUDLayer::pauseAllEventListener()
{
	// PAUSE Button
	_eventDispatcher->pauseEventListenersForTarget(_pauseButton, false);

	// MOVEMENT Buttons
	_rightButton->onTouchCancelled(nullptr, nullptr);
	_eventDispatcher->pauseEventListenersForTarget(_leftButton, false);
	_rightButton->onTouchCancelled(nullptr, nullptr);
	_eventDispatcher->pauseEventListenersForTarget(_rightButton, false);
	_eventDispatcher->pauseEventListenersForTarget(_jumpButton, false);

	// ATTACK Buttons
	_eventDispatcher->pauseEventListenersForTarget(_physicalAttackButton, false);
	_eventDispatcher->pauseEventListenersForTarget(_projectileAttackButton, false);
}

void HUDLayer::resumeAllEventListener()
{
	// PAUSE Button
	_eventDispatcher->resumeEventListenersForTarget(_pauseButton, false);

	// MOVEMENT Buttons
	_eventDispatcher->resumeEventListenersForTarget(_leftButton, false);
	_eventDispatcher->resumeEventListenersForTarget(_rightButton, false);
	_eventDispatcher->resumeEventListenersForTarget(_jumpButton, false);

	// ATTACK Buttons
	_eventDispatcher->resumeEventListenersForTarget(_physicalAttackButton, false);
	_eventDispatcher->resumeEventListenersForTarget(_projectileAttackButton, false);
}

void HUDLayer::hideHUDLayer()
{
	// Turns off all EventListeners
	pauseAllEventListener();


	// Sets visibility of all children of HUDLayer to FALSE
	for (auto child : this->getChildren())
	{
		child->setVisible(false);
	}
}

void HUDLayer::unhideHUDLayer()
{
	// Turns on all EventListeners
	resumeAllEventListener();


	// Sets visibility of all children of HUDLayer to TRUE (except avatar's border)
	for (auto child : this->getChildren())
	{
		if (child->getName() == "border_avatar")
			continue;

		child->setVisible(true);
	}
}

void HUDLayer::extractPlayer()
{
	// Extracts Player
	if (_player == nullptr)
	{
		auto playScene = Director::getInstance()->getRunningScene()->getChildByName("play_scene");
		auto worldLayer = dynamic_cast<World*>(playScene->getChildByName("world_layer"));
		_player = worldLayer->getPlayer();
	}


	//// Extracts Player's skills-unlock pattern
	//if (_player->getCharType() == Character::CHARACTER_TYPE::RED_DINOSAUR)
	//{
	//	_unlockSkillPattern = UserDefault::getInstance()->getIntegerForKey("red_dino_skills_set");
	//}
	//else if (_player->getCharType() == Character::CHARACTER_TYPE::WHITE_DINOSAUR)
	//{
	//	_unlockSkillPattern = UserDefault::getInstance()->getIntegerForKey("white_dino_skills_set");
	//}
	//else if (_player->getCharType() == Character::CHARACTER_TYPE::YELLOW_DINOSAUR)
	//{
	//	_unlockSkillPattern = UserDefault::getInstance()->getIntegerForKey("yellow_dino_skills_set");
	//}
}

bool HUDLayer::isSkillUnlocked(int index)
{
	return true; // unlock all
	//// Creates the accurate key
	//int key = 16;			// 0x10000
	//while (index-- > 0)
	//{
	//	key = key >> 1;
	//}

	//// Extracts the state of the skill
	//return (_unlockSkillPattern & key);
}
