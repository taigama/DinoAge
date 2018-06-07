#include "PushableBlock.h"

USING_NS_CC;

#define STANDING_VECTOR_Y_LIMIT 1.0f

PushableBlock::~PushableBlock() {}

bool PushableBlock::init()
{
	if (!Block::init())
		return false;

	_blockType = BLOCK_TYPE::PUSHABLE;
	m_vecPush = Vec2::ZERO;
	m_xPush = 0.0f;

	return true;
}

void PushableBlock::update(float delta)
{
	m_vecPush = _body->getVelocity();

	if (m_vecPush.y > -STANDING_VECTOR_Y_LIMIT && m_vecPush.y < STANDING_VECTOR_Y_LIMIT)
	{
		m_vecPush.x = m_xPush;
	}
	else
	{
		m_vecPush.x = 0.0f;
	}

	_body->setVelocity(m_vecPush);
	m_xPush = 0.0f;
}

void PushableBlock::push(float x_vecPush)
{
	m_xPush = x_vecPush;
}


void PushableBlock::initSprites()
{
	_sprite = Sprite::create("pushable_block.png");
	this->addChild(_sprite);


	size = _sprite->getBoundingBox().size;
}

void PushableBlock::init(bool isHidden, bool isOneWay
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
	_body->getShapes().at(0)->setFriction(0.0f);


	_isHidden = isHidden;
	_isOneWay = isOneWay;
}

//void PushableBlock::initPhysics()
//{
//	
//	_body = PhysicsBody::createBox(size, PhysicsMaterial(BLOCK_DENSITY, 0.0f, 0.0f));
//	_body->setRotationEnable(false);
//
//	// different
//	_body->setDynamic(true);
//
//	setColliderBitMask();
//
//	setPhysicsBody(_body);
//}


void PushableBlock::setColliderBitMask()
{
	if (_body == nullptr)
		return;

	_body->setCategoryBitmask((int)OBJECT_TYPE::BLOCK);
	_body->setCollisionBitmask(
		(int)OBJECT_TYPE::CHARACTER
		| (int)OBJECT_TYPE::ITEM
		| (int)OBJECT_TYPE::BLOCK
	);
	_body->setContactTestBitmask(
		(int)OBJECT_TYPE::CHARACTER
		| (int)OBJECT_TYPE::PROJECTILE
	);
}