#include "Item.h"

#include "Meat.h"
#include "Gold.h"
#include "EnergyOrb.h"
#include "EnergyBar.h"
#include "HUDLayer.h"
#include "WorldScene.h"

#if (CC_TARGET_PLATFORM == CC_PLATFORM_LINUX)
		#include "ui/CocosGUI.h"
#else
		#include "ui\CocosGUI.h"
#endif

USING_NS_CC;

Item::~Item() {}

bool Item::init()
{
	if (!GameObject::init())
		return false;

	_type = GameObject::OBJECT_TYPE::ITEM;

	_value = 0;

	// Disables scheduleUpdate()
	this->unscheduleUpdate();
	
	return true;
}

// create item at position | must manually do: world->addChild
Item* Item::createItem(ITEM_TYPE type, float x, float y)
{
	Item* result = nullptr;
	switch (type)
	{
	case ITEM_TYPE::MEAT:
		result = Meat::create();
		break;
	case ITEM_TYPE::GOLD:
		result = Gold::create();
		break;
	case ITEM_TYPE::ENERGY_ORB:
		result = EnergyOrb::create();
		break;
	default:
		break;
	}

	if (result)
	{
		result->setPosition(Vec2(x, y));		// anchor point -> default at Vec2(0.5, 0.5)
	}

	return result;
}

void Item::setColliderBitMask()
{
	if (_body == nullptr)
		return;

	_body->setCategoryBitmask((int)OBJECT_TYPE::ITEM);
	_body->setCollisionBitmask(
		(int)OBJECT_TYPE::CHARACTER
		//| (int) OBJECT_TYPE::BLOCK
	);
	_body->setContactTestBitmask(
		(int)OBJECT_TYPE::CHARACTER
	);
}


bool Item::onContactBegin(const cocos2d::PhysicsContact &contact)
{
	PhysicsBody* target = contact.getShapeA()->getBody();
	//PhysicsBody* own = contact.getShapeB()->getBody();
	if (_body == target)	// target is of shape B
	{
		//own = target;
		target = contact.getShapeB()->getBody();
	}

	// If target is a Character
	{
		Character* character = dynamic_cast<Character*>(target->getOwner());
		if (character->getTeam() == 1)		// Player's team
		{
			onObtained(character);
		}
	}


	return true;
}

// Called when player obtains this item
void Item::onObtained(Character* target)
{
	switch (_itemType)
	{
	case ITEM_TYPE::MEAT:
		AUDIO::play2d("eat.ogg");
		target->increaseHealth(_value);
		updateHealthHUD();

		// Change Player's avartar on HUD layer based on the current health
		if (target->getHP() > PLAYER_HEALTH_THRESHOLD_HURT * target->getMaxHP() &&
			((target->getHP() - _value) <= PLAYER_HEALTH_THRESHOLD_HURT * target->getMaxHP()))	// first threshold
		{
			// Retrieve HUDLayer -> Player's avatar
			auto hudLayer = dynamic_cast<HUDLayer*>(Director::getInstance()->getRunningScene()->getChildByName("play_scene")->getChildByName("hud_layer"));

			//auto sprite = dynamic_cast<Sprite*>(hudLayer->getChildByName("player_avatar"));
			auto avatar = dynamic_cast<ui::Button*>(hudLayer->getChildByName("player_avatar"));

			// Based on Player's type -> set corresponding sprite
			char str[50];
			if (target->getCharType() == Character::CHARACTER_TYPE::RED_DINOSAUR)
			{
				sprintf(str, "tyro_status_normal.png");
			}
			else if (target->getCharType() == Character::CHARACTER_TYPE::WHITE_DINOSAUR)
			{
				sprintf(str, "trino_status_normal.png");
			}
			else if (target->getCharType() == Character::CHARACTER_TYPE::YELLOW_DINOSAUR)
			{
				sprintf(str, "ptero_status_normal.png");
			}
			//sprite->initWithSpriteFrameName(str);
			//sprite->setAnchorPoint(Vec2(0, 1));

			avatar->loadTextures(str, str, str, ui::Widget::TextureResType::PLIST);
			avatar->setAnchorPoint(Vec2(0, 1));
		}
		if (target->getHP() > PLAYER_HEALTH_THRESHOLD_SERIOUS_HURT * target->getMaxHP() &&
			((target->getHP() - _value) <= PLAYER_HEALTH_THRESHOLD_SERIOUS_HURT * target->getMaxHP()))	// second threshold (lower than the first)
		{
			// Retrieve HUDLayer -> Player's avatar
			auto hudLayer = dynamic_cast<HUDLayer*>(Director::getInstance()->getRunningScene()->getChildByName("play_scene")->getChildByName("hud_layer"));

			//auto sprite = dynamic_cast<Sprite*>(hudLayer->getChildByName("player_avatar"));
			auto avatar = dynamic_cast<ui::Button*>(hudLayer->getChildByName("player_avatar"));

			// Based on Player's type -> set corresponding sprite
			char str[50];
			if (target->getCharType() == Character::CHARACTER_TYPE::RED_DINOSAUR)
			{
				sprintf(str, "tyro_status_hurt.png");
			}
			else if (target->getCharType() == Character::CHARACTER_TYPE::WHITE_DINOSAUR)
			{
				sprintf(str, "trino_status_hurt.png");
			}
			else if (target->getCharType() == Character::CHARACTER_TYPE::YELLOW_DINOSAUR)
			{
				sprintf(str, "ptero_status_hurt.png");
			}
			//sprite->initWithSpriteFrameName(str);
			//sprite->setAnchorPoint(Vec2(0, 1));

			avatar->loadTextures(str, str, str, ui::Widget::TextureResType::PLIST);
			avatar->setAnchorPoint(Vec2(0, 1));
		}

		break;
	case ITEM_TYPE::GOLD:
		AUDIO::play2d("smb_coin.ogg");
		target->increaseLegacy(_value);
		updateLegacyHUD();

		break;
	case ITEM_TYPE::ENERGY_ORB:
		if (!(dynamic_cast<EnergyOrb*>(this))->getDisappearState())
		{
			auto hudLayer = dynamic_cast<HUDLayer*>(Director::getInstance()->getRunningScene()->getChildByName("play_scene")->getChildByName("hud_layer"));
			auto bar = dynamic_cast<EnergyBar*>(hudLayer->getChildByName("energy_bar"));

			// Increase energy bar
			bar->increaseEnergy(getPosition());

			// Some effects
			(dynamic_cast<EnergyOrb*>(this))->disappear();
			AUDIO::play2d("EatHeart.ogg");
		}

		break;
	default:
		log("Invalid item type!!!!");
		break;
	}


	// For pool manager management: set active state to false (for re-use) and hide the item
	if (_itemType != ITEM_TYPE::ENERGY_ORB)
	{
		setActive(false);
	}
	else
	{
		// If ENERGY_ORB -> calls setActive(false) in EnergyOrb method
	}
}

void Item::updateHealthHUD()
{
	// Retrieves HUD layer and World layer
	auto hudLayer = dynamic_cast<HUDLayer*>(Director::getInstance()->getRunningScene()->getChildByName("play_scene")->getChildByName("hud_layer"));
	auto worldLayer = dynamic_cast<World*>(Director::getInstance()->getRunningScene()->getChildByName("play_scene")->getChildByName("world_layer"));

	// Gets Player
	auto player = worldLayer->getPlayer();

	// Update HP on HUD layer
	char text[20];
	sprintf(text, "%03.0f/%03.0f", player->getHP(), player->getMaxHP());

	auto hpBar = dynamic_cast<ui::LoadingBar*>(hudLayer->getChildByName("hp_bar"));		// retrieves HP bar
	auto hpLabel = dynamic_cast<Label*>(hudLayer->getChildByName("hp_label"));			// retrieves HP label

	hpLabel->setString(text);														// update HP label
	hpBar->setPercent(player->getHP() * 100.0f / player->getMaxHP());				// update HP bar
}

void Item::updateLegacyHUD()
{
	// Retrieves HUD layer and World layer
	auto hudLayer = dynamic_cast<HUDLayer*>(Director::getInstance()->getRunningScene()->getChildByName("play_scene")->getChildByName("hud_layer"));
	auto worldLayer = dynamic_cast<World*>(Director::getInstance()->getRunningScene()->getChildByName("play_scene")->getChildByName("world_layer"));

	// Gets Player
	auto player = worldLayer->getPlayer();

	// Update Player's legacy (Gold)
	char text[20];
	sprintf(text, "x%06d", player->getLegacy());

	auto legacyLabel = dynamic_cast<Label*>(hudLayer->getChildByName("legacy_label"));	// retrieves legacy label

	legacyLabel->setString(text);		// update legacy label
}

// Pool manager calls this
void Item::reset(float params[10])
{
	// Objects is in use (in active)
	this->setActive(true);

	// Sets position
	this->setPosition(params[0], params[1]);
}
