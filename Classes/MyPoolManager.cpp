#include "MyPoolManager.h"


USING_NS_CC;

MyPoolManager* MyPoolManager::m_instance = nullptr;

MyPoolManager::MyPoolManager()
{
	m_instance = this;
}

MyPoolManager::~MyPoolManager()
{
	// may being leak memory
	if (m_instance)
		m_instance = nullptr;	
	// but without the above function, may be get memory access violent

	for (int i = 0; i < 4; i++)
	{
		for (auto iter = pools[i].begin(); iter != pools[i].end(); iter++)
		{
			(*iter)->setActive(false);
		}

		pools[i].clear();
	}
}

MyPoolManager* MyPoolManager::getInstance()
{
	if (m_instance == nullptr)
		m_instance = new MyPoolManager();
		
	return m_instance;
}

/* add an GameObject to pool
- supported object: CHARACTER, PROJECTILE, ITEM
!!! WARNING !!! MUST PUT SUPPORTED TYPE !!!*/
void MyPoolManager::addToPool(GameObject* object)
{
	int pos = 0;

	switch (object->getType())
	{
	case GameObject::OBJECT_TYPE::CHARACTER:
		pos = (int)POOL_TYPE::CHARACTER;
		break;
	case GameObject::OBJECT_TYPE::PROJECTILE:
		pos = (int)POOL_TYPE::PROJECTILE;
		break;
	case GameObject::OBJECT_TYPE::ITEM:
		pos = (int)POOL_TYPE::ITEM;
		break;
	case GameObject::OBJECT_TYPE::EFFECT:
		pos = (int)POOL_TYPE::EFFECT;
		break;
	case GameObject::OBJECT_TYPE::DUMMY:
		pos = (int)POOL_TYPE::DUMMY;
		break;
	default:
	{
		MessageBox("YOU CAN'T ADD AN UNSUPPORTED OBJECT TO POOL MANAGER!", "Error pool push object");
		return;
	}
		break;
	}

	// if object is not exist in pool => add to pool
	if (pools[pos].find(object) == pools[pos].end())
	{
		pools[pos].pushBack(object);
	}
}

/* get a inactive object from pool
* POOL_TYPE type: CHARACTER, PROJECTILE, ITEM
* int subType: will convert to CHARACTER_TYPE, PROJECTILE_TYPE, ITEM_TYPE based on POOL_TYPE*/
GameObject* MyPoolManager::getInactiveFromPool(POOL_TYPE type, int subType)
{
	GameObject* result = nullptr;

	cocos2d::Vector<GameObject*>* pool = &pools[(int)type];

	switch (type)
	{
	case MyPoolManager::CHARACTER:
	{
		Character::CHARACTER_TYPE tmpSubType = (Character::CHARACTER_TYPE)subType;
		for (auto it = pool->begin(); it != pool->end(); it++)
		{
			if (((*it)->isActive() == false) && 
				(((Character*)(*it))->getCharType() == tmpSubType))
			{
				result = (*it);
				break;
			}
		}
	}
		break;
	case MyPoolManager::PROJECTILE:
	{
		Projectile::PROJECTILE_TYPE tmpSubType = (Projectile::PROJECTILE_TYPE)subType;
		for (auto it = pool->begin(); it != pool->end(); it++)
		{
			if (((*it)->isActive() == false) && 
				(((Projectile*)(*it))->getProjectileType() == tmpSubType))
			{
				result = (*it);
				break;
			}
		}
	}
		break;
	case MyPoolManager::ITEM:
	{
		Item::ITEM_TYPE tmpSubType = (Item::ITEM_TYPE)subType;
		for (auto it = pool->begin(); it != pool->end(); it++)
		{
			if (((*it)->isActive() == false) && 
				(((Item*)(*it))->getItemType() == tmpSubType))
			{
				result = (*it);
				break;
			}
		}
	}
		break;
	case MyPoolManager::EFFECT:
	{
		// effect is a special type, it just a thing that animate anything we want
		// so we dont need to get the correct type
		// we just let them live in here for memory management
		for (auto it = pool->begin(); it != pool->end(); it++)
		{
			if ((*it)->isActive() == false)
			{
				result = (*it);
				break;
			}
		}
	}
	break;
	case MyPoolManager::DUMMY:
	{
		Dummy::DUMMY_TYPE tmpSubType = (Dummy::DUMMY_TYPE)subType;
		for (auto it = pool->begin(); it != pool->end(); it++)
		{
			if (((*it)->isActive() == false) &&
				(((Dummy*)(*it))->getDummyType() == tmpSubType))
			{
				result = (*it);
				break;
			}
		}
	}
	break;
	default:
		MessageBox("UNSUPPORTED TYPE!", "Error pool get object");
		break;
	}
	
	return result;
}

// ##################################################################################################
GameObject* makeCharacter(
	Character::CHARACTER_TYPE type
	, float x, float y
	, int direct
	, int team
	, float patrol_left = 0, float patrol_right = 0)
{
	MyPoolManager* instance = MyPoolManager::getInstance();

	Character* result = (Character*)instance->getInactiveFromPool(MyPoolManager::POOL_TYPE::CHARACTER, (int)type);

	if (result == nullptr)
	{
		result = Character::createCharacter(type, team);

		if (result == nullptr)
		{
			MessageBox("This game not contain this type of object!", "Error pool get object");
			return nullptr;
		}

		instance->addToPool(result);
	}

	float params[10] = { x, y, direct, team, patrol_left, patrol_right, 0,0,0,0 };
	result->reset(params);

	return result;
}
// ##################################################################################################

Projectile* MyPoolManager::makeProjectile(
	Projectile::PROJECTILE_TYPE type
	, float x, float y
	, int direct
	, int team)
{
	Projectile* result = (Projectile*)this->getInactiveFromPool(MyPoolManager::POOL_TYPE::PROJECTILE, (int)type);

	if (result == nullptr)
	{
		result = Projectile::createProjectile(type);
		if (result == nullptr)
		{
			MessageBox("This game not contain this type of object!", "Error pool get object");
			return nullptr;
		}
		this->addToPool(result);
	}
	float params[10] = { x, y, direct, team, 0,0, 0,0,0,0 };
	result->reset(params);
	return result;
};

Effect* MyPoolManager::makeEffect(
	Effect::EFFECT_TYPE type
	, float x, float y
	, int direct
	, int repeat_times
	, float scale)
{
	Effect* result = (Effect*)this->getInactiveFromPool(MyPoolManager::POOL_TYPE::EFFECT, (int)type);

	if (result == nullptr)
	{
		//result = Effect::createEffect(type, x, y, duration);
		result = Effect::create();
		if (result == nullptr)
		{
			MessageBox("This game not contain this type of object!", "Error pool get object");
			return nullptr;
		}
		this->addToPool(result);
		result->setActive(false);
	}
	float params[10] = { x, y, direct, (int)type , repeat_times, scale, 0,0,0,0 };
	result->reset(params);
	return result;
};

Item* MyPoolManager::makeItem(Item::ITEM_TYPE type, float x, float y)
//#define MAKE_ITEM(type, x, y)
{
	Item* result = (Item*)this->getInactiveFromPool(MyPoolManager::POOL_TYPE::ITEM, (int)type);
	if (result == nullptr)
	{
		result = Item::createItem(type, x, y);
		if (result == nullptr)
		{
			MessageBox("This game not contain this type of object!", "Error pool get object");
			return nullptr;
		}
		this->addToPool(result);
	}

	float params[10] = { x, y, 0, 0, 0, 0, 0, 0, 0, 0 };
	result->reset(params);

	return result;
};

Dummy* MyPoolManager::makeDummy(
	Dummy::DUMMY_TYPE type
	, float x, float y
	, int direct
	, int team
	, float speed
	, float delayPerCast
	, float repeatTimes
	, float widthArea)
{
	Dummy* result = (Dummy*)this->getInactiveFromPool(MyPoolManager::POOL_TYPE::DUMMY, (int)type);

	if (result == nullptr)
	{
		result = Dummy::createDummy(type);
		if (result == nullptr)
		{
			MessageBox("This game not contain this type of object!", "Error pool get object");
			return nullptr;
		}
		this->addToPool(result);
	}
	float params[10] = { x, y, direct, team, speed, delayPerCast, repeatTimes, widthArea, 0, 0 };
	result->reset(params);
	return result;
};

//
//GameObject* makeItem(Item::ITEM_TYPE type, int x, int y)
////#define MAKE_ITEM(type, x, y)
//{
//	MyPoolManager* instance = MyPoolManager::getInstance();
//	Item* result = (Item*)instance->getInactiveFromPool(MyPoolManager::POOL_TYPE::ITEM, (int)type);
//	if (result == nullptr)
//	{
//		result = Item::createItem(type, x, y);
//		if (result == nullptr)
//		{
//			MessageBox("This game not contain this type of object!", "Error pool get object");
//			return nullptr;
//		}
//		instance->addToPool(result);
//	}
//	else
//	{
//		float params[10] = { x, y, 0, 0, 0, 0, 0, 0, 0, 0 };
//		result->reset(params);
//	}
//	return result;
//};