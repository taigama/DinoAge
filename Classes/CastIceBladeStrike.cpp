#include "CastIceBladeStrike.h"



#include "MyPoolManager.h"
#include "WorldScene.h"



USING_NS_CC;






CastIceBladeStrike::~CastIceBladeStrike() {}

bool CastIceBladeStrike::init()
{
	if (!Dummy::init())
		return false;
	// sprite init & physic init were called in base

	_dummyType = DUMMY_TYPE::CAST_GLACIAL_BLADE;

	// because this dummy init after scene init complete
	m_camera = Camera::getDefaultCamera();
	// so you won't meet bug about no default camera

	info_screenHeight = Director::getInstance()->getVisibleSize().height; // 2.0f;

	return true;
}

void CastIceBladeStrike::update(float delta)
{
	m_posX += _speed * delta * _direction;
	this->setPositionX(m_posX);

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
void CastIceBladeStrike::reset(float params[10])
{
	m_camera = Camera::getDefaultCamera();
	this->setActive(true);
	scheduleUpdate();// because this object need to be updated

	this->setPositionX(params[0]);
	m_posX = params[0] - params[2] * 200 / Director::getInstance()->getContentScaleFactor();
	// automatically get the position y
	m_posY = m_camera->getPosition().y + 300 / Director::getInstance()->getContentScaleFactor();// top of screen
	this->setPositionY(m_posY);

	_direction = params[2];
	_team = params[3];
	_speed = params[4] / Director::getInstance()->getContentScaleFactor();

	m_delay = params[5];
	_timer = m_delay;

	m_maxCountStrike = params[6];
	m_countStrike = 0;

	m_halfWidthArea = params[7] / 2.0f;
}

void CastIceBladeStrike::strike()
{

	// SPAWN_PROJECTILE lightning_bolt
	SPAWN_PROJECTILE(
		Projectile::PROJECTILE_TYPE::ICE_BLADE
		, RandomHelper::random_int<int>( m_posX - m_halfWidthArea, m_posX + m_halfWidthArea), m_posY
		, _direction
		, _team
		, World::WORLD_LAYER::EFFECT_FRONT);

	

	m_countStrike++;
	if (m_countStrike >= m_maxCountStrike)
	{
		this->setActive(false);
	}
}