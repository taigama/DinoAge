#include "DeathZone.h"
#include "WorldScene.h"


USING_NS_CC;

#define TEAM_PLAYER 1

// check direction of contact normal
#define DIAGONAL_NORMAL_CONS 0.7f// sqrt(2)  /  2

// target -> onhit
#define BASE_FORCE 0.7f

DeathZone::~DeathZone() {}

bool DeathZone::init()
{
	if (!Zone::init())
		return false;

	dmg = 0;
	return true;
}

bool DeathZone::onContactBegin(const cocos2d::PhysicsContact &contact)
{
	PhysicsBody* targetBody = contact.getShapeA()->getBody();
	if (_body == targetBody)// target la b
	{
		targetBody = contact.getShapeB()->getBody();
	}

	
	if (targetBody->getCategoryBitmask() == (int)GameObject::CHARACTER)
	{
		Character* target = (Character*)targetBody->getOwner();

		if (target->getTeam() != TEAM_PLAYER)
		{
			target->onHit(9999.0f, Character::DIRECTION::BOTTOM, BASE_FORCE);
		}


		Vec2 normal = contact.getContactData()->normal;

		Node* world = World::getCurrent();
		Node* parrent;


		Vec2 tarPos = target->getPosition();
		parrent = target->getParent();
		while (parrent != world && parrent != nullptr)
		{
			tarPos = parrent->convertToWorldSpace(tarPos);
			parrent = parrent->getParent();
		}

		Character::DIRECTION directForTarget = Character::DIRECTION::TOP;
		float force = BASE_FORCE;

		// vertical collision (skip if SelfExplode projectile)
		if (normal.y < -DIAGONAL_NORMAL_CONS || normal.y > DIAGONAL_NORMAL_CONS)
		{
			if (_position.y < tarPos.y)
			{
				directForTarget = Character::DIRECTION::BOTTOM;
				force = BASE_FORCE * 2.0f;
			}
		}
		else
		{
			// horizontal collision
			if (_position.x < tarPos.x)
			{
				directForTarget = Character::DIRECTION::LEFT;
			}
			else
			{
				directForTarget = Character::DIRECTION::RIGHT;
			}
		}

		if ((m_direction == 0) || ((int)directForTarget == m_direction))
		{
			target->onHit(dmg, directForTarget, force);
		}
	}


	return true;
}


void DeathZone::init(float x1, float y1
	, int start_index, int end_index
	, int i_value, int ID, int direction)
{
	dmg = i_value;
	_ID = ID;

	if (direction < 2)
	{
		m_direction = -1 * direction;
	}
	else
	{
		direction -= 3;
		m_direction = -1 * direction;
		m_direction += 3;
	}


	// reset collision bitmask
	_body->setCollisionBitmask(GameObject::OBJECT_TYPE::CHARACTER);
}