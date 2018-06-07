#include "Dummy.h"

#include "WorldScene.h"
#include "CastLightningStrike.h"
#include "CastMeteorRain.h"
#include "CastLavaTotem.h"
#include "CastIceBladeStrike.h"
#include "CastThunderBody.h"

USING_NS_CC;

Dummy::~Dummy() {}

bool Dummy::init()
{
	if (!GameObject::init())
		return false;

	_type = OBJECT_TYPE::DUMMY;

	_host = nullptr;
	//_target = nullptr;

	return true;
}

/* Create projectile (you need to manually add child)
* type enum PROJECTILE_TYPE
* host where the projectile belong to*/
Dummy* Dummy::createDummy(DUMMY_TYPE type, Character* host)
{
	Dummy* result = nullptr;
	switch (type)
	{
	case Dummy::NONE:
		break;
	case Dummy::CAST_THUNDER_BODY:
		result = CastThunderBody::create();
		break;
	case Dummy::CAST_LIGHTNING_STRIKE:
		result = CastLightningStrike::create();
		break;
	case Dummy::CAST_METEOUR_RAIN:
		result = CastMeteorRain::create();
		break;
	case Dummy::CAST_LAVA_TOTEM:
		result = CastLavaTotem::create();
		break;
	case Dummy::CAST_GLACIAL_BLADE:
		result = CastIceBladeStrike::create();
		break;
	default:
		break;
	}

	if (result)
	{
		result->setHost(host);
	}
	return result;
}

// createProjectile call this
void Dummy::setHost(Character* host)
{
	if (host == nullptr)
		return;

	_host = host;
	_team = host->getTeam();
}

// you call this if needed
void Dummy::setTeam(Dummy* dummyBase)
{
	if (dummyBase == nullptr)
		return;

	_team = dummyBase->getTeam();
}