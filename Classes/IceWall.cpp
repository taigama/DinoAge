#include "IceWall.h"



#include "MyPoolManager.h"
//#include "Effect.h"
#include "WorldScene.h"
#include "Block.h"



USING_NS_CC;


#define VERTICAL_NORMAL_CONS 0.5f

#define WALL_DAMAGE 10.0f
#define WALL_LIFE_TIME 3.0f

#define SIZE_WIDTH 50.0f
#define SIZE_HEIGHT 80.0f

IceWall::~IceWall() {}

bool IceWall::init()
{
	if (!Projectile::init())
		return false;
	// sprite init & physic init were called in base

	_projectileType = PROJECTILE_TYPE::ICE_WALL;

	_damage = WALL_DAMAGE;


	
	return true;
}

void IceWall::update(float delta)
{	
	if (m_isNew)
	{
		if (act_animate->isDone())
		{
			m_isNew = false;
		}
	}

	if (_timer <= 0)
	{
		AUDIO::play2d("ice_crash.ogg");
		this->setActive(false);
	}
	_timer -= delta;
	if (_timer <= 0)
	{
		// this algorithm prevent this object disapear before the effect come
		SPAWN_EFFECT(
			Effect::EFFECT_TYPE::ICE_WALL_CRASH
			, this->getPositionX()
			, this->getPositionY()
			, this->getScaleX()// direction
			, 1 // repeat times
			, 1 // scale
			, World::WORLD_LAYER::EFFECT_FRONT);
	}
}






void IceWall::initSprites()
{
	//float scale_factor = Director::getInstance()->getContentScaleFactor();

	_sprite = Sprite::create();

	this->addChild(_sprite);




	Vector<SpriteFrame*> frames;

	char str[100] = { 0 };
	SpriteFrame* frame;
	for (int i = 0; i <= 2; i++)
	{
		sprintf(str, "ice_wall_%d.png", i);
		frame = SpriteFrameCache::getInstance()->getSpriteFrameByName(str);
		frames.pushBack(frame);
	}

	auto anim = Animation::createWithSpriteFrames(frames, 0.15f);

	act_animate = Animate::create(anim);
	act_animate->retain();
}

void IceWall::initPhysics()
{
	float scale_factor = Director::getInstance()->getContentScaleFactor();

	Size s = Size(SIZE_WIDTH / scale_factor, SIZE_HEIGHT / scale_factor);

	_body = PhysicsBody::createBox(s, PhysicsMaterial(0.1F, 0.0f, 0.0f));
	_body->setDynamic(true);
	_body->setGravityEnable(false);
	//_body->setPositionOffset(Vec2(SIZE_WIDTH * 0.1f / scale_factor, - SIZE_HEIGHT * 0.1f / scale_factor));
	this->setPhysicsBody(_body);

	// call Projectile's func
	this->setColliderBitMask();

	m_brick = Block::createBlock(
		Block::BLOCK_TYPE::STATIC
		, this->getPositionX(), this->getPositionY()
		, SIZE_WIDTH * 0.8f / scale_factor, SIZE_HEIGHT * 0.8f / scale_factor
		, false, false
		, 0, 0 , 0, 0, 0);// rubbish parameters


	m_brick->setPosition(SIZE_WIDTH * 0.1f / scale_factor, - SIZE_HEIGHT * 0.1f / scale_factor);
	this->addChild(m_brick);
}



/* Reset this object
parameter: x, y, direct, team
// method called by pool*/
void IceWall::reset(float params[10])
{
	this->setActive(true);
	scheduleUpdate();
	m_isNew = true;
	_timer = WALL_LIFE_TIME;

	this->setPosition(Vec2(params[0], params[1]));
	this->setScaleX(params[2]);

	float scale_factor = Director::getInstance()->getContentScaleFactor();
	_body->setPositionOffset(Vec2(params[2] * SIZE_WIDTH * 0.1f / scale_factor, -SIZE_HEIGHT * 0.1f / scale_factor));

	_sprite->runAction(act_animate);

	_team = params[3];
	((Block*)m_brick)->setTeam(params[3]);
}

// auto callback
bool IceWall::onContactBegin(const cocos2d::PhysicsContact &contact)
{
	// Retrieves PhysicsBody of target
	auto targetBody = contact.getShapeA()->getBody();

	if (_body == targetBody)		// if current body is of shape A
	{
		targetBody = contact.getShapeB()->getBody();
	}

	Vec2 normal = contact.getContactData()->normal;
	if (m_isNew)
	{
		// Checks whether the target is Player
		if (targetBody->getCategoryBitmask() == (int)GameObject::OBJECT_TYPE::CHARACTER)
		{
			Character* target = (Character*)targetBody->getOwner();
			

			// Target is of opposite team -> inflicts damage
			if (target->getTeam() != _team)
			{
				

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

				Character::DIRECTION directForTarget;


				// vertical collision
				if (normal.y < -VERTICAL_NORMAL_CONS || normal.y > VERTICAL_NORMAL_CONS)
				{
					if (myPos.y < tarPos.y)
					{
						directForTarget = Character::DIRECTION::BOTTOM;
						attack(target, directForTarget);
						return true;
					}
				}

				// horizontal collision
				if (myPos.x < tarPos.x)
				{
					directForTarget = Character::DIRECTION::LEFT;
				}
				else
				{
					directForTarget = Character::DIRECTION::RIGHT;
				}


				attack(target, directForTarget);
			}			
		}
	}
	
	// touch player => crash
	if (targetBody->getCategoryBitmask() == (int)GameObject::OBJECT_TYPE::CHARACTER)
	{
		if (((Character*)targetBody->getOwner())->getTeam() == _team)
		{
			SPAWN_EFFECT(
				Effect::EFFECT_TYPE::ICE_WALL_CRASH
				, this->getPositionX()
				, this->getPositionY()
				, this->getScaleX()// direction
				, 1 // repeat times
				, 1 // scale
				, World::WORLD_LAYER::EFFECT_FRONT);

			AUDIO::play2d("ice_crash.ogg");
			this->setActive(false);
		}
	}
	
	if (targetBody->getCategoryBitmask() == (int)GameObject::OBJECT_TYPE::PROJECTILE)
	{
		Projectile* theBullet = (Projectile*)targetBody->getOwner();

		// delete the opposite bullet
		if ((theBullet->getTeam() != _team) && (theBullet->isStatic() == false))
		{
			theBullet->setActive(false);
		}
	}



	return true;
}