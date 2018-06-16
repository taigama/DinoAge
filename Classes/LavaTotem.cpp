#include "LavaTotem.h"



#include "MyPoolManager.h"
//#include "Effect.h"
#include "WorldScene.h"
#include "Block.h"


USING_NS_CC;



#define TOTEM_DAMAGE 40.0f
#define TOTEM_LIFE_TIME 0.1f

#define SIZE_WIDTH 50.0f
#define SIZE_HEIGHT 90.0f

LavaTotem::~LavaTotem() {}

bool LavaTotem::init()
{
	if (!Projectile::init())
		return false;
	// sprite init & physic init were called in base

	_projectileType = PROJECTILE_TYPE::LAVA_TOTEM;

	_damage = TOTEM_DAMAGE;


	
	return true;
}

void LavaTotem::update(float delta)
{
	if (_timer <= 0)
	{
		this->setActive(false);
	}
	// made this object has at least 1 update
	_timer -= delta;
}

void LavaTotem::initPhysics()
{
	float scale_factor = Director::getInstance()->getContentScaleFactor();

	Size s = Size(SIZE_WIDTH / scale_factor, SIZE_HEIGHT / scale_factor);

	_body = PhysicsBody::createBox(s, PhysicsMaterial(0.1F, 0.0f, 0.0f));
	_body->setDynamic(true);
	_body->setGravityEnable(false);
	this->setPhysicsBody(_body);

	// call Projectile's func
	this->setColliderBitMask();
}



/* Reset this object
parameter: x, y, direct, team
// method called by pool*/
void LavaTotem::reset(float params[10])
{
	this->setActive(true);
	scheduleUpdate();
	_timer = TOTEM_LIFE_TIME;

	this->setPosition(Vec2(params[0], params[1]));

	float scale_factor = Director::getInstance()->getContentScaleFactor();

	SPAWN_EFFECT(Effect::EFFECT_TYPE::FIRE_ROCK
		, params[0] - params[2] * SIZE_WIDTH * 0.1f / scale_factor
		, this->getPositionY() + SIZE_HEIGHT * 0.07f / scale_factor
		, - params[2]
		, 1
		, 1
		, World::WORLD_LAYER::EFFECT_FRONT);

	_team = params[3];
}

// auto callback
bool LavaTotem::onContactBegin(const cocos2d::PhysicsContact &contact)
{
	// Retrieves PhysicsBody of target
	auto targetBody = contact.getShapeA()->getBody();

	if (_body == targetBody)		// if current body is of shape A
	{
		targetBody = contact.getShapeB()->getBody();
	}

		// Checks whether the target is Player
	if (targetBody->getCategoryBitmask() == (int)GameObject::OBJECT_TYPE::CHARACTER)
	{
		Character* target = (Character*)targetBody->getOwner();

		// Target is of opposite team -> inflicts damage
		if (target->getTeam() != _team)
		{
			attack(target, Character::DIRECTION::BOTTOM);
			return true;
		}
	}

	return false;
}