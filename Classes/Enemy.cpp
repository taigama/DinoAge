#include "Gold.h"
#include "Enemy.h"

#include "WorldScene.h"
#include "MyPoolManager.h"
#include "HUDLayer.h"
#include "EnergyBar.h"

#include "TriggerBlockZone.h"
#include "Projectile.h"

#define BASE_FORCE_KNOCK_BACK 150.0f

#define SCALE_X_LEFT +1
#define SCALE_X_RIGHT -1

#define DROP_LEGACY_ANIMATION_DURATION 0.7f

USING_NS_CC;


Enemy::~Enemy()
{
	
}

bool Enemy::init()
{
	// Super init
	if (!Character::init())
	{
		return false;
	}

	// Sets HP
	_hp = 1;
	_maxHP = 1;

	// Sets team -> number 2
	_team = 2;						// team 1 -> Player ; team 2 -> Enemy

	// Sets all kinds of speed
	_speed = 0.0f;
	_jumpSpeed = 0.0f;

	// Sets time for hurting and dying animations
	_hurtTime = 0.0f;
	_dieTime = 1.0f;

	// Sets (default) number of legacy
	_numLegacy = 0;

	_zone = nullptr;


	return true;
}

Rect Enemy::getBoundingBox() const
{
	return Rect(0, 0, getContentSize().width, getContentSize().height);
}

void Enemy::move(Character::DIRECTION direct)
{
	// direct == DIRECTION::NONE -> stops moving
	if (direct == Character::DIRECTION::NONE)
	{
		// Sets state
		_state = STATE::IDLE;

		_body->setVelocity(Vec2::ZERO);
	}

	// direct == DIRECTION::LEFT || direct == DIRECTION::RIGHT
	else
	{
		// If move LEFT but NOT currently facing LEFT
		if (direct == Character::DIRECTION::LEFT && getScaleX() != SCALE_X_LEFT)
		{
			// Changes direction
			_direction = Character::DIRECTION::LEFT;
			this->setScaleX(SCALE_X_LEFT);
		}

		// If move RIGHT but NOT currenly facing RIGHT
		else if (direct == Character::DIRECTION::RIGHT && getScaleX() != SCALE_X_RIGHT)
		{
			// Changes direction
			_direction = Character::DIRECTION::RIGHT;
			this->setScaleX(SCALE_X_RIGHT);
		}

		// Sets state
		_state = STATE::RUN;

		_body->setVelocity(Vec2((int)direct * _speed, _body->getVelocity().y));
	}
}

void Enemy::jump()
{
	// Sets state
	_state = STATE::JUMP;

	_body->setVelocity(Vec2(_body->getVelocity().x, _jumpSpeed));
}

void Enemy::attackPhysical()
{

}

void Enemy::attackProjectile()
{

}

void Enemy::knock(Character::DIRECTION direction, float forceFactor)
{
	AUDIO::play2d("bump.ogg");

	// If object dies -> nothing happens
	// If NOT
	if (_hp > 0)
	{
		auto scaleFactor = Director::getInstance()->getContentScaleFactor();
		float vecX, vecY;

		// Horizontally
		if (direction != DIRECTION::BOTTOM)
		{
			vecX = forceFactor * (-(int)direction) * BASE_FORCE_KNOCK_BACK / scaleFactor;

		}
		else
		{
			vecX = 0;
		}
		vecY = forceFactor * BASE_FORCE_KNOCK_BACK / scaleFactor;

		_body->setVelocity(Vec2(vecX, vecY));
	}
}

void Enemy::onHit(float dmg, DIRECTION dir, float force)
{
	// Calls super version
	Character::onHit(dmg, dir, force);

	// Sets state
	_state = STATE::HURTED;
	scheduleOnce([&](float delta) {

		_state = STATE::IDLE;	// returns back to IDLE state

	}, _hurtTime, "enemy_hit");

	// Animation for hurting -> executed by inherited classes
}

void Enemy::onDie()
{
	// ------------------------ ENEMY DIES -----------------------------

	// Knock up animation when dies and let Enemy fall through the Ground
	// [Normal dead for Enemy in general]
	auto scaleFactor = Director::getInstance()->getContentScaleFactor();

	auto vecX = 0;
	auto vecY = 2.0f * BASE_FORCE_KNOCK_BACK / scaleFactor;

	_body->setVelocity(Vec2(vecX, vecY));
	
	_body->setCollisionBitmask(0);
	_body->setContactTestBitmask(0);


	// After "dying time" -> sets this object into in-active state
	scheduleOnce([&](float delta) {

		// Notifies the parent Zone that the current Enemy dies
		if (getZone())
		{
			dynamic_cast<TriggerBlockZone*>(this->getZone())->notifyCreepDie(this);
		}

		this->setActive(false);

	}, _dieTime, "enemy_dies");

	// ----------------------------------------------------------------



	// ------------------------- DROPPED ITEMS ------------------------

	// LEGACY
	dropLegacy();

	// ENERGY ORB (with rate)
	if (RandomHelper::random_int(1, 1))
	{
		// Increase energy bar
		auto hudLayer = dynamic_cast<HUDLayer*>(Director::getInstance()->getRunningScene()->getChildByName("play_scene")->getChildByName("hud_layer"));
		auto bar = dynamic_cast<EnergyBar*>(hudLayer->getChildByName("energy_bar"));

		//bar->increaseEnergy(Vec2(this->getBoundingBox().getMidX(), this->getBoundingBox().getMidY()));
		bar->increaseEnergy(this->getPosition());
	}


	// ----------------------------------------------------------------
}

void Enemy::turnAround()
{
	_direction = (Character::DIRECTION)(-1 * (int)_direction);
	setScaleX(-1.0f * getScaleX());
}

// NO COLLISIONS BETWEEN ENEMIES
bool Enemy::onContactBegin(const PhysicsContact &contact)
{
	// Retrieves PhysicsBody of target
	auto targetBody = contact.getShapeA()->getBody();

	if (_body == targetBody)		// if current body is of shape A
	{
		targetBody = contact.getShapeB()->getBody();
	}

	// IF target is Item -> NO COLLISION
	if (targetBody->getCategoryBitmask() == (int)GameObject::OBJECT_TYPE::ITEM)
	{
		return false;
	}

	// IF target is Projectile of the same team -> NO COLLISION
	if (targetBody->getCategoryBitmask() == (int)GameObject::OBJECT_TYPE::PROJECTILE)
	{
		auto projectile = dynamic_cast<Projectile*>(targetBody->getOwner());

		if (projectile->getTeam() == _team)
		{
			return false;
		}
	}

	// IF target is Enemy (of the same team) -> NO COLLISION
	if (targetBody->getCategoryBitmask() == (int)GameObject::OBJECT_TYPE::CHARACTER)
	{
		auto character = dynamic_cast<Character*>(targetBody->getOwner());

		if (character->getTeam() == _team)
		{
			return false;
		}
	}

	return true;
}

void Enemy::dropLegacy()
{
	auto scaleFactor = Director::getInstance()->getContentScaleFactor();

	// Lift up animation
	auto moveY = MoveBy::create(DROP_LEGACY_ANIMATION_DURATION * 0.5f, Vec3(0, 80 / scaleFactor, 0));
	auto moveYSequence = Sequence::create(EaseQuadraticActionOut::create(moveY->clone()), EaseQuadraticActionIn::create(moveY->reverse()), nullptr);


	// For each 
	char name_s[50]; std::string name;
	Vec2 destination;
	ActionInterval* moveX;
	Spawn* moveAction;
	Sprite* legacySprite;

	float offset = 30 / scaleFactor;
	int sign = +1;

	double randResult = RandomHelper::random_real(0.0, 1000.0);

	for (int i = 0; i < _numLegacy; ++i)
	{
		// Creates alternate sprite for Gold -> FOR animation
		legacySprite = Gold::makeSprite();

		// Determine the drop destination
		destination.x = this->getPosition().x + sign * (offset + (int)(i / 2) * legacySprite->getBoundingBox().size.width * 1.05);
		destination.y = this->getPosition().y + legacySprite->getBoundingBox().size.height / 2 + 5 / scaleFactor;
		sign *= -1;		// flips the "side"


		// Sets anchor point and (initial) position for the alternate sprite
		legacySprite->setAnchorPoint(Vec2(0.5, 0.5));
		legacySprite->setPosition(Vec2(this->getPosition().x, destination.y));

		// Name for sprite
		sprintf(name_s, "enemy_%d_%d_%lf", (int)_charType, i, randResult);
		name = name_s;
		legacySprite->setName(name);

		World::getCurrent()->addChild(legacySprite, World::WORLD_LAYER::OBJECT);		// adds to World



		// Animation to drop the coin in place (destination)
		moveX = MoveTo::create(DROP_LEGACY_ANIMATION_DURATION, Vec3(destination.x, destination.y, 0));
		moveAction = Spawn::create(moveX->clone(), moveYSequence->clone(), nullptr);

		// Runs action
		legacySprite->runAction(moveAction);


		// Schedules to remove the alternate sprite for Gold amd spawn a Gold object at the destination
		World::getCurrent()->scheduleOnce([destination, name, this](float delta) {

			World::getCurrent()->removeChildByName(name);

			// Spawns Item
			//SPAWN_ITEM(Item::ITEM_TYPE::GOLD, destination.x, destination.y, World::WORLD_LAYER::OBJECT);


			int random = RandomHelper::random_int(0, 100);

			if (random > 90)//10%
			{
				World::getCurrent()->addChild(MyPoolManager::getInstance()->makeItem(Item::ITEM_TYPE::MEAT, destination.x, destination.y), World::WORLD_LAYER::OBJECT);
			}
			else if (random > 60)//30%
			{
				World::getCurrent()->addChild(MyPoolManager::getInstance()->makeItem(Item::ITEM_TYPE::ENERGY_ORB, destination.x, destination.y), World::WORLD_LAYER::OBJECT);
			}
			else// 60 %
			{
				World::getCurrent()->addChild(MyPoolManager::getInstance()->makeItem(Item::ITEM_TYPE::GOLD, destination.x, destination.y), World::WORLD_LAYER::OBJECT);
			}

			

		}, DROP_LEGACY_ANIMATION_DURATION, name);
	}
}
