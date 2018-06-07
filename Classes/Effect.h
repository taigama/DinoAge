// abstract class for effect
// no collision

#ifndef __EFFECT_H__
#define __EFFECT_H__

#include "GameObject.h"

// Only one class of effect, handler all animation
// * this is far different from TextEffect, which is can't be added to MyPoolManager
class Effect : public GameObject
{
public:
	enum EFFECT_TYPE
	{
		NONE = 0,
		BITE,

		FIRE_EXPLODE_SMALL,
		FIRE_EXPLODE_LARGE,
		FIRE_ROCK,
		DRAGON_NOVA,

		ICE_BALL_CRASH,
		ICE_WALL_CRASH,
		ICE_BLADE_STRIKE,

		BLAST,
		ENEGY_EXPLODE,
		LIGHTNING_STRIKE,
		BOLT_BLUE,
		BOLT_PURPLE,
		THUNDER_HEAD,

		SPARK_BLUE_I,
		SPARK_BLUE_II,
		SPARK_RED_I,
		SPARK_RED_II,
		SPARK_YELLOW_I,
		SPARK_YELLOW_II,

		FLASH_BLUE,
		FLASH_ULTI,
		CHARGE_GOKU,

		BLACK_IN,
		BLACK_OUT
	};

	virtual ~Effect();

	virtual bool init();

	virtual void update(float) override;
	
	// Returns a Effect object, with the specified type, x, y and duration
	// * x,y : position of the effect
	// * duration : no need to answear
	static Effect* createEffect(EFFECT_TYPE type, float x, float y);

	//static Block* createPoly(float x, float y, cocos2d::Vec2* points, int count);

	CREATE_FUNC(Effect);

	// _blockType : enum BLOCK_TYPE
	// To get -> getBlockType()
	CC_SYNTHESIZE_READONLY(EFFECT_TYPE, _effectType, EffectType);

	/* method call by the pool manager
	* params[0], [1]: x,y : position
	* params[2] : direct
	* params[3] : EFFECT_TYPE
	* params[4] : repeat time
	* params[5] : scale <scale at 0, 1, 2, 3,... lol who define this> */
	void reset(float params[10]) override;

private:
	// for initilize the static animations
	static Effect* m_instance;
	void initFirst();



	// ====== animation =========
	// main animation per fx
	cocos2d::ActionInterval* act_anim;

	// static animation
	static cocos2d::Animation* anim_fireExplodeSmall;
	static cocos2d::Animation* anim_fireExplodeLarge;
	static cocos2d::Animation* anim_fireRock;
	static cocos2d::Animation* anim_dragonNova;

	static cocos2d::Animation* anim_iceBallCrash;
	static cocos2d::Animation* anim_iceWallCrash;
	static cocos2d::Animation* anim_iceBladeStrike;

	static cocos2d::Animation* anim_blast;
	static cocos2d::Animation* anim_enegyExplode;
	static cocos2d::Animation* anim_lightningStrike;
	static cocos2d::Animation* anim_boltBlue;
	static cocos2d::Animation* anim_boltPurple;
	static cocos2d::Animation* anim_thunderHead;

	static cocos2d::Animation* anim_sparkBlueI;
	static cocos2d::Animation* anim_sparkBlueII;

	static cocos2d::Animation* anim_sparkRedI;
	static cocos2d::Animation* anim_sparkRedII;

	static cocos2d::Animation* anim_sparkYellowI;
	static cocos2d::Animation* anim_sparkYellowII;

	static cocos2d::Animation* anim_bite;

	static cocos2d::Animation* anim_flashBlue;
	static cocos2d::Animation* anim_flashUlti;
	static cocos2d::Animation* anim_chargeGoku;

	static cocos2d::Animation* anim_black_in;
	static cocos2d::Animation* anim_black_out;
	/*static cocos2d::Sequence* seq_black_in;
	static cocos2d::Sequence* seq_black_out;*/
};

#endif // !__EFFECT_H__
