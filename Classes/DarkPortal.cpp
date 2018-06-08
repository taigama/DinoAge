#include "ResourceManager.h"
#include "MenuScene1.h"

#include "DarkPortal.h"

#include "WorldScene.h"
#include "MyPoolManager.h"
#include "EnemyFeeder.h"
#include "EnemyWerewolf.h"

USING_NS_CC;

#define SCALE 0.5f


#define MAX_HEALTH 200.0f
#define RAGE_HEALTH_FACTOR 0.3f
#define TEAM_ENEMY 2


#define TIME_CYCLE_CAST 5.0f
#define TIME_CYCLE_SPAWN 2.0f

#define SPAWN_OFFSET_Y -100.0f

#define RAGE_CAST_FACTOR 0.5f
#define RAGE_SPAWN_FACTOR 2.0f


#define TIME_CYCLE_ROTATE 1.0f
#define VORTEX_OFFSET_Y -22.0f
#define VORTEX_OPACITY 75


#define TIME_DIE 5.0f
#define EXPLODE_COUNT 50

DarkPortal::~DarkPortal()
{
	
}

bool DarkPortal::init()
{
	// Super init
	if (!Enemy::init())
	{
		return false;
	}
	this->setScale(SCALE);

	m_posSpawn = Vec2(0, SCALE * SPAWN_OFFSET_Y / Director::getInstance()->getContentScaleFactor());

	// Sets HP
	_maxHP = MAX_HEALTH;
	_hp = _maxHP;

	_team = TEAM_ENEMY;						// team 1 -> Player ; team 2 -> Enemy

	// Dark portal won't move!
	_speed = 0.0f;
	_jumpSpeed = 0.0f;

	// Sets time for hurting and dying animations
	_dieTime = TIME_DIE;


	m_timerSpawn = TIME_CYCLE_SPAWN;
	m_timerCast = TIME_CYCLE_CAST;
	m_isRage = false;

	_charType = Character::CHARACTER_TYPE::DARK_PORTAL;

	return true;
}

void DarkPortal::initSprites()
{
	_sprite = Sprite::create("dark_portal.png");
	this->addChild(_sprite, 0);

	auto vortex = Sprite::create("vortex.png");
	vortex->setPositionY(VORTEX_OFFSET_Y / Director::getInstance()->getContentScaleFactor());
	vortex->setOpacity(VORTEX_OPACITY);
	this->addChild(vortex, 1);

	auto rotate = RotateBy::create(TIME_CYCLE_ROTATE, -360.0f);
	vortex->runAction(RepeatForever::create(rotate));
}

void DarkPortal::initPhysics()
{
	Size s = _sprite->getBoundingBox().size;
	s.width *= 0.7f;
	s.height *= 0.98f;

	_body = PhysicsBody::createBox(s);
	_body->setDynamic(true);
	_body->setRotationEnable(false);
	_body->setGravityEnable(false);
	
	this->setPhysicsBody(_body);

	// set bit mask
	_body->setCategoryBitmask((int)OBJECT_TYPE::CHARACTER);	// character
	_body->setCollisionBitmask(0);
	_body->setContactTestBitmask((int)OBJECT_TYPE::PROJECTILE);
}

void DarkPortal::update(float delta)
{
	m_timerSpawn -= delta;

	if (m_timerSpawn <= 0)
	{
		m_timerSpawn += TIME_CYCLE_SPAWN;

		// spawn creep
		EnemyFeeder* creep = (EnemyFeeder*) Character::createCharacter(Character::CHARACTER_TYPE::ENEMY_FEEDER, _team);
		creep->setInitialDirection(Character::DIRECTION::LEFT);

		// Sets type and size for EnemyFeeder
		creep->setEnemyFeederType((EnemyFeeder::FEEDER_TYPE)2);
		creep->setEnemyFeederSize((EnemyFeeder::FEEDER_SIZE)1);		

		creep->setPosition(_position.x + m_posSpawn.x, _position.y + m_posSpawn.y);
		creep->startAction();
		this->getParent()->addChild(creep, World::WORLD_LAYER::OBJECT);
	}
}

void DarkPortal::onHit(float dmg, DIRECTION dir, float force)
{
	// Calls super version
	Enemy::onHit(dmg, dir, force);

	if ((!m_isRage) && (_hp <= _maxHP / RAGE_HEALTH_FACTOR))
	{
		m_isRage = true;

		// not implemented yet
	}
}

void DarkPortal::onDie()
{
	m_timerSpawn = INFINITY;
	m_timerCast = INFINITY;

	char str[16];
	float timer = 0.0f;
	Vec2 posExplode = Vec2();

	Size s = _sprite->getBoundingBox().size * SCALE * 0.5f;
	int left =(int) -(s.width / 0.8f) / 2.0f;
	int right =(int) (s.width / 0.8f) / 2.0f;
	int low =(int) -(s.height / 0.8f) / 2.0f;
	int height =(int) (s.height / 0.8f) / 2.0f;

	_body->setEnabled(false);

	// schedule effects
	for (int i = 0; i < EXPLODE_COUNT; i++)
	{
		timer += _dieTime / EXPLODE_COUNT;
		sprintf(str, "DarkDie_%d", i);

		posExplode.x = RandomHelper::random_int<int>(left, right) + _position.x;
		posExplode.y = RandomHelper::random_int<int>(low, height) + _position.y;

		this->scheduleOnce([posExplode](float delay) {

			SPAWN_EFFECT(Effect::EFFECT_TYPE::FIRE_EXPLODE_LARGE
				, posExplode.x, posExplode.y
				, 1
				, 1
				, RandomHelper::random_real<float>(0.5f, 1.5f)
				, World::WORLD_LAYER::EFFECT_FRONT);

		}, timer, str);
	}

	posExplode = _position;


	// schedule disappear
	this->scheduleOnce([posExplode, this](float delay) {

		SPAWN_EFFECT(Effect::EFFECT_TYPE::FIRE_EXPLODE_LARGE
			, posExplode.x, posExplode.y
			, 1
			, 1
			, 5.0f
			, World::WORLD_LAYER::EFFECT_FRONT);
	}, _dieTime, "DarkFXDie");

	this->scheduleOnce([posExplode, this](float delay) {
		//this->setActive(false);
		this->endGame();
	}, _dieTime + 0.3f, "DarkDie");
}

void DarkPortal::endGame() {
	this->scheduleOnce([this](float delay) {

		AUDIO::setVolume(ResourceManager::getInstance()->backgroundSongID, 0.6f);


		this->scheduleOnce([this](float delay) {

			AUDIO::setVolume(ResourceManager::getInstance()->backgroundSongID, 0.3f);

			// notify that "do not delete the res manager"
			ResourceManager::getInstance()->readyDelete = false;

			// Returns to Main Menu scene
			Director::getInstance()->replaceScene(TransitionFade::create(0.5f, MenuScene1::createScene()));

		}, 1.0f, "AfterDarkPortalDie");

	}, 1.0f, "AfterDarkPortalDie");
	
}