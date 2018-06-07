#include "Projectile.h"
#include "WorldScene.h"

#include "Block.h"

#include "Fire.h"
#include "FireBall.h"
#include "LavaTotem.h"
#include "DragonNova.h"

#include "IceShard.h"
#include "IceDragon.h"
#include "IceWall.h"
#include "IceBlade.h"
#include "IceBladeStrike.h"

#include "ElectOrb.h"
#include "ThunderBody.h"
#include "LightningStrike.h"
#include "ZeusWrath.h"

#include "Spike.h"
#include "Charge.h"
#include "JawAttack.h"
#include "HornAttack.h"
#include "YClawAttack.h"
#include "EnemyApeDino.h"
#include "ClawAttack.h"
#include "RollAttack.h"
#include "SelfExplode.h"

#define DIAGONAL_NORMAL_CONS 0.7f// sqrt(2) / 2

USING_NS_CC;


Projectile::~Projectile()
{

}

bool Projectile::init()
{
	if (!GameObject::init())
		return false;

	_type = OBJECT_TYPE::PROJECTILE;
	_damage = 0;
	_projectileType = PROJECTILE_TYPE::NONE;
	_team = 0;

	_force = PROJECTILE_BASE_FORCE;

	_host = nullptr;
	//_target = nullptr;

	_isStatic = true;

	return true;
}

/* Create projectile (you need to manually add child)
* type enum PROJECTILE_TYPE
* host where the projectile belong to*/
Projectile* Projectile::createProjectile(PROJECTILE_TYPE type, Character* host)
{
	Projectile* result = nullptr;
	switch (type)
	{
	case Projectile::FIRE:
		result = Fire::create();
		break;
	case Projectile::FIREBALL:
		result = FireBall::create();
		break;
	case Projectile::LAVA_TOTEM:
		result = LavaTotem::create();
		break;
	case Projectile::DRAGON_NOVA:
		result = DragonNova::create();
		break;

	case Projectile::ICE_SHARD:
		result = IceShard::create();
		break;
	case Projectile::ICE_DRAGON:
		result = IceDragon::create();
		break;
	case Projectile::ICE_WALL:
		result = IceWall::create();
		break;
	case Projectile::ICE_BLADE:
		result = IceBlade::create();
		break;
	case Projectile::ICE_BLADE_STRIKE:
		result = IceBladeStrike::create();
		break;

	case Projectile::ELECT_ORB:
		result = ElectOrb::create();
		break;
	case Projectile::THUNDER_BODY:
		result = ThunderBody::create();
		break;
	case Projectile::LIGHTNING_STRIKE:
		result = LightningStrike::create();
		break;
	case Projectile::ZEUS_WRATH:
		result = ZeusWrath::create();
		break;


	case Projectile::SPIKE_ATTACK:
		result = Spike::create();
		break;
	case Projectile::JAW_ATTACK:
		result = JawAttack::create();
		break;
	case Projectile::YCLAW_ATTACK:
		result = YClawAttack::create();
		break;
	case Projectile::HORN_ATTACK:
		result = HornAttack::create();
		break;
	case Projectile::CHARGE_ATTACK:
		result = Charge::create();
		break;
	case Projectile::ROLL_ATTACK:
		result = RollAttack::create();
		break;
	case Projectile::CLAW_ATTACK:
		result = ClawAttack::create();
		break;
	case Projectile::SELF_EXPLODE:
		result = SelfExplode::create();
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
void Projectile::setHost(Character* host)
{
	if (host == nullptr)
		return;

	_host = host;
	_team = host->getTeam();
}

// you call this if needed
void Projectile::setTeam(Projectile* projectileBase)
{
	if (projectileBase == nullptr)
		return;

	_team = projectileBase->getTeam();
}




void Projectile::update(float delta) {}



// initPhysics call this
void Projectile::setColliderBitMask()
{
	if (_body == nullptr)
		return;

	_body->setCategoryBitmask((int)OBJECT_TYPE::PROJECTILE);// projectile
	_body->setCollisionBitmask(0);

	_body->setContactTestBitmask(
	(int) OBJECT_TYPE::CHARACTER
		| (int)OBJECT_TYPE::PROJECTILE
		| (int)OBJECT_TYPE::BLOCK
	);
}

// auto callback
bool Projectile::onContactBegin(const cocos2d::PhysicsContact &contact)
{
	// Retrieves PhysicsBody of target
	auto targetBody = contact.getShapeA()->getBody();

	if (_body == targetBody)		// if current body is of shape A
	{
		targetBody = contact.getShapeB()->getBody();
	}


	// Checks whether the target is Character
	if (targetBody->getCategoryBitmask() == (int)GameObject::OBJECT_TYPE::CHARACTER)
	{
		Character* target = (Character*)targetBody->getOwner();

		// Target is of opposite team -> inflicts damage
		if (target->getTeam() != _team)
		{
			Vec2 normal = contact.getContactData()->normal;

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


			// vertical collision (skip if SelfExplode projectile)
			if ((normal.y < -DIAGONAL_NORMAL_CONS || normal.y > DIAGONAL_NORMAL_CONS) && _projectileType != Projectile::PROJECTILE_TYPE::SELF_EXPLODE)
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

			// If Projectile == Charge_Attack -> call function chargeHit() on its owner (EnemyApeDino)
			if (this->getProjectileType() == Projectile::PROJECTILE_TYPE::CHARGE_ATTACK)
			{
				// Retrieves owner
				auto enemy = dynamic_cast<EnemyApeDino*>(this->getParent());

				enemy->getHit();
			}
		}

		// same team -> SKIP
		else
		{
			return false;
		}
	}
	else if (!_isStatic)
	{
		if (targetBody->getCategoryBitmask() == (int)GameObject::OBJECT_TYPE::BLOCK)
		{
			Block* tmpBlk = (Block*)targetBody->getOwner();
			if (tmpBlk->getTeam() != _team)
			{
				this->onDie();
			}
		}
	}


	return true;
}


/**
 * Carries out effect when the projectile "hit" character
 *
 * @param target : Character* ; the character that is hit by this projectile
 * @param dir: DIRECTION ; the direction of character face this projectile
 */
void Projectile::attack(Character* target, Character::DIRECTION dir)
{
	//_damage
	target->onHit(_damage, dir, _force);

	// ----- test ----------
	//char str[100] = { 0 };
	//sprintf(str, "onHit dmg: %.0f - health remain: %.1f", _damage, target->getHP());
	//Label* label = (Label*)target->getChildByTag(123123);
	//if (label == nullptr)
	//{
	//	label = Label::createWithTTF(str, "fonts/Marker Felt.ttf", 15);
	//	label->setPosition(Vec2(0, 60));
	//	target->addChild(label, 5, 123123);
	//}
	//else
	//{
	//	label->setString(str);
	//}
	//onDie();	
}

void Projectile::onDie()
{
	// spawn effect
	this->setActive(false);
}