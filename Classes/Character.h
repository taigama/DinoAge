#ifndef __CHARACTER_H__
#define __CHARACTER_H__

#include "GameObject.h"

// base class of all character, like players and enemies
class Character : public GameObject
{
public:
	enum class CHARACTER_TYPE
	{
		NONE = 0,
		RED_DINOSAUR = 1,
		WHITE_DINOSAUR = 2,
		YELLOW_DINOSAUR = 3,
		ENEMY_APE_DINO = 4,
		ENEMY_RAMUS = 5,
		ENEMY_WEREWOLF = 6,
		ENEMY_FEEDER = 7,			// Temporary
		DARK_PORTAL = 8,
	};
	enum class DIRECTION
	{
		LEFT = -1,
		NONE = 0,
		RIGHT = 1,

		// for take dmg handling
		TOP = 2,
		BOTTOM = 4
	};
	enum class STATE
	{
		NONE,
		IDLE,
		RUN,
		JUMP,
		DOUBLE_JUMP,
		HURTED
	};


	virtual ~Character();
	CREATE_FUNC(Character);
	virtual bool init();

	virtual void update(float delta) override;

	/**
	 * 
	 * @param type : enum CHARACTER_TYPE ; specifies the type of Character object
	 * @param team : int ; specifies the team that the current character belongs to
	 */
	static Character* createCharacter(CHARACTER_TYPE type, int team);

	/**
	 * Called by Projectile class when begins contact with Character object
	 * @param dmg   : specifies the amount of damage that character has to endure
	 * @param dir   : the direction for the character [bottom, left, right]
	 * @param force : the force that the power source affect the character
	 */
	virtual void onHit(float dmg, DIRECTION dir, float force);
	
	/**
	 * Called by programmer to start an attack (= event touch if: player)
	 */
	virtual void attackPhysical();
	virtual void attackProjectile();

	virtual void jump();

	/**
	 * Called by programmer to start moving character in the specified direction (= event touch if: player)
	 * @param direct : indicates the direction the character is going to move
	 */
	virtual void move(DIRECTION direct);


	// Callback function for when the collision begins
	virtual bool onContactBegin(const cocos2d::PhysicsContact &contact) override;
	// Callback function during the contact, but before collision is resolved
	virtual bool onContactPresolve(const cocos2d::PhysicsContact &contact) override;
	// Callback function after separate
	virtual bool onContactSeparate(const cocos2d::PhysicsContact &contact) override;
	

	/**
	 * Increase Character's health (called when obtained Meat)
	 *
	 * @param value : amount of health earned
	 */
	virtual void increaseHealth(float value);
	/**
	 * Increase the amount of earned Legayc (called when obtained Gold)
	 *
	 * @param value : amount of legacy (Gold) earned
	 */
	virtual void increaseLegacy(float value);

	// Called by "onHit()" function, when the character dies
	virtual void onDie();


	// The current health of Character
	CC_SYNTHESIZE(float, _hp, HP);

	// Number of legacy (Gold) that Character has earned
	CC_SYNTHESIZE(int, _legacy, Legacy);

	// Maximum health of Character
	CC_SYNTHESIZE_READONLY(float, _maxHP, MaxHP);

	// The team to which Character belongs to
	CC_SYNTHESIZE_READONLY(int, _team, Team);

	// Character's movement speed
	CC_SYNTHESIZE_READONLY(float, _speed, Speed);

	// Character's jumping speed
	CC_SYNTHESIZE_READONLY(float, _jumpSpeed, JumpSpeed);

    // Character's current state
	CC_SYNTHESIZE_READONLY(STATE, _state, State);
	
	// Character's (current) moving direction
	CC_SYNTHESIZE_READONLY(DIRECTION, _direction, Direction);
	
	// Type of Character
	CC_SYNTHESIZE_READONLY(CHARACTER_TYPE, _charType, CharType);

	// debug label
	CC_SYNTHESIZE_READONLY(cocos2d::Label*, _debugLabel, DebugLabel);

	// Zone container (if ya want to trigger something)
	CC_SYNTHESIZE(GameObject*, _zoneContainer, ZoneContainer);

protected:

	// Chacracter function --------

	// Adds contact listener for the current object
	// Sets the corresponding group bitmask for category, collision, and contact
	// All inherited class(es) need to call this in initPhysics()
	virtual void setColliderBitMask();

	




	//////////////////////
	// rubbishes
	//----------

	/*cocos2d::Vector<Status*> statuses;
	cocos2d::Vector<Status*> need_del_statuses;

	void addStatus(Status*);
	void delStatus(Status*);

	friend class Status;*/
};

#endif // !__CHARACTER_H__
