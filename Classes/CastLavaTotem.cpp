#include "CastLavaTotem.h"



#include "MyPoolManager.h"
#include "WorldScene.h"



USING_NS_CC;


#define SPEED 200
#define HEIGHT_MODIFIER_UP 60
#define HEIGHT_MODIFIER_DOWN 230





CastLavaTotem::~CastLavaTotem() {}

bool CastLavaTotem::init()
{
	if (!Dummy::init())
		return false;
	// sprite init & physic init were called in base

	_dummyType = DUMMY_TYPE::CAST_LAVA_TOTEM;

	_speed = SPEED / Director::getInstance()->getContentScaleFactor();

	m_wasStrikes = nullptr;

	info_halfScreenWidth = Director::getInstance()->getVisibleSize().width / 2.0f;
	
	
	// because this dummy init after scene init complete
	m_camera = Camera::getDefaultCamera();
	// so you won't meet bug about no default camera

	return true;
}

void CastLavaTotem::update(float delta)
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
		CC_SAFE_DELETE_ARRAY(m_wasStrikes);
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
void CastLavaTotem::reset(float params[10])
{
	m_camera = Camera::getDefaultCamera();
	this->setActive(true);
	scheduleUpdate();// because this object need to be updated

	this->setPosition(Vec2(params[0], params[1]));
	m_posX = params[0];
	m_posY = params[1] + HEIGHT_MODIFIER_UP;

	_direction = params[2];
	_team = params[3];
	_speed = params[4] / Director::getInstance()->getContentScaleFactor();

	m_delay = params[5];
	_timer = m_delay;

	m_maxCountStrike = params[6];
	m_countStrike = 0;

	m_wasStrikes = new bool[m_maxCountStrike];
	for (int i = 0; i < m_maxCountStrike; i++)
	{
		m_wasStrikes[i] = false;
	}
}

void CastLavaTotem::strike()
{
	float scale_factor = Director::getInstance()->getContentScaleFactor();
	auto physics_world = Director::getInstance()->getRunningScene()->getPhysicsWorld();

	Vec2 startPos = Vec2(m_posX, m_posY);
	Vec2 endPos = Vec2(m_posX, m_posY - HEIGHT_MODIFIER_DOWN / scale_factor);

	physics_world->rayCast(
		CC_CALLBACK_3(CastLavaTotem::raycastCallback, this), startPos, endPos, &(m_wasStrikes[m_countStrike]));


	//SPAWN_PROJECTILE(
	//	Projectile::PROJECTILE_TYPE::LAVA_TOTEM
	//	, m_posX, m_posY
	//	, 1
	//	, _team
	//	, World::WORLD_LAYER::EFFECT_FRONT);



	m_countStrike++;
	if (m_countStrike >= m_maxCountStrike)
	{
		CC_SAFE_DELETE_ARRAY(m_wasStrikes);
		this->setActive(false);
	}
}


// callback to make ice wall
bool CastLavaTotem::raycastCallback(cocos2d::PhysicsWorld& world, const cocos2d::PhysicsRayCastInfo& info, void* data)
{
	// ################ the coder doesn't really understand the way physics world callback
	if (m_wasStrikes == nullptr)
	{
		MessageBox("Callback after delete m_wasStrikes[]!", "CastLavaTotem::raycastCallback");
		return false;
	}
	// #########################################33

	if (*((bool*)data) == true)
		return false;


	auto contactBody = info.shape->getBody();
	

	if (contactBody->getCategoryBitmask() != (int)GameObject::OBJECT_TYPE::BLOCK)
		return true;

	Size* boxSize = (Size*) contactBody->getTag();
	Vec2 targetPosition = contactBody->getPosition();

	if ((info.contact.y + 2) < (targetPosition.y + boxSize->height / 2.0f))
		return true;
	
	// prevent inside the land when initilized
	if (info.contact.distance( Vec2(m_posX, m_posY)) > 0.01f)
	{
		AUDIO::play2d("ST0_1_1_00007.ogg");
		SPAWN_PROJECTILE(Projectile::PROJECTILE_TYPE::LAVA_TOTEM,
			info.contact.x, // X
			info.contact.y + (50.0f / Director::getInstance()->getContentScaleFactor()), // Y
			_direction, // direction
			_team,
			World::WORLD_LAYER::OBJECT);
			
		*((bool*)data) = true;
		return false;
	}
	

	return true;
}