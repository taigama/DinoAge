#include "WorldScene.h"
#include "RedDino.h"
//#include "Block.h"
#include "ResourceManager.h"
#include "MyPoolManager.h"
#include "CameraHelper.h"

#include "CharacterSelectionScene.h"

USING_NS_CC;


//World* World::m_instance = nullptr;
World* World::m_current = nullptr;

// #############################################################################################
// FIXED RANDOM BOUNCE - Khang: "I don't know why? Why?"
// https://stackoverflow.com/questions/42712753/cocos2dx-zero-restitution-does-not-stop-bouncing

#define FLT_EPSILON 1.19209290e-07F
float _filteredUpdateDelta;


void World::update(float delta)
{
	const int phyiscsSubSteps = 3;
	float dt = delta / static_cast<float>(phyiscsSubSteps);
	_filteredUpdateDelta = dt > FLT_EPSILON ? 0.15 * dt + 0.85 * _filteredUpdateDelta : 0.0;
	for (int i = phyiscsSubSteps; i > 0; i--)
	{
		this->getScene()->getPhysicsWorld()->step(_filteredUpdateDelta);
	}

	//this->getScene()->getPhysicsWorld()->step(delta);
// ##########################################################################################################


}

// not using
//Scene* World::createScene()
//{
//    auto scene = Scene::createWithPhysics();    
//
//    auto layer = World::create();
//    scene->addChild(layer);
//
//    
//
//	scene->getPhysicsWorld()->setDebugDrawMask(PhysicsWorld::DEBUGDRAW_ALL);
//	scene->getPhysicsWorld()->setGravity(Vec2(0, -500));
//
//	scene->getPhysicsWorld()->setAutoStep(false);
//
//    return scene;
//}

Layer* World::createLayer()
{
	auto world = World::create();
	// create ra null la bug lien
	world->setName("world_layer");

	return world;
}

Layer* World::createLayer(std::string mapName)
{
	World* world = new(std::nothrow) World();

	if (world && world->init(mapName))
	{
		world->autorelease();
		world->setName("world_layer");
		return world;
	}
	else
	{
		delete world;
		world = nullptr;
		return world;
	}
}

World::World()
{
	if (m_current)
	{
		MessageBox("Already has current World!", "World constructor");
	}

	m_current = this;
}

// on "init" you need to initialize your instance
bool World::init()
{// obsolete ###########################################################
	MessageBox("Obsolete World init!", "");
    //////////////////////////////
    // 1. super init first
    if ( !Layer::init() )
    {
        return false;
    }

	//m_instance = this;
    
    Size visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();
	this->scheduleUpdate();


	//////////////////////////////////////
	// 2. res Loading

	auto resManager = ResourceManager::getInstance();
	resManager->loadSprites();
	resManager->loadTiledMap("prologue_tutorial.tmx", this);




	//////////////////////////////////
	// 3. dinos

	// NOTE: SET PLAYER LATER

	//m_mainPlayer = (Player*) Character::createCharacter(Character::CHARACTER_TYPE::WHITE_DINOSAUR, 1);
	//m_mainPlayer->setPosition(resManager->m_startLocation);
	//this->addChild(m_mainPlayer, (int)WORLD_LAYER::OBJECT);
	//this->runAction(Follow::create(m_mainPlayer, resManager->getArea()));
	//Camera::getDefaultCamera()->runAction(Follow::create(m_mainPlayer, resManager->getArea()));
	//m_mainPlayer->setScale(2);











	/////////////////////////////////
	// 4. world contact listener
	addContactListener();




    return true;
}



// on "init" you need to initialize your instance
bool World::init(std::string mapName)
{
	//////////////////////////////
	// 1. super init first
	if (!Layer::init())
	{
		return false;
	}

	Size visibleSize = Director::getInstance()->getVisibleSize();
	Vec2 origin = Director::getInstance()->getVisibleOrigin();
	this->scheduleUpdate();


	//////////////////////////////////////
	// 2. res Loading

	auto resManager = ResourceManager::getInstance();
	resManager->loadSprites();
	resManager->loadTiledMap(mapName, this);




	//////////////////////////////////
	// 3. dinos

	// NOTE: SET PLAYER LATER

	//m_mainPlayer = (Player*) Character::createCharacter(Character::CHARACTER_TYPE::WHITE_DINOSAUR, 1);
	//m_mainPlayer->setPosition(resManager->m_startLocation);
	//this->addChild(m_mainPlayer, (int)WORLD_LAYER::OBJECT);
	//this->runAction(Follow::create(m_mainPlayer, resManager->getArea()));
	//Camera::getDefaultCamera()->runAction(Follow::create(m_mainPlayer, resManager->getArea()));
	//m_mainPlayer->setScale(2);











	/////////////////////////////////
	// 4. world contact listener
	addContactListener();




	return true;
}

void World::setPlayer(CharacterSelectionScene::SELECTION selection)
{
	// Retrieves resource manager -> get initial position for Player
	auto resManager = ResourceManager::getInstance();

	switch (selection)
	{
	case CharacterSelectionScene::SELECTION::RED_DINO:
		m_mainPlayer = (Player*)Character::createCharacter(Character::CHARACTER_TYPE::RED_DINOSAUR, 1);		// create character
		m_mainPlayer->setPosition(resManager->m_startLocation);	// set to starting position
		break;
	case CharacterSelectionScene::SELECTION::WHITE_DINO:
		m_mainPlayer = (Player*)Character::createCharacter(Character::CHARACTER_TYPE::WHITE_DINOSAUR, 1);	// create character
		m_mainPlayer->setPosition(resManager->m_startLocation);	// set to starting position
		break;
	case CharacterSelectionScene::SELECTION::YELLOW_DINO:
		m_mainPlayer = (Player*)Character::createCharacter(Character::CHARACTER_TYPE::YELLOW_DINOSAUR, 1);	// create character
		m_mainPlayer->setPosition(resManager->m_startLocation);	// set to starting position
		break;
	default:
		log("ERROR: INVALID SELECTION");
		break;
	}

	// Adds to World
	this->addChild(m_mainPlayer, (int)WORLD_LAYER::PLAYER);
	m_mainPlayer->retain();
}

void World::exitGame(Ref* pSender)
{
    Director::getInstance()->end();

#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    exit(0);
#endif
}

Player* World::getPlayer()
{
	return m_mainPlayer;
}


World* World::getCurrent()
{
	if (m_current == nullptr)
	{
		//CCLOG("Current world null!", "World::getCurrent()");
	}

	return m_current;
}

void World::setCurrent(World* newCurrent)
{
	m_current = newCurrent;
}

// Physics
void World::addContactListener()
{
	auto contactListener = EventListenerPhysicsContact::create();
	contactListener->onContactBegin = CC_CALLBACK_1(World::onContactBegin, this);
	contactListener->onContactPreSolve = CC_CALLBACK_1(World::onContactPresolve, this);
	contactListener->onContactSeparate = CC_CALLBACK_1(World::onContactSeparate, this);

	this->_eventDispatcher->addEventListenerWithSceneGraphPriority(contactListener, this);
}
bool World::onContactBegin(const cocos2d::PhysicsContact &contact)
{
	// requested by Thinh
	if (contact.getShapeA()->getBody()->getCategoryBitmask() == GameObject::OBJECT_TYPE::CHARACTER)
	{		
		m_check2 = ((GameObject*)contact.getShapeB()->getBody()->getOwner())->onContactBegin(contact);
		m_check1 = ((GameObject*)contact.getShapeA()->getBody()->getOwner())->onContactBegin(contact);
	}
	else
	{
		m_check1 = ((GameObject*)contact.getShapeA()->getBody()->getOwner())->onContactBegin(contact);
		m_check2 = ((GameObject*)contact.getShapeB()->getBody()->getOwner())->onContactBegin(contact);
	}


	return m_check1 && m_check2;
}
bool World::onContactPresolve(cocos2d::PhysicsContact &contact)
{
	m_check1 = ((GameObject*)contact.getShapeA()->getBody()->getOwner())->onContactPresolve(contact);
	m_check2 = ((GameObject*)contact.getShapeB()->getBody()->getOwner())->onContactPresolve(contact);

	return m_check1 && m_check2;
}

bool World::onContactSeparate(const cocos2d::PhysicsContact &contact)
{
	m_check1 = ((GameObject*)contact.getShapeA()->getBody()->getOwner())->onContactSeparate(contact);
	m_check2 = ((GameObject*)contact.getShapeB()->getBody()->getOwner())->onContactSeparate(contact);

	return m_check1 && m_check2;
}

World::~World()
{
	zones.clear();

	//delete ResourceManager::getInstance();
	ResourceManager::getInstance()->unloadTiledMap();

	m_mainPlayer->release();
	//delete MyPoolManager::getInstance();
	delete CameraHelper::getInstance();
}