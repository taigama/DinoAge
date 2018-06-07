#ifndef __PROJECTILE_H__
#define __PROJECTILE_H__

#include "GameObject.h"
#include "Character.h"

#define PROJECTILE_BASE_FORCE 1.0f

// base class of projectiles
// anything that use for attacking, is projectile.
// *ex: spike of a enemy, or jaw of dinosaur
class Projectile : public GameObject
{
public:
	enum PROJECTILE_TYPE
	{
		NONE = 0,

		// PHYSICAL skills
		JAW_ATTACK,
		HORN_ATTACK,
		YCLAW_ATTACK,

		// FIRE skills
		FIRE,
		FIREBALL,
		LAVA_TOTEM,
		DRAGON_NOVA,

		FIRE0 = 100, FIRE1, FIRE2, FIRE3, FIRE4,	// PROJECTILE_TYPEs for SKILL ICONS

		// ICE skills
		ICE_SHARD,
		ICE_DRAGON,
		ICE_WALL,
		ICE_BLADE,
		ICE_BLADE_STRIKE,

		ICE0 = 200, ICE1, ICE2, ICE3, ICE4,			// PROJECTILE_TYPEs for SKILL ICONS

		// ELECTRIC skills
		ELECT_ORB,
		LIGHTNING_STRIKE,
		THUNDER_BODY,
		ZEUS_WRATH,

		ELECTRIC0 = 300, ELECTRIC1, ELECTRIC2, ELECTRIC3, ELECTRIC4,	// PROJECTILE_TYPEs for SKILL ICONS

		// For EnemyWerewolf
		CLAW_ATTACK,

		// For EnemyFeeder
		SELF_EXPLODE,

		// For EnemyApeDino
		SPIKE_ATTACK,
		CHARGE_ATTACK,

		// For EnemyRamus
		ROLL_ATTACK
	};


	virtual ~Projectile();
	CREATE_FUNC(Projectile);
	virtual bool init();

	/**
	 * Returns a Projectile object
	 * @param type : enum PROJECTILE_TYPE ; specifies projectile's type
	 * @param host : Character* ; the owner of the projectile
	 */
	static Projectile* createProjectile(PROJECTILE_TYPE type, Character* host = nullptr);

	/**
	 * Set the owner of the projectile into the given host
	 * @param host : Character* ; (new) owner of the projectile */
	void setHost(Character* host);

	// set child projectile team. Imagine: 1 projectile explode and create 10 sub-projectiles
	void setTeam(Projectile* projectileBase);

	virtual void update(float) override;

	virtual bool onContactBegin(const cocos2d::PhysicsContact &contact);


	// idea: know this is true projectile
	// using: prevent projectile lost when contact the Ice Wall
	bool isStatic() { return _isStatic; }
	
	// movement speed | fly speed
	CC_SYNTHESIZE_READONLY(float, _speed, Speed);


	CC_SYNTHESIZE_READONLY(float, _damage, Damage);
	CC_SYNTHESIZE_READONLY(PROJECTILE_TYPE, _projectileType, ProjectileType);
	CC_SYNTHESIZE_READONLY(int, _team, Team);

	// force for knocking enemy back
	CC_SYNTHESIZE_READONLY(float, _force, Force);

	

protected:
	// using in your initPhysics()
	void setColliderBitMask();


	/* Carries out effect when the projectile "hit" character
	@param target : Character* ; the character that is hit by this projectile
	@param dir: DIRECTION ; the direction of character face this projectile*/
	virtual void attack(Character*, Character::DIRECTION);


	virtual void onDie();

	// where this projectile belong to
	Character* _host;

	// (imagine: homing missile), no need for now
	//Character* _target;

	// timer for this projectile, make it dissapear
	float _timer;

	// idea: know this is true projectile
	// using: prevent projectile lost when contact the Ice Wall
	bool _isStatic;
};

#endif // !__PROJECTILE_H__
