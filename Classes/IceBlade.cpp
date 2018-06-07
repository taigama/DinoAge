#include "IceBlade.h"



#include "MyPoolManager.h"
//#include "Effect.h"
#include "WorldScene.h"
#include "Block.h"



USING_NS_CC;


#define BLADE_DAMAGE 10.0f
#define BLADE_FALL_SPEED -300.0f

#define SPRITE_OFFSET_Y 5.0f

#define SPRITE_ROTATION_DEGREE 45.0f

IceBlade::~IceBlade()
{
	CC_SAFE_DELETE(act_animate);
}

bool IceBlade::init()
{
	if (!Projectile::init())
		return false;
	// sprite init & physic init were called in base

	_projectileType = PROJECTILE_TYPE::ICE_BLADE;

	m_speedY = BLADE_FALL_SPEED / Director::getInstance()->getContentScaleFactor();
	_speed = m_speedY * tan( SPRITE_ROTATION_DEGREE * M_PI / 180.0f);

	_damage = BLADE_DAMAGE;

	// cant prevented
	//_isStatic = true; // base class set this already
	
	return true;
}

void IceBlade::update(float delta)
{
	// go outside the screen? become in-active

	Size visibleSize = Director::getInstance()->getVisibleSize();
	Vec2 camPos = Camera::getDefaultCamera()->getPosition();
	//Vec2 myPos = this->getPosition();

	if (_position.y < camPos.y - visibleSize.height / 2)		
	{
		this->setActive(false);
	}
}






void IceBlade::initSprites()
{
	float scale_factor = Director::getInstance()->getContentScaleFactor();

	// coder note: after experiment this rain of fire => I see that no need to make animation of this fire ball
	_sprite = Sprite::create();
	_sprite->setPosition( - SPRITE_OFFSET_Y * tan(SPRITE_ROTATION_DEGREE * M_PI / 180.0f) / scale_factor, SPRITE_OFFSET_Y / scale_factor);
	_sprite->setScale(0.5f);
	this->addChild(_sprite);

	//_sprite->setRotation(-SPRITE_ROTATION_DEGREE);







	Vector<SpriteFrame*> frames;

	char str[100] = { 0 };
	SpriteFrame* frame;
	for (int i = 1; i <= 12; i++)
	{
		sprintf(str, "ice_blade (%d).png", i);
		frame = SpriteFrameCache::getInstance()->getSpriteFrameByName(str);
		frames.pushBack(frame);
	}

	auto anim = Animation::createWithSpriteFrames(frames, 0.032f);

	act_animate = RepeatForever::create(Animate::create(anim));
	act_animate->retain();

	
}

void IceBlade::initPhysics()
{
	float scale_factor = Director::getInstance()->getContentScaleFactor();

	Size s = Size(40.0f / scale_factor, 40.0f / scale_factor);

	_body = PhysicsBody::createBox(s, PhysicsMaterial(0.1F, 0.0f, 0.0f));
	_body->setDynamic(true);
	_body->setGravityEnable(false);
	this->setPhysicsBody(_body);

	// call Projectile's func
	this->setColliderBitMask();
}


bool IceBlade::onContactBegin(const cocos2d::PhysicsContact &contact)
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
			// algorithm for check true position in this world -#-#-#

			//Node* world = World::getInstance();
			Node* world = World::getCurrent();
			Node* parrent;


			Vec2 myPos = this->getPosition();
			parrent = this->getParent();
			while (parrent != world && parrent != nullptr)
			{
				myPos = parrent->convertToWorldSpace(myPos);
				parrent = parrent->getParent();
			}


			Vec2 tarPos = target->getPosition();
			parrent = target->getParent();
			while (parrent != world && parrent != nullptr)
			{
				tarPos = parrent->convertToWorldSpace(tarPos);
				parrent = parrent->getParent();
			}
			// -#-#-#-#-#-#-#-#-#-#-#

			Character::DIRECTION directForTarget;

			// horizontal collision check
			if (myPos.x < tarPos.x)
			{
				directForTarget = Character::DIRECTION::LEFT;
			}
			else
			{
				directForTarget = Character::DIRECTION::RIGHT;
			}

			attack(target, directForTarget);
			AUDIO::play2d("ice_crash.ogg");

		}
	}// create crash when touch ground
	else if (targetBody->getCategoryBitmask() == (int)GameObject::OBJECT_TYPE::BLOCK)
	{
		Block* target = (Block*)targetBody->getOwner();
		/*
		Sprite* tarSprite = target->getSprite();
		if (tarSprite!= nullptr && !tarSprite->isVisible())
		{
			return false;
		}*/

		Node* world = World::getCurrent();
		Node* parrent;


		Vec2 myPos = this->getPosition();
		parrent = this->getParent();
		while (parrent != world && parrent != nullptr)
		{
			myPos = parrent->convertToWorldSpace(myPos);
			parrent = parrent->getParent();
		}


		Vec2 tarPos = target->getPosition();
		parrent = target->getParent();
		while (parrent != world && parrent != nullptr)
		{
			tarPos = parrent->convertToWorldSpace(tarPos);
			parrent = parrent->getParent();
		}
		// -#-#-#-#-#-#-#-#-#-#-#

		// this strike the ground from top
		if (myPos.y > tarPos.y && myPos.x < (tarPos.x + target->size.width / 2) && myPos.x >(tarPos.x - target->size.width / 2))
		{
			SPAWN_PROJECTILE(Projectile::ICE_BLADE_STRIKE
				, _position.x
				, _position.y
				, m_direction
				, _team
				, World::WORLD_LAYER::OBJECT);
			AUDIO::play2d("ice_crash.ogg");
		}
	}



	return true;
}

/* Reset this object
parameter: x, y, direct, team
// method called by pool*/
void IceBlade::reset(float params[10])
{
	this->setActive(true);
	scheduleUpdate();
	_sprite->runAction(act_animate);

	this->setPosition(Vec2(params[0], params[1]));

	this->setScaleX(params[2]);
	m_direction = (int) params[2];

	//_body->setVelocity(Vec2(0, _speed));
	// falling diagonal
	_body->setVelocity(Vec2( -params[2] * _speed, m_speedY));

	_team = params[3];
}

void IceBlade::attack(Character* target, Character::DIRECTION dir)
{
	// make enemy become hurted
	Projectile::attack(target, dir);

	
	/*SPAWN_EFFECT(Effect::EFFECT_TYPE::FIRE_EXPLODE_LARGE, this->getPositionX(), this->getPositionY(), 1, 1, 0.5f, World::WORLD_LAYER::EFFECT_FRONT);


	this->setActive(false);*/
}