#include "TriggerBlockZone.h"
#include "WorldScene.h"

#include "MyPoolManager.h"
#include "WorldScene.h"
#include "ResourceManager.h"
#include "MovingBlock.h"
#include "Enemy.h"

#define TIME_TELEPORT 1.0f

USING_NS_CC;


TriggerBlockZone::~TriggerBlockZone() { m_creeps.clear(); }

bool TriggerBlockZone::init()
{
	if (!Zone::init())
		return false;

	_zoneType == ZONE_TYPE::TRIGGERBLOCK;

	m_creepCount = 0;
	m_creepAlive = 0;

	return true;
}
// when player enter the zone
void TriggerBlockZone::onPlayerEntered(Character* target)
{
	if (m_creepCount == 0)
	{
		if (_blockTrigger)
		{
			((MovingBlock*)_blockTrigger)->triggered();
		}
	}
	else
	{

		if (m_creepAlive == 0)
		{
			Enemy* myCreep;
			char str[100];

			for (auto iter = m_creeps.begin(); iter != m_creeps.end(); iter++)
			{
				int index = m_creeps.getIndex(*iter);
				myCreep = (Enemy*)(*iter);


				sprintf(str, "%d", index);

				this->scheduleOnce([myCreep](float delta) {

					// Adds Enemy into World
					World::getCurrent()->addChild(myCreep, World::WORLD_LAYER::OBJECT);
					myCreep->setActive(true);

					// Starts Enemy's action
					myCreep->startAction();

				}, m_delayTimeSpawn.at(index), str);



				// add BAAANG sound effect // :v


			}
			m_creepAlive = m_creepCount;
		}
	}

}

//void TriggerBlockZone::init(float x1, float y1, int start_index, int end_index, int i_value, int ID)
//{
//	_ID = ID;
//}


void TriggerBlockZone::addCreep(GameObject* creep, float time)
{
	if (creep->getType() == GameObject::OBJECT_TYPE::CHARACTER)
	{
		if (m_creeps.find(creep) == m_creeps.end())
		{
			m_creeps.pushBack(creep);
			m_delayTimeSpawn.push_back(time);
			m_creepCount++;

			((Character*)creep)->setZoneContainer(this);
			creep->setActive(false);
		}
		else
		{
			MessageBox("Dupplicate add creep!", "TriggerBlockZone::addCreep");
		}
	}
	else
	{
		MessageBox("You must add a CHARACTER!", "TriggerBlockZone::addCreep");
	}
}

// enemy->zoneHolder->
void TriggerBlockZone::notifyCreepDie(GameObject* creep)
{
	// posibility dupplicate
	m_creepAlive--;

	// Removes creep from the list
	auto delayTimeIterator = m_delayTimeSpawn.cbegin() + m_creeps.getIndex(creep);

	m_delayTimeSpawn.erase(delayTimeIterator);
	m_creeps.eraseObject(creep, false);

	// if there a no creep alive -> trigger something
	if (m_creepAlive == 0)
	{
		if (_blockTrigger)
		{
			((MovingBlock*)_blockTrigger)->triggered();
		}

		m_creepAlive = -1;			// avoid re-spawning Enemy
	}
}