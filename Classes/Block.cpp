#include "Block.h"

#include "Ground.h"
#include "GroundSprite.h"
#include "PushableBlock.h"
#include "MovingBlock.h"
#include "HiddenBlock.h"
//#include "MovingBlockTrigger.h"

#include "ResourceManager.h"

USING_NS_CC;

Block::~Block() { /*CC_SAFE_DELETE(rect);*/ }

Rect Block::getBoundingBox() const
{
	return Rect(Vec2::ZERO,size);
}

bool Block::init()
{
	size = Size();

	if (!GameObject::init())
		return false;

	_isOneWay = false;
	_isHidden = false;
	
	_team = 0;// neutral

	return true;
}

// isHidden, isOneWay <bool>
// x1, y1, x2, y2 : based on type
void Block::init(bool isHidden, bool isOneWay
	, int x1, int y1
	, int x2, int y2
	, int zone_ID)
{
	_isHidden = isHidden;
	_isOneWay = isOneWay;
}

void Block::update(float delta)
{

}

/* create block
* type enum BLOCK_TYPE
* x, y is the center position
* width, height is the size of block
* physics of block created here*/
Block* Block::createBlock(		
	BLOCK_TYPE type
	, float x, float y
	, float width, float height
	, bool isHidden, bool isOneWay
	, int x1, int y1// based on type
	, int x2, int y2
	, int zone_ID)
{
	Block* result = nullptr;
	switch (type)
	{
	case BLOCK_TYPE::STATIC:
	{
		result = Ground::create();
	}
		break;
	case BLOCK_TYPE::DESTRUCTIBLE:
		break;
	case BLOCK_TYPE::PUSHABLE:
	{
		result = PushableBlock::create();
	}
		break;
	case BLOCK_TYPE::MOVING:
	{		
		result = MovingBlock::create();
		result->convertTopLeftToMid(x1, y1, width, height);
		result->convertTopLeftToMid(x2, y2, width, height);
	}
	break;
	/*case BLOCK_TYPE::MOVING_BY_TRIGGER:
	{
		result = MovingBlockTrigger::create();
		result->convertTopLeftToMid(x1, y1, width, height);
	}
	break;*/
	case BLOCK_TYPE::GROUND_SPRITE:
	{
		result = GroundSprite::create();
	}
	break;
	case BLOCK_TYPE::HIDDEN:
	{
		result = HiddenBlock::create();
	}
	break;
	}

	if (result)
	{
		result->setPosition(Vec2(x, y));

		// add physics to result
		if (result->_body == nullptr)
		{
			//result->rect = new Rect(x - width / 2, y - height / 2, width, height);
			result->size.setSize(width, height);

			result->_body = PhysicsBody::createBox(result->size, PhysicsMaterial(BLOCK_DENSITY, 0.0f, BLOCK_FRICTION));

			result->_body->setDynamic(false);

			result->setColliderBitMask();

			result->setPhysicsBody(result->_body);
		}

	}

	result->init(isHidden, isOneWay, x1, y1, x2, y2, zone_ID);

	// result must be added to world
	return result;
}

void Block::convertTopLeftToMid(int &x, int &y, const float &width, const float &height)
{
	float scale_factor = Director::getInstance()->getContentScaleFactor();

	x /= scale_factor;
	y /= scale_factor;

	x += width / 2.0f;

	y = ResourceManager::getInstance()->getArea().size.height - y;
	y -= height / 2.0f;
}

// ### UNCOMPLETE FUNCION ###
//Block* Block::createPoly(float x, float y, Vec2* points, int count)
//{
//	Block* result = Ground::create();
//	result->_blockType = BLOCK_TYPE::POLYGON_STATIC;
//
//
//	if (result)
//	{
//		result->setPosition(Vec2(x, y));
//
//		// add physics to result
//		result->_body = PhysicsBody::createPolygon(points, count, PhysicsMaterial(1.0f, 0.0f, 1.0f));
//		result->_body->setDynamic(false);
//
//		result->setColliderBitMask();
//
//		result->setPhysicsBody(result->_body);
//	}
//
//	return result;
//}



// using in createBlock()
void Block::setColliderBitMask()
{
	if (_body == nullptr)
		return;

	_body->setCategoryBitmask((int)OBJECT_TYPE::BLOCK);
	_body->setCollisionBitmask(
		(int)OBJECT_TYPE::CHARACTER
		| (int) OBJECT_TYPE::ITEM
		| (int) OBJECT_TYPE::BLOCK
	);
	_body->setContactTestBitmask(
		(int)OBJECT_TYPE::CHARACTER
		| (int)OBJECT_TYPE::PROJECTILE
		| (int)OBJECT_TYPE::BLOCK
	);
}

/* auto callback
* I dunno what I need to do :v*/
bool Block::onContactBegin(const cocos2d::PhysicsContact &contact)
{
	// Retrieves PhysicsBody of target
	auto targetBody = contact.getShapeA()->getBody();

	if (_body == targetBody)		// if current body is of shape A
	{
		targetBody = contact.getShapeB()->getBody();
	}

	// target is block
	if (targetBody->getCategoryBitmask() == (int)GameObject::OBJECT_TYPE::CHARACTER)
	{
		if (_isHidden)
		{
			if (_sprite)
			{
				_sprite->setVisible(true);
				CCLOG("Visible on!");
				_isHidden = true;
			}
		}

	}

	


	return true;
}
//
//bool Block::onContactSeparate (const cocos2d::PhysicsContact &contact)
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
//		if (_isHidden)
//		{
//			if (_sprite)
//			{
//				_sprite->setVisible(false);
//				CCLOG("Visible off!");
//			}
//		}
//
//	}
//
//
//	return true;
//}