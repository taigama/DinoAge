#include "Zone.h"

#include "DeathZone.h"
#include "Teleporter.h"
#include "TriggerBlockZone.h"
#include "NextStageZone.h"

#include "SpeechZone.h"
USING_NS_CC;


Zone::~Zone() {}

#define TEAM_PLAYER 1

bool Zone::init()
{
	if (!GameObject::init())
		return false;


	return true;
}

void Zone::update(float delta)
{

}

// Returns a Zone object with the given size and position
// * type : enum ZONE_TYPE ; type of the Zone object
// * x : position x
// * y : position y
// * width : width of zone
// * height : height of zone
// #### based on type of zone ####
// * x1 : the trigger X (may be teleport location)
// * y1 : the trigger Y
// * value : dmg of death_zone, type of item to be spawned
// * end_index, start_index: for the zone that hold many enemy, index is for binding enemies
// * ID is for binding
Zone* Zone::createZone(ZONE_TYPE type
	, float x, float y, float width, float heigh
	, float x1, float y1
	, int start_index, int end_index
	, int i_value, int ID, int direction)
{
	Zone* result = nullptr;
	switch (type)
	{
	case ZONE_TYPE::TELEPORTER:
	{
		result = Teleporter::create();
	}
		break;
	case ZONE_TYPE::DEATH:
	{
		result = DeathZone::create();
	}
		break;
	case ZONE_TYPE::TRIGGERBLOCK:
	{
		result = TriggerBlockZone::create();
	}
	break;
	case ZONE_TYPE::NEXTSTAGE:
	{
		result = NextStageZone::create();
	}
	break;
	case ZONE_TYPE::SPEECH:
	{
		result = SpeechZone::create();
	}
	default:
		break;
	}

	if (result)
	{
		result->setPosition(Vec2(x, y));

		// add physics to result
		result->_body = PhysicsBody::createBox(Size(width, heigh), PhysicsMaterial(1, 0, 0));
		result->_body->setDynamic(false);

		result->setColliderBitMask();

		result->setPhysicsBody(result->_body);

		result->init(x1, y1, start_index, end_index, i_value, ID, direction);
	}
	else
	{
		CCLOG("create Zone failed! Zone type: %d", (int)type);
		return Zone::create();
	}


	return result;
}

void Zone::init(float x1, float y1
	, int start_index, int end_index
	, int i_value, int ID, int direction)
{
	_ID = ID;
}

// used by createZone
void Zone::setColliderBitMask()
{
	if (_body == nullptr)
		return;

	_body->setCategoryBitmask((int)OBJECT_TYPE::ZONE);
	_body->setCollisionBitmask(0);// no need to collide anything (ex: prevent player moving)
	_body->setContactTestBitmask(
		(int)OBJECT_TYPE::CHARACTER
	);
}

// auto callback
bool Zone::onContactBegin(const cocos2d::PhysicsContact &contact)
{
	PhysicsBody* target = contact.getShapeA()->getBody();
	if (_body == target)// target la b
	{
		target = contact.getShapeB()->getBody();
	}

	// doi tuong va cham la nguoi choi
	if (target->getCategoryBitmask() == (int)GameObject::CHARACTER)
	{
		Character* character = (Character*)target->getOwner();
		///////
		//character->setPosition(character->getPositionX() - 1000, character->getPositionY());

		if (character->getTeam() == TEAM_PLAYER)// team nguoi choi
			onPlayerEntered(character);
	}


	return false;
}

//// auto callback
//bool Zone::onContactPresolve(const cocos2d::PhysicsContact &contact)
//{
//	PhysicsBody* target = contact.getShapeA()->getBody();
//	if (_body == target)// target la b
//	{
//		target = contact.getShapeB()->getBody();
//	}
//
//	// doi tuong va cham la nguoi choi
//	if (target->getCategoryBitmask() == (int)GameObject::CHARACTER)
//	{
//		Character* character = (Character*)target->getOwner();
//		if (character->getTeam() == TEAM_PLAYER)// team nguoi choi
//			onPlayerKeepInside(character);
//	}
//
//
//	return true;
//}

// auto callback
bool Zone::onContactSeparate(const cocos2d::PhysicsContact &contact)
{
	PhysicsBody* target = contact.getShapeA()->getBody();
	if (_body == target)// target la b
	{
		target = contact.getShapeB()->getBody();
	}

	// doi tuong va cham la nguoi choi
	if (target->getCategoryBitmask() == (int)GameObject::CHARACTER)
	{
		Character* character = (Character*)target->getOwner();
		if (character->getTeam() == TEAM_PLAYER)// team nguoi choi
			onPlayerExit(character);
	}


	return true;
}

// when player enter the zone
void Zone::onPlayerEntered(Character* target) {}
//// when player keeping inside the zone
//void Zone::onPlayerKeepInside(Character* target) {}
// when player exit the zone
void Zone::onPlayerExit(Character* target) {}

