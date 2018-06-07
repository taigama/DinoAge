#include "MovingBlock.h"

#include "TriggerBlockZone.h"
#include "ResourceManager.h"
#include "WorldScene.h"

USING_NS_CC;


#define TIME_RESET_VELOCITY 5.0f

#define MIN_DISTANCE 1.0f

#define BLOCK_SPEED 50.0f

#define DIAGONAL_NORMAL_CONS 0.7f// sqrt(2) / 2

#define MASS 999999999.0f

MovingBlock::~MovingBlock() {}

bool MovingBlock::init()
{
	if (!Block::init())
		return false;

	_blockType = BLOCK_TYPE::MOVING;

	m_speed = BLOCK_SPEED / Director::getInstance()->getContentScaleFactor();
	m_timer = TIME_RESET_VELOCITY;

	m_isOn = true;

	return true;
}

// x1, y1 : point_start
// x2, y2 :point_end
void MovingBlock::init(bool isHidden, bool isOneWay
	, int x1, int y1
	, int x2, int y2
	, int zone_ID)
{
	// change scale of the sprite
	Size sizeSprite = _sprite->getBoundingBox().size;
	float scaleX = size.width / sizeSprite.width;
	float scaleY = size.height / sizeSprite.height;
	_sprite->setScale(scaleX, scaleY);

	// change physics info
	_body->setDynamic(true);
	_body->setRotationEnable(false);
	_body->setGravityEnable(false);
	_body->setMass(MASS);


	_isHidden = isHidden;
	_isOneWay = isOneWay;


	m_startPoint.x = x1;
	m_startPoint.y = y1;

	m_endPoint.x = x2;
	m_endPoint.y = y2;

	m_vecMove = m_endPoint - _position;
	m_isToEnd = true;

	if (m_vecMove.length() < m_speed)
	{
		m_vecMove = m_startPoint - _position;
		if (m_vecMove.length() < m_speed)// the length of the road are too short for this speed!
		{
			m_vecMove = Vec2::ZERO;
			m_speed = 0;
		}

		m_isToEnd = false;
	}

	m_vecMove.normalize();
	_body->setVelocity(m_vecMove * m_speed);


	if (zone_ID != 0)
	{
		_sprite->setVisible(false);
		_body->setEnabled(false);


		// search zone for binding this block	
		Zone* tmpZone = nullptr;
		tmpZone = (Zone*)World::getCurrent()->zones.at(zone_ID);
		if (tmpZone != nullptr)
		{
			((TriggerBlockZone*)tmpZone)->setBlockTrigger(this);
			m_isOn = false;
			_sprite->setVisible(false);
			_body->setEnabled(false);
			return;
		}
		MessageBox("Zone not found!", "MovingBlock::init(...)");
	}
	updateArrow();
}


void MovingBlock::initSprites()
{
	auto spriteCache = SpriteFrameCache::getInstance();

	_sprite = Sprite::createWithSpriteFrameName("moving_block.png");
	this->addChild(_sprite, 0);

	/*Size size = _sprite->getBoundingBox().size;

	float scaleX = width / size.width;
	float scaleY = height / size.height;
	_sprite->setScale(scaleX, scaleY);*/


	m_arrowSprite = Sprite::create();
	m_frameUp = spriteCache->getSpriteFrameByName("arrow_up.png");
	m_frameDown = spriteCache->getSpriteFrameByName("arrow_down.png");
	m_frameLeft = spriteCache->getSpriteFrameByName("arrow_left.png");
	m_frameRight = spriteCache->getSpriteFrameByName("arrow_right.png");

	this->addChild(m_arrowSprite, 1);
}

//void MovingBlock::initSpecialPhysics()
//{
//	rect = new Rect(0.0f, 0.0f, width, height);
//	_body = PhysicsBody::createBox(rect->size, PhysicsMaterial(BLOCK_DENSITY * 1000000000, 0.0f, BLOCK_FRICTION));
//	
//	_body->setDynamic(true);
//	_body->setRotationEnable(false);
//	_body->setGravityEnable(false);
//
//	setColliderBitMask();
//
//	setPhysicsBody(_body);
//}


void MovingBlock::update(float delta)
{
	// this is not turned on yet
	if (!m_isOn)
		return;

	// update velocity per interval
	m_timer -= delta;
	if (m_timer <= 0)
	{
		updateVelocity();
	}
	
	// check distance for reverse direction
	if (m_isToEnd)
	{
		if (_position.distance(m_endPoint) <= MIN_DISTANCE)
		{
			m_isToEnd = false;
			updateVelocity();
		}
	}
	else if (_position.distance(m_startPoint) <= MIN_DISTANCE)
	{
		m_isToEnd = true;
		updateVelocity();
	}
	
}

void MovingBlock::updateVelocity()
{
	m_timer += TIME_RESET_VELOCITY;

	if (m_isToEnd)
	{
		m_vecMove = m_endPoint - _position;
	}
	else
	{
		m_vecMove = m_startPoint - _position;
	}

	m_vecMove.normalize();
	_body->setVelocity(m_vecMove * m_speed);

	updateArrow();
}

void MovingBlock::updateArrow()
{
	if (m_vecMove.y > DIAGONAL_NORMAL_CONS)
	{
		m_arrowSprite->setSpriteFrame(m_frameUp);
	}
	else if (m_vecMove.y < -DIAGONAL_NORMAL_CONS)
	{
		m_arrowSprite->setSpriteFrame(m_frameDown);
	}
	else if (m_vecMove.x > DIAGONAL_NORMAL_CONS)
	{
		m_arrowSprite->setSpriteFrame(m_frameRight);
	}
	else
	{
		m_arrowSprite->setSpriteFrame(m_frameLeft);
	}
}

void MovingBlock::triggered()
{
	if (!m_isOn)
	{
		_sprite->setVisible(true);
		_body->setEnabled(true);

		m_isOn = true;
		updateVelocity();
	}
}