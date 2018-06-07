#include "CastThunderBody.h"



#include "MyPoolManager.h"
#include "WorldScene.h"



USING_NS_CC;


#define SPEED 200
#define HEIGHT_MODIFIER 25





CastThunderBody::~CastThunderBody() {}

bool CastThunderBody::init()
{
	if (!Dummy::init())
		return false;
	// sprite init & physic init were called in base

	_dummyType = DUMMY_TYPE::CAST_THUNDER_BODY;

	_speed = SPEED / Director::getInstance()->getContentScaleFactor();

	this->setPosition(Vec2::ZERO);
	return true;
}

void CastThunderBody::update(float delta)
{
	_timer -= delta;
	if (_timer <= 0)
	{
		strike();
		_timer = m_delay;
	}
}

/* Reset this object
*params[0], [1] : position x, y of object
*params[2] : direction
*params[3] : team
*params[4] : speed
*params[5] : delay_per_cast
*params[6] : repeat times
*params[7] : width of area
### METHOD CALLED BY POOL ###*/
void CastThunderBody::reset(float params[10])
{
	this->setActive(true);
	scheduleUpdate();// because this object need to be updated

	this->setPosition(Vec2(params[0], params[1]));

	_direction = params[2];
	_team = params[3];
	_speed = params[4] / Director::getInstance()->getContentScaleFactor();

	m_delay = params[5];
	_timer = m_delay;

	m_maxCountStrike = params[6];
	m_countStrike = 0;
}

void CastThunderBody::strike()
{
	m_myPos = _host->convertToWorldSpace(_position);

	SPAWN_PROJECTILE(
		Projectile::PROJECTILE_TYPE::THUNDER_BODY
		, m_myPos.x, m_myPos.y
		, 1
		, _team
		, World::WORLD_LAYER::OBJECT);
	SPAWN_EFFECT(
		Effect::EFFECT_TYPE::BLAST
		, m_myPos.x, m_myPos.y
		, 1
		, 1
		, 1
		, World::WORLD_LAYER::EFFECT_FRONT);


	m_countStrike++;
	if (m_countStrike >= m_maxCountStrike)
	{
		this->setActive(false);
	}
}