#include "ThunderBody.h"

#include "MyPoolManager.h"
#include "WorldScene.h"

//#include "Effect.h"// test
//#include "WorldScene.h"
//#define CHECK_REF_COUNT_VALID(x) ((x > 0) && (x < 1000000))// prevent overflow

USING_NS_CC;

#define ATTACK_TIME 0.1f
#define DAMAGE 5
#define SIZE_WIDTH 280

#define LIGHTNING_HEIGHT 178.0f


bool ThunderBody::init()
{
	if (!Projectile::init())
		return false;
	// sprite init & physic init were called in base

	_projectileType = PROJECTILE_TYPE::THUNDER_BODY;

	_damage = DAMAGE;

	return true;
}

void ThunderBody::update(float delta)
{
	if (_timer <= 0)
	{
		this->setActive(false);
	}
	_timer -= delta;
}

void ThunderBody::initPhysics()
{
	float scale_factor = Director::getInstance()->getContentScaleFactor();
	Size s = Size(SIZE_WIDTH / scale_factor, SIZE_WIDTH / scale_factor);

	_body = PhysicsBody::createBox(s, PhysicsMaterial(0.1F, 0.0f, 0.0f));
	_body->setDynamic(true);
	_body->setGravityEnable(false);
	this->setPhysicsBody(_body);

	// call Projectile's func
	this->setColliderBitMask();

	m_heightLightning = LIGHTNING_HEIGHT / scale_factor;
}



/* Reset this object
parameter: x, y, direct, team
// method called by pool*/
void ThunderBody::reset(float params[10])
{
	this->setActive(true);
	_timer = ATTACK_TIME;

	this->setPosition(/* _host->convertToNodeSpace(Vec2(*/params[0], params[1]/*))*/);

	_team = params[3];
}



void ThunderBody::attack(Character* target, Character::DIRECTION dir)
{
	// make enemy hurted
	Projectile::attack(target, dir);

	Vec2 tarPos = target->getPosition();
	tarPos.y += target->getSprite()->getBoundingBox().getMidY();
	drawLightning(tarPos);
}

//
void ThunderBody::drawLightning(Vec2 pointTarget)
{
	Vec2 tmpPos = (_position + pointTarget) / 2.0f;
	float scale = pointTarget.distance(_position) / m_heightLightning;

	float radian = M_PI_2 - (pointTarget - _position).getAngle();

	GameObject* m_lightning = MAKE_EFFECT(Effect::EFFECT_TYPE::BOLT_BLUE
		, tmpPos.x
		, tmpPos.y
		, 1
		, 1
		, scale);
	m_lightning->setRotation(CC_RADIANS_TO_DEGREES(radian));
	World::getCurrent()->addChild(m_lightning);
}