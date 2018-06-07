#include "Character.h"

#include "RedDino.h"
#include "WhiteDino.h"
#include "YellowDino.h"

#include "EnemyApeDino.h"
#include "EnemyWerewolf.h"
#include "EnemyRamus.h"
#include "EnemyFeeder.h"

#include "DarkPortal.h"

#include "WorldScene.h"
#include <time.h>

#define TAKEN_DAMAGE_FONT_SIZE 15.0f
#define TAKEN_DAMAGE_ANIMATION_TIME 1.5f

USING_NS_CC;

Character::~Character() {}

bool Character::init()
{
	// call base function
	if (!GameObject::init())
		return false;

	// set variables
	_type = OBJECT_TYPE::CHARACTER;

	_hp = 0;
	_team = 0;

	_state = STATE::IDLE;
	_direction = DIRECTION::NONE;
	_charType = CHARACTER_TYPE::NONE;
	
	_legacy = 0;


	_debugLabel = Label::createWithTTF("", "fonts/Marker Felt.ttf", 15);
	_debugLabel->setPosition(Vec2(0, 50));
	this->addChild(_debugLabel, 5, 123123);

	_zoneContainer = nullptr;

	return true;
}


/////////////////////////////////////////
// rubbishes
///----------------

/**
 * Characters (have status) should call Character::update(delta) first!
 * Many characters don't need status? ok shouldn't call base func
 */
void Character::update(float delta)
{
	//// update all status of this char
	//int n = statuses.size();
	//if (n > 0)
	//{
	//	for (int i = 0; i < n; i++)
	//	{
	//		statuses.at(i)->update(delta);
	//	}
	//}

	//// clean status
	//n = need_del_statuses.size();
	//if (n > 0)
	//{
	//	// need_del_statuses sorted!
	//	for (int i = 0; i < n; i++)
	//	{
	//		statuses.eraseObject(need_del_statuses.at(i), true);
	//		delete need_del_statuses.at(i);
	//	}
	//	need_del_statuses.clear();
	//}
}

/**
 * Create character
 * type enum CHARACTER_TYPE
 * team that the character belong to. p/s: main player in team #1
 */
Character* Character::createCharacter(CHARACTER_TYPE type, int team)
{
	Character* result = nullptr;
	switch (type)
	{
	// PLAYER
	case CHARACTER_TYPE::RED_DINOSAUR:
		result = RedDino::create();	
		break;
	case CHARACTER_TYPE::WHITE_DINOSAUR:
		result = WhiteDino::create();
		break;
	case CHARACTER_TYPE::YELLOW_DINOSAUR:
		result = YellowDino::create();
	    break;

	// ENEMY
	case CHARACTER_TYPE::ENEMY_APE_DINO:
		result = EnemyApeDino::create();
		break;
	case CHARACTER_TYPE::ENEMY_WEREWOLF:
		result = EnemyWerewolf::create();
		break;
	case CHARACTER_TYPE::ENEMY_RAMUS:
		result = EnemyRamus::create();
		break;
	case CHARACTER_TYPE::ENEMY_FEEDER:
		result = EnemyFeeder::create();
		break;

	case CHARACTER_TYPE::DARK_PORTAL:
		result = DarkPortal::create();
		break;

	default:
		log("Invalid character type");
		break;
	}

	if (result)
	{
		result->_team = team;
	}

	return result;
}

// Projectile calls this
void Character::onHit(float dmg, DIRECTION dir, float force)
{
	// IF Character already dies -> skip
	if (_hp <= 0) return;

	// Inflicts damage into current health of Character
	auto previousHP = _hp;
	_hp -= dmg;
	if (_hp <= 0)
	{
		_hp = 0;

		onDie();
	}

	// Calculates lost HP
	auto lostHP = previousHP - _hp;


	// Displays labels notifying the taken damage
	char str[10] , name[100];
	std::string name_s;


	sprintf(str, "-%.0f", lostHP);
	sprintf(name, "label_damage_%d_%d", (int)_charType, RandomHelper::random_int(0, 1000));

	auto scaleFactor = Director::getInstance()->getContentScaleFactor();

	auto labelDamage = Label::createWithTTF(str, "6809 chargen.ttf", TAKEN_DAMAGE_FONT_SIZE / scaleFactor);

	labelDamage->setName(name);
	labelDamage->setAnchorPoint(Vec2(0.5, 0));
	labelDamage->setPosition(getPosition() + Vec2(0, getBoundingBox().size.height));
	labelDamage->setTextColor(Color4B::RED);

	//World::getInstance()->addChild(labelDamage, World::WORLD_LAYER::EFFECT_BACK);		// adds to World
	World::getCurrent()->addChild(labelDamage, World::WORLD_LAYER::EFFECT_BACK);

	// Actions -> animation
	auto moveUp = MoveBy::create(TAKEN_DAMAGE_ANIMATION_TIME, Vec3(0, labelDamage->getBoundingBox().size.height * 2.5f, 0));
	auto fadeOut = FadeOut::create(TAKEN_DAMAGE_ANIMATION_TIME * 1.5f);
	auto scaleBy = ScaleBy::create(TAKEN_DAMAGE_ANIMATION_TIME, 2.5f);

	auto spawn = Spawn::create(moveUp, fadeOut, scaleBy, nullptr);

	labelDamage->runAction(spawn->clone());		// executes action

	name_s = name;

	// After animation time -> removes label
	World::getCurrent()->scheduleOnce([name_s](float delta) {

		//this->removeChildByName("label_damage");
		//World::getInstance()->removeChildByName(name);
		World::getCurrent()->removeChildByName(name_s);

	}, TAKEN_DAMAGE_ANIMATION_TIME * 1.5f, name);
}

// Programmer calls this -> carried out physical attack
void Character::attackPhysical()
{
	//_ throw projectile
	// physical attacks??
}

void Character::attackProjectile() {}

// Programmer calls this -> move character in the specified direction
void Character::move(DIRECTION direct) {}

void Character::jump() {};

// Inherited classes call this in initPhysics()
void Character::setColliderBitMask()
{
	if (_body == nullptr)
		return;

	_body->setCategoryBitmask((int)OBJECT_TYPE::CHARACTER);	// character

	_body->setCollisionBitmask(
		(int)OBJECT_TYPE::CHARACTER
		| (int)OBJECT_TYPE::BLOCK
	);
	
	_body->setContactTestBitmask(
		(int)OBJECT_TYPE::CHARACTER	
		| (int)OBJECT_TYPE::PROJECTILE
		| (int)OBJECT_TYPE::BLOCK
		| (int)OBJECT_TYPE::ITEM
		| (int)OBJECT_TYPE::ZONE
	);
}

// auto callback
bool Character::onContactBegin(const cocos2d::PhysicsContact &contact)
{

	return true;
}

// auto callback
bool Character::onContactPresolve(const cocos2d::PhysicsContact &contact)
{
	return true;
}

bool Character::onContactSeparate(const cocos2d::PhysicsContact &contact)
{
	return true;
}


// onHit call this
void Character::onDie()
{
	// ----- test ----------
	//char str[100] = { 0 };
	//sprintf(str, "died");
	//
	//auto label = Label::createWithTTF(str, "fonts/Marker Felt.ttf", 15);
	//label->setPosition(this->getPosition());
	//getParent()->addChild(label, 5, 123123);


	this->setActive(false);
	// run dead animation
	// call destroy function, or make object in-active (pool manager management)
}

void Character::increaseHealth(float value)
{
	_hp += value;
	if (_hp > _maxHP)
		_hp = _maxHP;
}

void Character::increaseLegacy(float value)
{
	_legacy += value;
}

/////////////////////////////////////
// rubbishes
//----------------------

//
//
//// them trang thai vao
//void Character::addStatus(Status* status)
//{
//	statuses.pushBack(status);
//}
//
//// xoa trang thai
//void Character::delStatus(Status* status)
//{
//	need_del_statuses.pushBack(status);
//	// why not just delete status?
//	// because he still has "function call stack"
//}
