#include "CastLightningStrike.h"



#include "MyPoolManager.h"
#include "WorldScene.h"



USING_NS_CC;


#define SPEED 200
#define HEIGHT_MODIFIER 25





CastLightningStrike::~CastLightningStrike() {}

bool CastLightningStrike::init()
{
	if (!Dummy::init())
		return false;
	// sprite init & physic init were called in base

	_dummyType = DUMMY_TYPE::CAST_LIGHTNING_STRIKE;

	_speed = SPEED / Director::getInstance()->getContentScaleFactor();

	info_halfScreenWidth = Director::getInstance()->getVisibleSize().width / 2.0f;
	
	
	// because this dummy init after scene init complete
	m_camera = Camera::getDefaultCamera();
	// so you won't meet bug about no default camera

	return true;
}

void CastLightningStrike::update(float delta)
{
	m_posX += _speed * delta * _direction;
	this->setPositionX(m_posX);

	_timer -= delta;
	if (_timer <= 0)
	{
		strike();
		_timer = m_delay;
	}

	m_posCamera = m_camera->getPosition();
	if (m_posX > (m_posCamera.x + info_halfScreenWidth) || m_posX < (m_posCamera.x - info_halfScreenWidth))
	{
		this->setActive(false);
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
void CastLightningStrike::reset(float params[10])
{
	m_camera = Camera::getDefaultCamera();
	this->setActive(true);
	scheduleUpdate();// because this object need to be updated

	this->setPosition(Vec2(params[0], params[1]));
	m_posX = params[0];
	m_posY = params[1] + HEIGHT_MODIFIER;

	_direction = params[2];
	_team = params[3];
	_speed = params[4] / Director::getInstance()->getContentScaleFactor();

	m_delay = params[5];
	_timer = m_delay;

	m_maxCountStrike = params[6];
	m_countStrike = 0;
}

void CastLightningStrike::strike()
{

	// SPAWN_PROJECTILE lightning_bolt
	SPAWN_PROJECTILE(
		Projectile::PROJECTILE_TYPE::LIGHTNING_STRIKE
		, m_posX, m_posY
		, 1
		, _team
		, World::WORLD_LAYER::EFFECT_FRONT);



	m_countStrike++;
	if (m_countStrike >= m_maxCountStrike)
	{
		this->setActive(false);
	}
}