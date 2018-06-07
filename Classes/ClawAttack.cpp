#include "ClawAttack.h"

#include "WorldScene.h"
#include "Enemy.h"

#define CLAW_ATTACK_SPEED 0.0f
#define CLAW_ATTACK_DAMAGE 10.0f
#define CLAW_ATTACK_FORCE 1.0f

#define CLAW_ATTACK_WIDTH 65.0f
#define CLAW_ATTACK_HEIGHT 30.0f

#define CLAW_EFFECT_SCALE_FACTOR_X 0.8f
#define CLAW_EFFECT_SCALE_FACTOR_Y 0.35f
#define CLAW_EFFECT_OPACITY 220.0f

USING_NS_CC;


ClawAttack::~ClawAttack()
{
	//World::getInstance()->removeChild(_sprite);
	auto world = World::getCurrent();
	if (world)
	{
		world->removeChild(_sprite);
	}
}

bool ClawAttack::init()
{
	// Super init
	if (!Projectile::init())
	{
		return false;
	}

	// Sets values for properties
	_speed = CLAW_ATTACK_SPEED;
	_damage = CLAW_ATTACK_DAMAGE;
	_force = CLAW_ATTACK_FORCE;

	_projectileType = PROJECTILE_TYPE::CLAW_ATTACK;
	_team = 2;		// enemy team

	return true;
}

void ClawAttack::initSprites()
{
	// Sets content size for ClawAttack
	this->setContentSize(Size(CLAW_ATTACK_WIDTH, CLAW_ATTACK_HEIGHT));

	// Sprite for effect
	_sprite = Sprite::create("scratch.png");
	
	_sprite->setName("claw_effect");
	_sprite->setScaleX(CLAW_EFFECT_SCALE_FACTOR_X);
	_sprite->setScaleY(CLAW_EFFECT_SCALE_FACTOR_Y);
	_sprite->setOpacity(CLAW_EFFECT_OPACITY);
	_sprite->setAnchorPoint(Vec2(0.5, 0));
	_sprite->setVisible(false);

	//_sprite->setNormalizedPosition(Vec2(0.4, -0.25));

	//auto world = World::getInstance();
	auto world = World::getCurrent();
	world->addChild(_sprite, World::WORLD_LAYER::EFFECT_FRONT);
}

void ClawAttack::initPhysics()
{
	_body = PhysicsBody::createBox(getContentSize(), PhysicsMaterial(0.1f, 0.0f, 0.5f));
	_body->setDynamic(false);

	// Assigns PhysicsBody
	this->setPhysicsBody(_body);

	// Sets all related bitmasks
	setColliderBitMask();
}

void ClawAttack::attack(Character* target, Character::DIRECTION direction)
{
	// The direction of attacking is based on the currently facing direction of EnemyWerewolf
	auto host = dynamic_cast<Enemy*>(this->getParent());
	Projectile::attack(target, (Character::DIRECTION)(-(int)host->getDirection()));
}

void ClawAttack::showEffects()
{
	// Position
	auto enemy = dynamic_cast<Enemy*>(this->getParent());
	auto pos = enemy->convertToWorldSpace(this->getPosition());

	_sprite->setPosition(pos + Vec2((int)enemy->getDirection() * this->getContentSize().width / 2, 0));

	if (enemy->getDirection() == Character::DIRECTION::LEFT)
	{
		_sprite->setScaleX(CLAW_EFFECT_SCALE_FACTOR_X);
	}
	else if (enemy->getDirection() == Character::DIRECTION::RIGHT)
	{
		_sprite->setScaleX(-CLAW_EFFECT_SCALE_FACTOR_X);
	}
	

	// Shows
	_sprite->setVisible(true);

	AUDIO::play2d("ST01_00_00002.ogg");
}

void ClawAttack::hideEffects()
{
	_sprite->setVisible(false);
}
