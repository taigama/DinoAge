#include "ResourceManager.h"

#include "MyPoolManager.h"// makeItem
#include "CameraHelper.h"
#include "WorldScene.h"

#include "Background.h"

#include "Block.h"
#include "Item.h"
#include "Gold.h"
#include "Zone.h"
#include "TriggerBlockZone.h"
#include "NextStageZone.h"

#include "EnemyApeDino.h"
#include "EnemyRamus.h"
#include "EnemyWerewolf.h"
#include "EnemyFeeder.h"

#include "AudioEngine.h"
#define AUDIO experimental::AudioEngine

USING_NS_CC;


#define FONT_SIZE 20.0f


#define STRING_PRE_ALLOCATE 64

ResourceManager* ResourceManager::instance = nullptr;

ResourceManager::ResourceManager()
{
	m_startLocation = Vec2::ZERO;
	m_map = nullptr;
	m_rect_map = Rect::ZERO;

	backgroundSongID = AUDIO::INVALID_AUDIO_ID;
	readyDelete = true;

	MyPoolManager::getInstance();

	
}

ResourceManager::~ResourceManager()
{
	instance = nullptr;

	unloadSprites();
	unloadTiledMap();

	// still in experiment
	AUDIO::uncacheAll();
	//AUDIO::end();

	delete MyPoolManager::getInstance();

	clearSpeechDatas();
}


ResourceManager* ResourceManager::getInstance()
{
	if (instance == nullptr)
	{
		instance = new ResourceManager();
	}

	return instance;	
}


void ResourceManager::loadSprites()
{
	auto spriteCache = SpriteFrameCache::getInstance();




	// load the blank sprite
	auto frm = SpriteFrame::create("blank.png", Rect(0, 0, 1, 1));
	spriteCache->addSpriteFrame(frm, "blank.png");
	frm = SpriteFrame::create("black.png", Rect(0, 0, 1, 1));
	spriteCache->addSpriteFrame(frm, "black.png");

	// block
	spriteCache->addSpriteFramesWithFile("moving_block.plist");


	// player
	spriteCache->addSpriteFramesWithFile("tyro.plist");
	spriteCache->addSpriteFramesWithFile("trino.plist");
	spriteCache->addSpriteFramesWithFile("yellow_dino.plist");

	// ENEMY
	spriteCache->addSpriteFramesWithFile("ape_dino.plist");
	spriteCache->addSpriteFramesWithFile("ramus.plist");
	spriteCache->addSpriteFramesWithFile("werewolf.plist");
	spriteCache->addSpriteFramesWithFile("feeder.plist");

	// UI
	spriteCache->addSpriteFramesWithFile("skills_icons.plist");
	spriteCache->addSpriteFramesWithFile("play_frames.plist");
	//spriteCache->addSpriteFramesWithFile("hud_icon.plist");
	spriteCache->addSpriteFramesWithFile("hud_icon1.plist");

	// ITEM
	spriteCache->addSpriteFramesWithFile("coins.plist");
	spriteCache->addSpriteFramesWithFile("heal_orb.plist");
	spriteCache->addSpriteFramesWithFile("gold.plist");

	// projectile & effect
	spriteCache->addSpriteFramesWithFile("fire.plist");
	spriteCache->addSpriteFramesWithFile("fire_ball.plist");
	spriteCache->addSpriteFramesWithFile("explode.plist");

	spriteCache->addSpriteFramesWithFile("twin_dragon01.plist");
	spriteCache->addSpriteFramesWithFile("twin_dragon02.plist");
	spriteCache->addSpriteFramesWithFile("twin_dragon03.plist");
	spriteCache->addSpriteFramesWithFile("twin_dragon04.plist");


	spriteCache->addSpriteFramesWithFile("ice.plist");
	spriteCache->addSpriteFramesWithFile("ice_blade.plist");

	spriteCache->addSpriteFramesWithFile("ice_blade1.plist");
	spriteCache->addSpriteFramesWithFile("ice_blade2.plist");
	spriteCache->addSpriteFramesWithFile("ice_blade3.plist");


	spriteCache->addSpriteFramesWithFile("blast.plist");
	spriteCache->addSpriteFramesWithFile("thunder.plist");
	spriteCache->addSpriteFramesWithFile("electric.plist");


	spriteCache->addSpriteFramesWithFile("bite.plist");
	spriteCache->addSpriteFramesWithFile("mistic.plist");
	spriteCache->addSpriteFramesWithFile("power_charge_effect.plist");
	spriteCache->addSpriteFramesWithFile("flash_ulti.plist");

	spriteCache->addSpriteFramesWithFile("black_in.plist");


	// preload effects
	MAKE_EFFECT(Effect::EFFECT_TYPE::BITE, 0, 0, 0, 1, 1);
}

void ResourceManager::unloadSprites()
{
	auto spriteCache = SpriteFrameCache::getInstance();
	spriteCache->removeSpriteFrames();
}

void ResourceManager::loadSounds()
{
	AUDIO::lazyInit();


	AUDIO::preload("Button.ogg");
	AUDIO::preload("challenge.ogg");
	//AUDIO::preload("player_select.ogg");
	AUDIO::preload("PointError.ogg");
	AUDIO::preload("ST0E_U1_00001.ogg");
	AUDIO::preload("ST01_00_00002.ogg");
	AUDIO::preload("roar.ogg");

#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)
	AUDIO::preload("the-only-friend.mp3");
	AUDIO::preload("digimon-rumble-arena_mid_stress.mp3");
#else// for android
	AUDIO::preload("the-only-friend.ogg");
	AUDIO::preload("digimon-rumble-arena_mid_stress.ogg");
#endif
}


bool ResourceManager::loadTiledMap(std::string name, Node* layer)
{
	if (m_map)
	{
		unloadTiledMap();
	}

	// =======================================================================================
	m_map = TMXTiledMap::create(name);

	if (m_map == nullptr)
	{
		MessageBox("Load map failed!", "ResourceManager::loadTiledMap()");
		return false;
	}

	layer->addChild(m_map, World::WORLD_LAYER::TILED_MAP);

	m_rect_map = m_map->getBoundingBox();



	// ----------------------------------------------
	//        --------- initialize -----------
	auto spawner = m_map->getObjectGroup("Init")->getObject("start_location");
	float x = spawner["x"].asInt();
	float y = spawner["y"].asInt();

	m_startLocation = Vec2(x, y);

	std::string background_path = spawner["bg"].asString();
	// background null
	//auto background = Sprite::create(background_path);
	auto background = Background::create();
	background->initLayer(background_path, "", "");
	layer->addChild(background, World::WORLD_LAYER::BACKGROUND);

	float margin = spawner["margin"].asFloat();
	layer->scheduleOnce(
		[margin](float delta)
		{
			CameraHelper::getInstance()->setMargin(margin);
		}, 0.2f, "set_margin");



	//         ------------------------------
	// -------------------------------------------------


	

	
	loadZones(layer);
	loadBlocks(layer);

	



	



	///////////////////////////////////
	// LOAD ENEMY (EnemyApeDino) TEST
	//obGroup = m_map->getObjectGroup("Enemies");
	//if (obGroup == NULL)
	//{
	//	log("NO Enemy object group");
	//	return false;
	//}
	//objects = obGroup->getObjects();
	//valueMap = objects.at(0).asValueMap();
	//x = valueMap["x"].asFloat();
	//y = valueMap["y"].asFloat();

	//auto end = Vec2(m_rect_map.getMinX() + x, m_rect_map.getMinY() + y);

	//valueMap = objects.at(1).asValueMap();
	//x = valueMap["x"].asFloat();
	//y = valueMap["y"].asFloat();

	//auto start = Vec2(m_rect_map.getMinX() + x, m_rect_map.getMinY() + y);

	//auto enemy = dynamic_cast<EnemyApeDino*>(Character::createCharacter(Character::CHARACTER_TYPE::ENEMY_APE_DINO, 2));

	//enemy->setPosition(start);
	//enemy->setPatrolArea(start, end);
	//enemy->patrol();

	//layer->addChild(enemy, (int)World::WORLD_LAYER::OBJECT);



	//// EnemyRamus TEST
	//valueMap = objects.at(3).asValueMap();
	//x = valueMap["x"].asFloat();
	//y = valueMap["y"].asFloat();

	//start = Vec2(m_rect_map.getMinX() + x, m_rect_map.getMinY() + y);

	//valueMap = objects.at(2).asValueMap();
	//x = valueMap["x"].asFloat();
	//y = valueMap["y"].asFloat();

	//end = Vec2(m_rect_map.getMinX() + x, m_rect_map.getMinY() + y);

	//auto enemyRamus = dynamic_cast<EnemyRamus*>(Character::createCharacter(Character::CHARACTER_TYPE::ENEMY_RAMUS, 2));

	//enemyRamus->setPosition(start);
	//enemyRamus->setPatrolArea(start, end);
	//enemyRamus->attackPhysical();

	//layer->addChild(enemyRamus, (int)World::WORLD_LAYER::OBJECT);


	//// EnemyWerewolf TEST
	//valueMap = objects.at(4).asValueMap();
	//x = valueMap["x"].asFloat();
	//y = valueMap["y"].asFloat();

	//start = Vec2(m_rect_map.getMinX() + x, m_rect_map.getMinY() + y);

	//auto enemyWerewolf = dynamic_cast<EnemyWerewolf*>(Character::createCharacter(Character::CHARACTER_TYPE::ENEMY_WEREWOLF, 2));

	//enemyWerewolf->setPivotPosition(start, Character::DIRECTION::LEFT);
	////enemyWerewolf->move(Character::DIRECTION::RIGHT);

	//layer->addChild(enemyWerewolf, (int)World::WORLD_LAYER::OBJECT);


	//// EnemyFeeder TEST
	//valueMap = objects.at(5).asValueMap();
	//x = valueMap["x"].asFloat();
	//y = valueMap["y"].asFloat();

	//start = Vec2(m_rect_map.getMinX() + x, m_rect_map.getMinY() + y);

	//auto enemyFeeder = dynamic_cast<EnemyFeeder*>(Character::createCharacter(Character::CHARACTER_TYPE::ENEMY_FEEDER, 2));

	//enemyFeeder->setPosition(Vec2(x, y));
	//enemyFeeder->setInitialDirection(Character::DIRECTION::RIGHT);
	//enemyFeeder->setEnemyFeederSize(EnemyFeeder::FEEDER_SIZE::SMALL);
	//enemyFeeder->setEnemyFeederType(EnemyFeeder::FEEDER_TYPE::STATIONARY);

	//layer->addChild(enemyFeeder, (int)World::WORLD_LAYER::OBJECT);

	//enemyFeeder->startAction();

	loadEnemies();


	////////////////////////////////////////////
	// LOAD ITEMS

	loadItems();

	loadIllusions(layer);

	loadText(layer);

	//////////////////////////////////////////////////////
	//  POLYGONS LOADABLE

	// need fixed
//
//	obGroup = m_map->getObjectGroup("ground_polygon");
//	if (obGroup == NULL)
//	{
//		MessageBox("Object group NULL!", "ERROR!");
//		return false;
//	}
//	objects = obGroup->getObjects();
//
//	Vec2* vecPoints;
//	int count;
//
//	//auto iterator = objects.begin();
//	for (auto iterator = objects.begin(); iterator != objects.end(); iterator++)
//	{
//		vecPoints = new Vec2[30];
//		count = 0;
//
//		valueMap = iterator->asValueMap();
//
//		x = valueMap["x"].asFloat();
//		y = valueMap["y"].asFloat();
//
//		
//		// load point in polygons
////		ValueVector points = valueMap.begin()->second.asValueVector();
//		ValueVector points = valueMap["points"].asValueVector();
//		for (auto it = points.begin(); it != points.end(); it++)
//		{
//			valueMap = it->asValueMap();
//
//			vecPoints[count].x = valueMap["x"].asFloat();
//			vecPoints[count].y = valueMap["y"].asFloat();
//
//
//			// invert y point
//			vecPoints[count].y = y + (y - vecPoints[count].y);
//
//
//
//
//
//
//
//
//
//
//			count++;
//		}
//
//
//
//
//
//		tmpBlk = Block::createPoly(x, y, vecPoints, count);
//
//		layer->addChild(tmpBlk);
//	}
	// -----------------------------------



#if ((CC_TARGET_PLATFORM == CC_PLATFORM_WIN32) || (CC_TARGET_PLATFORM == CC_PLATFORM_LINUX))
	backgroundSongID = AUDIO::play2d("the-only-friend.mp3", true);
#else
	backgroundSongID = AUDIO::play2d("the-only-friend.ogg", true);
#endif


	

	return true;
}

	// ------- load ZONE ------------
void ResourceManager::loadZones(Node* layer)
{
	auto obGroup = m_map->getObjectGroup("Zones");
	if (obGroup == NULL)
	{
		MessageBox("Object group NULL!", "ERROR!");
		return;
	}

	ValueMap valueMap;
	float x, y// position
		, width, height;// size
	
		// special keys
	int sID, direction
		// because some fucking keyword take the name "value"
		, i_value, end_index, start_index
		, x1, y1, x2, y2;

	Zone* tmpZone = nullptr;
	Zone::ZONE_TYPE zoneType;
	

	auto objects = obGroup->getObjects();

	for (auto iterator = objects.begin(); iterator != objects.end(); iterator++)
	{
		valueMap = iterator->asValueMap();

		x = valueMap["x"].asFloat();
		y = valueMap["y"].asFloat();
		width = valueMap["width"].asInt();
		height = valueMap["height"].asInt();

		zoneType = (Zone::ZONE_TYPE) valueMap["zone_type"].asInt();

		x1 = valueMap["x1"].asInt();
		y1 = valueMap["y1"].asInt();
		i_value = valueMap["value"].asInt();
		end_index = valueMap["end_index"].asInt();
		start_index = valueMap["start_index"].asInt();
		sID = valueMap["zone_ID"].asInt();
		direction = valueMap["direction"].asInt();

		tmpZone = Zone::createZone(zoneType, x + width / 2, y + height / 2, width, height, x1, y1, start_index, end_index, i_value, sID, direction);

		// don't care about Z order because we can't see them :v
		layer->addChild(tmpZone);
		//((World*)layer)->zones.pushBack(tmpZone);

		if (zoneType == (int)Zone::ZONE_TYPE::NEXTSTAGE)
		{

			tmpZone->message(valueMap["next_map"].asString());
			((NextStageZone*)tmpZone)->setLevelTitle(valueMap["level_title"].asString());
			((NextStageZone*)tmpZone)->setStageTitle(valueMap["stage_title"].asString());
		}

		// add to list zone triggerer
		if (sID != 0)
		{
			((World*)layer)->zones.insert(sID, tmpZone);
		}
	}
}

	// ----- load BLOCK ----------
void ResourceManager::loadBlocks(Node* layer)
{
	auto obGroup = m_map->getObjectGroup("Blocks");
	if (obGroup == NULL)
	{
		MessageBox("Object group NULL!", "ERROR!");
		return;
	}

	ValueMap valueMap;
	float x, y// position
		, width, height;// size

						// special keys
	int zone_id // tagged zone id
		, x1, y1, x2, y2;

	Block* tmpBlk = nullptr;
	Block::BLOCK_TYPE blockType;

	auto objects = obGroup->getObjects();

	bool isHidden, isOneWay;



	//auto iterator = objects.begin();
	for (auto iterator = objects.begin(); iterator != objects.end(); iterator++)
	{
		valueMap = iterator->asValueMap();

		x = valueMap["x"].asFloat();
		y = valueMap["y"].asFloat();
		width = valueMap["width"].asFloat();
		height = valueMap["height"].asFloat();

		isHidden = valueMap["isHidden"].asBool();
		isOneWay = valueMap["isOneWay"].asBool();

		// special parameter
		x1 = valueMap["x1"].asFloat();
		y1 = valueMap["y1"].asFloat();
		x2 = valueMap["x2"].asFloat();
		y2 = valueMap["y2"].asFloat();

		zone_id = valueMap["zone_ID"].asInt();

		blockType = (Block::BLOCK_TYPE) valueMap["block_type"].asInt();

		tmpBlk = Block::createBlock(
			blockType
			, x + width / 2, y + height / 2
			, width, height
			, isHidden, isOneWay
			, x1, y1
			, x2, y2
			, zone_id
			);

		layer->addChild(tmpBlk, (int)World::WORLD_LAYER::GROUND);
	}
}

void ResourceManager::loadItems()
{
	//return;

	// Retrieves object group for Items
	auto itemGroup = m_map->getObjectGroup("Items");
	
	CCASSERT(itemGroup != nullptr, "Items object group NOT FOUND");


	// Needed variables
	int type;
	float x, y;
	ValueMap valueMap;

	// Iterates through each object
	auto itemObject = itemGroup->getObjects();
	for (auto iter = itemObject.cbegin(); iter != itemObject.cend(); ++iter)
	{
		// Extracts the current ValueMap
		valueMap = (*iter).asValueMap();

		// Gets the Item's type and location
		type = valueMap["item_type"].asInt();
		x = valueMap["x"].asFloat();
		y = valueMap["y"].asFloat();

		// Spawn the respective Item as the specified location
		//SPAWN_ITEM( ((Item::ITEM_TYPE)type), x, y, World::WORLD_LAYER::OBJECT);
		World::getCurrent()->addChild(MyPoolManager::getInstance()->makeItem((Item::ITEM_TYPE)type, x, y), World::WORLD_LAYER::OBJECT);
	}
}

void ResourceManager::loadEnemies()
{
	//return;

	// Retrieves object group for Enemies
	auto enemyGroup = m_map->getObjectGroup("Enemies");

	CCASSERT(enemyGroup != nullptr, "Enemies object group NOT FOUND");


	// Needed variables
	int type, zoneID;
	float delayTimeSpawn;
	Vec2 startPosition, endPosition;		// start-end positions for EnemyRamus + EnemyApeDino
	Character::DIRECTION direction;			// initial direction for EnemyFeeder + EnemyWerewolf
	ValueMap valueMap;
	Enemy* enemy;

	// Iterates through each object
	auto enemyObject = enemyGroup->getObjects();
	for (auto iter = enemyObject.cbegin(); iter != enemyObject.cend(); ++iter)
	{
		// Extracts the current ValueMap
		valueMap = (*iter).asValueMap();

		// Gets Enemy's type, initial position and direction, delay time to be spawned and the belonged Zone's ID
		type = valueMap["character_type"].asInt();

		startPosition.x = valueMap["x"].asFloat();
		startPosition.y = valueMap["y"].asFloat();
		endPosition.x = valueMap["endX"].asFloat();

		auto mapHeight = m_map->getMapSize().height * m_map->getTileSize().height;
		endPosition.y = mapHeight - valueMap["endY"].asFloat();

		direction = (Character::DIRECTION)valueMap["direction"].asInt();
		delayTimeSpawn = valueMap["delayTimeSpawn"].asFloat();
		zoneID = valueMap["zone_ID"].asInt();

		// Creates the respective Enemy
		enemy = dynamic_cast<Enemy*>(Character::createCharacter((Character::CHARACTER_TYPE)type, 2));
		// ENEMY FEEDER
		if ((Character::CHARACTER_TYPE)type == Character::CHARACTER_TYPE::ENEMY_FEEDER)
		{
			auto enemyFeeder = dynamic_cast<EnemyFeeder*>(enemy);

			enemyFeeder->setPosition(startPosition);
			enemyFeeder->setInitialDirection(direction);

			// Sets type and size for EnemyFeeder
			enemyFeeder->setEnemyFeederType((EnemyFeeder::FEEDER_TYPE)valueMap["feederType"].asInt());
			enemyFeeder->setEnemyFeederSize((EnemyFeeder::FEEDER_SIZE)valueMap["feederSize"].asInt());
		}

		// ENEMY RAMUS
		else if ((Character::CHARACTER_TYPE)type == Character::CHARACTER_TYPE::ENEMY_RAMUS)
		{
			auto enemyRamus = dynamic_cast<EnemyRamus*>(enemy);

			enemyRamus->setPosition(startPosition);

			// Sets two endpoints for EnemyRamus's patrol area
			enemyRamus->setPatrolArea(startPosition, endPosition);
		}

		// ENEMY APE DINO
		else if ((Character::CHARACTER_TYPE)type == Character::CHARACTER_TYPE::ENEMY_APE_DINO)
		{
			auto enemyApeDino = dynamic_cast<EnemyApeDino*>(enemy);

			enemyApeDino->setPosition(startPosition);

			// Sets two endpoints for EnemyApeDino's patrol area
			enemyApeDino->setPatrolArea(startPosition, endPosition);
		}

		// ENEMY WEREWOLF
		else if ((Character::CHARACTER_TYPE)type == Character::CHARACTER_TYPE::ENEMY_WEREWOLF)
		{
			auto enemyWerewolf = dynamic_cast<EnemyWerewolf*>(enemy);

			enemyWerewolf->setPivotPosition(startPosition, direction);
		}

		// DARK PORTAL

		else if ((Character::CHARACTER_TYPE)type == Character::CHARACTER_TYPE::DARK_PORTAL)
		{
			enemy->setPosition(startPosition);
		}
		else
		{
			CCLOG("INVALID ENEMY TYPE");
		}

		// Retrieves the corresponding Zone -> stored in Enemy
		auto zone = dynamic_cast<TriggerBlockZone*>(World::getCurrent()->zones.at(zoneID));
		enemy->setZone(zone);

		zone->addCreep(enemy, delayTimeSpawn);	// adds Enemy and its delay-time-spawn into the Zone
	}
}

// ----- load map sprite at the front ----------
void ResourceManager::loadIllusions(Node* layer)
{
	auto obGroup = m_map->getObjectGroup("Illusion");
	if (obGroup == NULL)
	{
		MessageBox("Object group NULL!", "ResourceManager::loadMapSprites()");
		return;
	}

	ValueMap valueMap;
	float x, y;				// position
	float width, height;	// size

	std::string path;
	// pre allocate
	path.reserve(STRING_PRE_ALLOCATE);
	Sprite* tmpSpr = nullptr;

	auto objects = obGroup->getObjects();



	//auto iterator = objects.begin();
	for (auto iterator = objects.begin(); iterator != objects.end(); iterator++)
	{
		valueMap = iterator->asValueMap();

		x = valueMap["x"].asFloat();
		y = valueMap["y"].asFloat();

		width = valueMap["width"].asFloat();
		height = valueMap["height"].asFloat();

		path = valueMap["path"].asString();

		tmpSpr = Sprite::create(path);
		if (tmpSpr == nullptr)
		{
			MessageBox("Path not found!", "ResourceManager::loadSprite()");
			continue;
		}

		//tmpSpr->setAnchorPoint(Vec2::ZERO);
		//tmpSpr->setPosition(x, y);

		tmpSpr->setAnchorPoint(Vec2(0.5, 0.5));
		tmpSpr->setPosition(x + width / 2, y + height / 2);

		tmpSpr->setScaleX(width * 1.15f / tmpSpr->getBoundingBox().size.width);
		tmpSpr->setScaleY(height * 1.15f / tmpSpr->getBoundingBox().size.height);

		layer->addChild(tmpSpr, (int)World::WORLD_LAYER::EFFECT_FRONT);
	}
}

// ----- load map sprite at the front ----------
void ResourceManager::loadText(Node* layer)
{
	auto obGroup = m_map->getObjectGroup("TutorialText");
	if (obGroup == NULL)
	{
		//MessageBox("Object group NULL!", "ResourceManager::loadMapSprites()");
		CCLOG("TutorialText not found!");
		return;
	}

	ValueMap valueMap;
	float x, y, width, height;// position
	//int size;

	std::string txt;
	// pre allocate
	txt.reserve(STRING_PRE_ALLOCATE);
	Label* tmpLb = nullptr;

	auto objects = obGroup->getObjects();



	//auto iterator = objects.begin();
	for (auto iterator = objects.begin(); iterator != objects.end(); iterator++)
	{
		valueMap = iterator->asValueMap();

		x = valueMap["x"].asFloat();
		y = valueMap["y"].asFloat();
		width = valueMap["width"].asFloat();
		height = valueMap["height"].asFloat();
		txt = valueMap["name"].asString();
		//size = valueMap["size"].asInt();

		tmpLb = Label::createWithTTF(txt, "verdana.ttf", FONT_SIZE);
		if (tmpLb == nullptr)
		{
			MessageBox("Error create label!", "ResourceManager::loadText()");
			continue;
		}

		tmpLb->setContentSize(Size(width, height));
		tmpLb->setMaxLineWidth(width);
		tmpLb->enableWrap(true);
		tmpLb->setColor(Color3B(0, 0, 0));

		//tmpLb->setAnchorPoint(Vec2::ZERO);
		//tmpLb->setPosition(x, y);

		tmpLb->setAnchorPoint(Vec2(0.5, 0.5));
		tmpLb->setPosition(x + width / 2, y + height / 2);

		tmpLb->setHorizontalAlignment(TextHAlignment::CENTER);
		tmpLb->setVerticalAlignment(TextVAlignment::CENTER);


		layer->addChild(tmpLb, (int)World::WORLD_LAYER::EFFECT_FRONT);
	}
}

bool ResourceManager::unloadTiledMap()
{
	if (m_map)
	{
		m_map->removeFromParent();
		m_map = nullptr;
	}

	if (backgroundSongID != AUDIO::INVALID_AUDIO_ID)
	{
		//AUDIO::stop(backgroundSongID);
		AUDIO::stopAll();
		backgroundSongID = AUDIO::INVALID_AUDIO_ID;
	}


	return true;
}

TMXTiledMap* ResourceManager::getMap()
{
	return m_map;
}

Rect ResourceManager::getArea()
{
	return m_rect_map;
}

void ResourceManager::loadSpeechDatas(std::string fileName)
{
	if (isSpeechDatasContainKey(fileName))
		return;

	std::vector<std::string>* lineDatas = new std::vector<std::string>();
	
	Data data = FileUtils::getInstance()->getDataFromFile(fileName);
	char* pBuffer = (char*)data.getBytes();
	std::stringstream ss(pBuffer);
	std::string line;
	while (std::getline(ss, line, '\n'))
	{
		lineDatas->push_back(line);
	}


	parseSpeechDatas(lineDatas, fileName);
	
	delete lineDatas;
}

void ResourceManager::parseSpeechDatas(std::vector<std::string>* lineDatas, std::string& fileName)
{
	if (lineDatas->size() == 0)
		return;

	std::vector<SpeechModel*>* result = new std::vector<SpeechModel*>();

	for (auto iter = lineDatas->begin(); iter != lineDatas->end(); iter++)
	{
		result->push_back(new SpeechModel(*iter));
	}

	speechDatas[fileName] = result;
}

bool ResourceManager::isSpeechDatasContainKey(std::string key)
{
	if (speechDatas.size() == 0)
		return false;
	for (auto iter = speechDatas.begin(); iter != speechDatas.end(); iter++)
	{
		if (iter->first == key)
			return true;
	}
	return false;
}

void ResourceManager::clearSpeechDatas()
{
	if (speechDatas.size() == 0)
		return;

	std::vector<SpeechModel*>* tmp;

	for (auto iter = speechDatas.begin(); iter != speechDatas.end(); iter++)
	{
		tmp = iter->second;
		if (tmp->size() != 0)
		{
			for (auto i = tmp->begin(); i != tmp->end(); i++)
			{
				delete (*i);
			}
			delete tmp;
		}
	}
	speechDatas.clear();
}