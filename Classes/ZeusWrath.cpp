#include "ZeusWrath.h"
#include "WorldScene.h"

//#include "Effect.h"// test
//#include "WorldScene.h"
#include "MyPoolManager.h"

USING_NS_CC;

#define ATTACK_TIME 0.1f
#define DAMAGE 50.0f

#define SIZE_WIDTH 854.0f
#define SIZE_HEIGHT 480.0f


#define LIGHTNING_HEIGHT 178.0f

bool ZeusWrath::init()
{
	if (!Projectile::init())
		return false;
	// sprite init & physic init were called in base

	_projectileType = PROJECTILE_TYPE::ZEUS_WRATH;

	_damage = DAMAGE;
	m_pointStart = Vec2::ZERO;

	m_lightning = nullptr;
	m_heightLightning = LIGHTNING_HEIGHT / Director::getInstance()->getContentScaleFactor();

	return true;
}

void ZeusWrath::update(float delta)
{	
	if (_timer <= 0)
	{
		this->setActive(false);
	}
	_timer -= delta;
}


void ZeusWrath::initPhysics()
{
	float scale_factor = Director::getInstance()->getContentScaleFactor();
	Size s = Size(SIZE_WIDTH / scale_factor, SIZE_HEIGHT / scale_factor);

	_body = PhysicsBody::createBox(s, PhysicsMaterial(0.1F, 0.0f, 0.0f));
	_body->setDynamic(false);
	this->setPhysicsBody(_body);

	// call Projectile's func
	this->setColliderBitMask();
}

bool ZeusWrath::onContactBegin(const cocos2d::PhysicsContact &contact)
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
			Node* world = World::getCurrent();
			Node* parrent;


			m_pointEnd = target->getPosition();
			parrent = target->getParent();
			while (parrent != world && parrent != nullptr)
			{
				m_pointEnd = parrent->convertToWorldSpace(m_pointEnd);
				parrent = parrent->getParent();
			}// because of the anchorpoint
			m_pointEnd.y += target->getSprite()->getBoundingBox().getMidY();

			attack(target, Character::DIRECTION::BOTTOM);

			
			// spawn lightning fx
			drawLightning();
			m_pointStart = m_pointEnd;
		}
	}


	return true;
}

void ZeusWrath::drawLightning()
{
	Vec2 tmpPos = (m_pointStart + m_pointEnd) / 2.0f;
	float scale = m_pointEnd.distance(m_pointStart) / m_heightLightning;

	float radian = M_PI_2 - (m_pointEnd - m_pointStart).getAngle();

	m_lightning = MAKE_EFFECT(Effect::EFFECT_TYPE::BOLT_BLUE
		, tmpPos.x
		, tmpPos.y
		, 1
		, 1
		, scale);
	m_lightning->setRotation( CC_RADIANS_TO_DEGREES(radian));
	World::getCurrent()->addChild(m_lightning);
}

/* Reset this object
parameter: x, y, direct, team
// method called by pool*/
void ZeusWrath::reset(float params[10])
{
	this->setActive(true);
	_timer = ATTACK_TIME;
	this->scheduleUpdate();
	m_lightning = nullptr;

	this->setPosition(params[0], params[1]);
	m_pointEnd = m_pointStart = Vec2(params[0], params[1]);

	_team = params[3];
}