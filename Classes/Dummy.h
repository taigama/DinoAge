#ifndef __DUMMY_H__
#define __DUMMY_H__

#include "GameObject.h"
#include "Character.h"

// a type of object, that is, helps character to do some work;
class Dummy : public GameObject
{
public:
	enum DUMMY_TYPE
	{
		NONE = 0,
		CAST_LIGHTNING_STRIKE,
		CAST_LAVA_TOTEM,
		CAST_METEOUR_RAIN,
		CAST_GLACIAL_BLADE,
		CAST_THUNDER_BODY
	};

	virtual ~Dummy();
	CREATE_FUNC(Dummy);
	virtual bool init();

	/**
	 * Returns a Dummy object
	 *type : enum DUMMY_TYPE
	 *host : Character* ; the owner of this dummy*/
	static Dummy* createDummy(DUMMY_TYPE type, Character* host = nullptr);

	/**
	 * Set the owner of the projectile into the given host
	 * @param host : Character* ; (new) owner of the projectile */
	void setHost(Character* host);

	// set child projectile team. Imagine: 1 projectile explode and create 10 sub-projectiles
	void setTeam(Dummy* dummyBase);

	
	// movement speed | fly speed
	CC_SYNTHESIZE_READONLY(float, _speed, Speed);
	CC_SYNTHESIZE_READONLY(int, _direction, Direction);

	CC_SYNTHESIZE_READONLY(DUMMY_TYPE, _dummyType, DummyType);
	CC_SYNTHESIZE_READONLY(int, _team, Team);

	/* Reset this object
	*params[0], [1] : position x, y of object
	*params[2] : direction
	*params[3] : team
	*params[4] : speed
	*params[5] : delay_per_cast
	*params[6] : repeat times
	*params[7] : width of area
	### METHOD CALLED BY POOL ###*/
	void reset(float params[10]) {};// if use PURE here => can't use CREATE_FUNC

protected:
	// this type of object no need physics (except some object)
	//void initPhysics() override;
	//void setColliderBitMask();
	//virtual bool onContactBegin(const cocos2d::PhysicsContact &contact);	

	// where this dummy belong to
	Character* _host;

	float _timer;
};

#endif // !__DUMMY_H__
