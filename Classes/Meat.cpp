#include "Meat.h"

#include "Item.h"

#define SPRITE_SCALE_FACTOR 0.4f

USING_NS_CC;

Meat::~Meat()
{

}

bool Meat::init()
{
	if (!Item::init())
	{
		return false;
	}

	_itemType = Item::ITEM_TYPE::MEAT;

	_value = 20;	// base value (for health)

	return true;
}

void Meat::initSprites()
{
	_sprite = Sprite::create("meat.png");

	_sprite->setNormalizedPosition(Vec2(0.5, 0.5));		// center of the node
	_sprite->setScale(SPRITE_SCALE_FACTOR);



	this->addChild(_sprite);
}

void Meat::initPhysics()
{
	_body = PhysicsBody::createBox(Size(_sprite->getBoundingBox().size.width * 0.6f, _sprite->getBoundingBox().size.height),
		PhysicsMaterial(0.1f, 0.0f, 0.5f));

	_body->setDynamic(false);

	this->addComponent(_body);

	// Helper functions for Physics handling
	setColliderBitMask();
}
