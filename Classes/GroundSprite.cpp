#include "GroundSprite.h"

USING_NS_CC;


#define FLASH_TIME 0.2f

#define SPRITE_OFFSET_Y -7.0f

#define SIZE_WIDTH 96.0f
#define SIZE_HEIGHT 48.0f

GroundSprite::~GroundSprite() {}

bool GroundSprite::init()
{
	if (!Block::init())
		return false;

	_blockType = BLOCK_TYPE::GROUND_SPRITE;

	m_isFlash = false;
	m_timerFLash = FLASH_TIME;

	m_countCharTouch = 0;

	return true;
}

void GroundSprite::update(float delta)
{
	if (m_isFlash)
	{
		m_timerFLash -= delta;
		if (m_timerFLash <= 0)
		{
			// VISIBLE OFF
			m_isFlash = false;

			if (m_countCharTouch == 0)
			{
				_sprite->setVisible(false);
			}

		}
	}
}

void GroundSprite::initSprites()
{
	_sprite = Sprite::create("hidden_block.png");
	this->addChild(_sprite);
	_sprite->setVisible(false);

	/*auto rr = _sprite->getBoundingBox();
	auto size = rr.size;
	size.height /= 1.5f;

	rect = new Rect(rr.origin, size);*/

	//_sprite->setPosition(0.0f, SPRITE_OFFSET_Y / Director::getInstance()->getContentScaleFactor());
}
//
//void GroundSprite::initPhysics()
//{	
//	float scale_factor = Director::getInstance()->getContentScaleFactor();
//	size.setSize(SIZE_WIDTH / scale_factor, SIZE_HEIGHT / scale_factor);
//
//	//rect = new Rect(Vec2::ZERO, size);
//
//	_body = PhysicsBody::createBox(size, PhysicsMaterial(BLOCK_DENSITY, 0.0f, BLOCK_FRICTION));
//	_body->setRotationEnable(false);
//
//	_body->setDynamic(false);
//
//	setColliderBitMask();
//	setPhysicsBody(_body);
//}

void GroundSprite::init(bool isHidden, bool isOneWay
	, int x1, int y1
	, int x2, int y2
	, int zone_ID)
{
	// change scale of the sprite
	//Size sizeSprite = _sprite->getBoundingBox().size;
	float scaleX = size.width / SIZE_WIDTH;
	float scaleY = size.height / SIZE_HEIGHT;
	_sprite->setScale(scaleX, scaleY);
	_sprite->setPosition(0.0f, scaleY * SPRITE_OFFSET_Y / Director::getInstance()->getContentScaleFactor());

	// change physics info
	//_body->setDynamic(true);
	//_body->setRotationEnable(false);
	//_body->getShapes().at(0)->setFriction(0.0f);


	_isHidden = isHidden;
	_isOneWay = isOneWay;
}

//bool GroundSprite::onContactBegin(const cocos2d::PhysicsContact &contact)
//{
//	// Retrieves PhysicsBody of target
//	auto targetBody = contact.getShapeA()->getBody();
//
//	if (_body == targetBody)		// if current body is of shape A
//	{
//		targetBody = contact.getShapeB()->getBody();
//	}
//
//	// target is block
//	if (targetBody->getCategoryBitmask() == (int)GameObject::OBJECT_TYPE::CHARACTER)
//	{
//		m_countCharTouch++;
//		if (targetBody->getPosition().y > _position.y + size.height / 2.0f)
//		{
//			_sprite->setVisible(true);
//			return true;
//		}
//	}
//	else if (targetBody->getCategoryBitmask() == (int)GameObject::OBJECT_TYPE::PROJECTILE)
//	{
//		_sprite->setVisible(true);
//		m_isFlash = true;
//		m_timerFLash = FLASH_TIME;
//	}
//
//
//	return false;
//}
//
//bool GroundSprite::onContactSeparate (const cocos2d::PhysicsContact &contact)
//{
//	// Retrieves PhysicsBody of target
//	auto targetBody = contact.getShapeA()->getBody();
//
//	if (_body == targetBody)		// if current body is of shape A
//	{
//		targetBody = contact.getShapeB()->getBody();
//	}
//
//	// target is block
//	if (targetBody->getCategoryBitmask() == (int)GameObject::OBJECT_TYPE::CHARACTER)
//	{
//		m_countCharTouch--;
//
//		if (!m_isFlash && m_countCharTouch == 0) // not flashing by touching projectile
//		{
//			_sprite->setVisible(false);
//		}
//	}
//
//
//	return false;
//}