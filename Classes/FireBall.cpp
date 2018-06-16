#include "FireBall.h"



#include "MyPoolManager.h"
//#include "Effect.h"
#include "WorldScene.h"



USING_NS_CC;


#define FIRE_DAMAGE 6.0f
#define FIRE_FALL_SPEED -500.0f

#define SPRITE_OFFSET_Y 5.0f

#define SPRITE_ROTATION_DEGREE 30.0f


FireBall::~FireBall()
{
	//CC_SAFE_DELETE(act_animate);
}

bool FireBall::init()
{
	if (!Projectile::init())
		return false;
	// sprite init & physic init were called in base

	_projectileType = PROJECTILE_TYPE::FIREBALL;

	m_speedY = FIRE_FALL_SPEED / Director::getInstance()->getContentScaleFactor();
	_speed = m_speedY * tan( SPRITE_ROTATION_DEGREE * M_PI / 180.0f);

	_damage = FIRE_DAMAGE;

	_isStatic = false;
	
	return true;
}

void FireBall::update(float delta)
{
	// go outside the screen? become in-active

	Size visibleSize = Director::getInstance()->getVisibleSize();
	Vec2 camPos = Camera::getDefaultCamera()->getPosition();
	Vec2 myPos = this->getPosition();

	if (myPos.y < camPos.y - visibleSize.height / 2)		
	{
		this->setActive(false);
	}
}






void FireBall::initSprites()
{
	float scale_factor = Director::getInstance()->getContentScaleFactor();

	// coder note: after experiment this rain of fire => I see that no need to make animation of this fire ball
	_sprite = Sprite::createWithSpriteFrameName("fire_ball_2.png");
	_sprite->setPosition( - SPRITE_OFFSET_Y * tan(SPRITE_ROTATION_DEGREE * M_PI / 180.0f) / scale_factor, SPRITE_OFFSET_Y / scale_factor);
	this->addChild(_sprite);

	_sprite->setRotation(-SPRITE_ROTATION_DEGREE);







	/*Vector<SpriteFrame*> frames;

	char str[100] = { 0 };
	SpriteFrame* frame;
	for (int i = 0; i <= 6; i++)
	{
		sprintf(str, "fire_ball_%d.png", i);
		frame = SpriteFrameCache::getInstance()->getSpriteFrameByName(str);
		frames.pushBack(frame);
	}

	auto anim = Animation::createWithSpriteFrames(frames, 0.08f);

	act_animate = RepeatForever::create(Animate::create(anim));
	act_animate->retain();*/
}

void FireBall::initPhysics()
{
	float scale_factor = Director::getInstance()->getContentScaleFactor();

	Size s = Size(12.0f / scale_factor, 12.0f / scale_factor);

	_body = PhysicsBody::createBox(s, PhysicsMaterial(0.1F, 0.0f, 0.0f));
	_body->setDynamic(true);
	_body->setGravityEnable(false);
	this->setPhysicsBody(_body);

	// call Projectile's func
	this->setColliderBitMask();
}


//bool FireBall::onContactBegin(const cocos2d::PhysicsContact &contact)
//{
//	// Retrieves PhysicsBody of target
//	auto targetBody = contact.getShapeA()->getBody();
//
//	if (_body == targetBody)		// if current body is of shape A
//	{
//		targetBody = contact.getShapeB()->getBody();
//	}
//
//
//	// Checks whether the target is Player
//	if (targetBody->getCategoryBitmask() == (int)GameObject::OBJECT_TYPE::CHARACTER)
//	{
//		Character* target = (Character*)targetBody->getOwner();
//
//		// Target is of opposite team -> inflicts damage
//		if (target->getTeam() != _team)
//		{
//			// algorithm for check true position in this world -#-#-#
//
//			//Node* world = World::getInstance();
//			Node* world = World::getCurrent();
//			Node* parrent;
//
//
//			Vec2 myPos = this->getPosition();
//			parrent = this->getParent();
//			while (parrent != world && parrent != nullptr)
//			{
//				myPos = parrent->convertToWorldSpace(myPos);
//				parrent = parrent->getParent();
//			}
//
//
//			Vec2 tarPos = target->getPosition();
//			parrent = target->getParent();
//			while (parrent != world && parrent != nullptr)
//			{
//				tarPos = parrent->convertToWorldSpace(tarPos);
//				parrent = parrent->getParent();
//			}
//			// -#-#-#-#-#-#-#-#-#-#-#
//
//			Character::DIRECTION directForTarget;
//
//			// horizontal collision check
//			if (myPos.x < tarPos.x)
//			{
//				directForTarget = Character::DIRECTION::LEFT;
//			}
//			else
//			{
//				directForTarget = Character::DIRECTION::RIGHT;
//			}
//			attack(target, directForTarget);
//		}
//	}// disapear when touch block
//	else if (targetBody->getCategoryBitmask() == (int)GameObject::OBJECT_TYPE::BLOCK)
//	{
//		SPAWN_EFFECT(Effect::EFFECT_TYPE::FIRE_EXPLODE_LARGE, this->getPositionX(), this->getPositionY(), 1, 1, 0.5f, World::WORLD_LAYER::EFFECT_FRONT);
//		this->setActive(false);
//	}
//
//
//
//	return true;
//}

/* Reset this object
parameter: x, y, direct, team
// method called by pool*/
void FireBall::reset(float params[10])
{
	this->setActive(true);
	scheduleUpdate();

	this->setPosition(Vec2(params[0], params[1]));
	_team = params[3];

	//_body->setVelocity(Vec2(0, _speed));
	// falling diagonal
	this->setScaleX(params[2]);
	_body->setVelocity(Vec2( -params[2] * _speed, m_speedY));


	//this->runAction(RepeatForever::create(RotateBy::create(0.5f, Vec3(0, 0, -360.0f * params[2]))));
	//_sprite->runAction(act_animate);	
}

void FireBall::attack(Character* target, Character::DIRECTION dir)
{
	// make enemy become hurted
	Projectile::attack(target, dir);
	
	onDie();
}

void FireBall::onDie()
{
	int id = AUDIO::play2d("fire_crash.ogg");
	AUDIO::setVolume(id, 0.3f);
	SPAWN_EFFECT(Effect::EFFECT_TYPE::FIRE_EXPLODE_LARGE, this->getPositionX(), this->getPositionY(), 1, 1, 0.5f, World::WORLD_LAYER::EFFECT_FRONT);

	this->setActive(false);
}