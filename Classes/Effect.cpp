#include "Effect.h"

USING_NS_CC;

#define FADE_DURATION 1.0f
#define BLACK_IN_OUT_SCALE 2.0f

// --- init static ---
Effect* Effect::m_instance = nullptr;

Animation* Effect::anim_fireExplodeSmall = nullptr;
Animation* Effect::anim_fireExplodeLarge = nullptr;
Animation* Effect::anim_fireRock = nullptr;
Animation* Effect::anim_dragonNova = nullptr;

Animation* Effect::anim_iceBallCrash = nullptr;
Animation* Effect::anim_iceWallCrash = nullptr;
Animation* Effect::anim_iceBladeStrike = nullptr;

Animation* Effect::anim_blast = nullptr;
Animation* Effect::anim_enegyExplode = nullptr;
Animation* Effect::anim_lightningStrike = nullptr;
Animation* Effect::anim_boltBlue = nullptr;
Animation* Effect::anim_boltPurple = nullptr;
Animation* Effect::anim_thunderHead = nullptr;

Animation* Effect::anim_sparkBlueI = nullptr;
Animation* Effect::anim_sparkBlueII = nullptr;

Animation* Effect::anim_sparkRedI = nullptr;
Animation* Effect::anim_sparkRedII = nullptr;

Animation* Effect::anim_sparkYellowI = nullptr;
Animation* Effect::anim_sparkYellowII = nullptr;

Animation* Effect::anim_bite = nullptr;

Animation* Effect::anim_flashBlue = nullptr;
Animation* Effect::anim_flashUlti = nullptr;
Animation* Effect::anim_chargeGoku = nullptr;

//Sequence* Effect::seq_black_in = nullptr;
//Sequence* Effect::seq_black_out = nullptr;
Animation* Effect::anim_black_in = nullptr;
Animation* Effect::anim_black_out = nullptr;


// only delete effect at the end of game
Effect::~Effect()
{
	if (this == m_instance)
	{
		anim_fireExplodeSmall->release();
		anim_fireExplodeLarge->release();
		anim_fireRock->release();
		anim_dragonNova->release();

		anim_iceBallCrash->release();
		anim_iceWallCrash->release();
		anim_iceBladeStrike->release();

		anim_blast->release();
		anim_enegyExplode->release();
		anim_lightningStrike->release();
		anim_boltBlue->release();
		anim_boltPurple->release();
		anim_thunderHead->release();

		anim_sparkBlueI->release();
		anim_sparkBlueII->release();
		anim_sparkRedI->release();
		anim_sparkRedII->release();
		anim_sparkYellowI->release();
		anim_sparkYellowII->release();

		anim_bite->release();

		anim_flashBlue->release();
		anim_flashUlti->release();
		anim_chargeGoku->release();

		/*seq_black_in->release();
		seq_black_out->release();*/
		anim_black_in->release();
		anim_black_out->release();

		// fixed issue 14/08 of Thinh
		m_instance = nullptr;
	}
}// auto deleted in MyPoolManager

bool Effect::init()
{
	if (!GameObject::init())
		return false;

	_type = OBJECT_TYPE::EFFECT;

	//_sprite = Sprite::create("black.png");
	_sprite = Sprite::create();
	this->addChild(_sprite);

	act_anim = nullptr;

	if (m_instance == nullptr)
		initFirst();
	
	return true;
}

// just init the static animates
void Effect::initFirst()
{
	m_instance = this;



	SpriteFrameCache* frameCache = SpriteFrameCache::getInstance();
	Vector<SpriteFrame*> frames;// list of frames
	char str[100] = { 0 };// string
	SpriteFrame* frame;// tmp frame

	// ########################################################################
	//  ----------- ADD MORE EFFECT ANIMATION HERE ------------	

	// Fire explode small
	for (int i = 1; i <= 39; i++)
	{
		sprintf(str, "fire_explode_II/fire_explode_II (%d).png", i);
		frame = frameCache->getSpriteFrameByName(str);
		frames.pushBack(frame);
	}

	anim_fireExplodeSmall = Animation::createWithSpriteFrames(frames, 0.01f);
	anim_fireExplodeSmall->retain();
	frames.clear();

	// fire explode large
	for (int i = 1; i <= 20; i++)
	{
		sprintf(str, "fire_explode/fire_explode (%d).png", i);
		frame = frameCache->getSpriteFrameByName(str);
		frames.pushBack(frame);
	}

	anim_fireExplodeLarge = Animation::createWithSpriteFrames(frames, 0.05f);
	anim_fireExplodeLarge->retain();
	frames.clear();

	// fire rock
	for (int i = 6; i <= 12; i++)
	{
		sprintf(str, "fire_rock/fire_rock_%d.png", i);
		frame = frameCache->getSpriteFrameByName(str);
		frames.pushBack(frame);
	}

	anim_fireRock = Animation::createWithSpriteFrames(frames, 0.1f);
	anim_fireRock->retain();
	frames.clear();

	// dragon's nova
	for (int i = 1; i <= 21; i++)
	{
		sprintf(str, "twin_dragon (%d).png", i);
		frame = frameCache->getSpriteFrameByName(str);
		frames.pushBack(frame);
	}

	anim_dragonNova = Animation::createWithSpriteFrames(frames, 0.05f);
	anim_dragonNova->retain();
	frames.clear();

	// ice ball crash
	for (int i = 3; i <= 40; i++)
	{
		sprintf(str, "ice_explode/ice_explode (%d).png", i);
		frame = frameCache->getSpriteFrameByName(str);
		frames.pushBack(frame);
	}

	anim_iceBallCrash = Animation::createWithSpriteFrames(frames, 0.01f);
	anim_iceBallCrash->retain();
	frames.clear();

	// ice blade strike
	for (int i = 25; i <= 91; i += 2)
	{
		sprintf(str, "ice_blade_strike (%d).png", i);
		frame = frameCache->getSpriteFrameByName(str);
		frames.pushBack(frame);
	}

	anim_iceBladeStrike = Animation::createWithSpriteFrames(frames, 0.033f);
	anim_iceBladeStrike->retain();
	frames.clear();

	// ice wall crash
	for (int i = 2; i <= 4; i++)
	{
		sprintf(str, "ice_wall_%d.png", i);
		frame = frameCache->getSpriteFrameByName(str);
		frames.pushBack(frame);
	}

	anim_iceWallCrash = Animation::createWithSpriteFrames(frames, 0.1f);
	anim_iceWallCrash->retain();
	frames.clear();

	// bite
	for (int i = 0; i <= 2; i++)
	{
		sprintf(str, "bite_%d.png", i);
		frame = frameCache->getSpriteFrameByName(str);
		frames.pushBack(frame);
	}

	anim_bite = Animation::createWithSpriteFrames(frames, 0.03f);
	anim_bite->retain();
	frames.clear();

	// flash blue
	for (int i = 1; i <= 12; i++)
	{
		sprintf(str, "flash_blue/flash_blue (%d).png", i);
		frame = frameCache->getSpriteFrameByName(str);
		frames.pushBack(frame);
	}

	anim_flashBlue = Animation::createWithSpriteFrames(frames, 0.05f);
	anim_flashBlue->retain();
	frames.clear();

	// flash ulti
	for (int i = 0; i <= 15; i++)
	{
		sprintf(str, "flash_ulti_%d.png", i);
		frame = frameCache->getSpriteFrameByName(str);
		frames.pushBack(frame);
	}

	anim_flashUlti = Animation::createWithSpriteFrames(frames, 0.033f);
	anim_flashUlti->retain();
	frames.clear();

	// charge goku
	for (int i = 0; i <= 3; i++)
	{
		sprintf(str, "charge_goku_%d.png", i);
		frame = frameCache->getSpriteFrameByName(str);
		frames.pushBack(frame);
	}

	anim_chargeGoku = Animation::createWithSpriteFrames(frames, 0.05f);
	anim_chargeGoku->retain();
	frames.clear();


	// black in
	for (int i = 9; i >= 0; i--)
	{
		sprintf(str, "black_%d.png", i);
		frame = frameCache->getSpriteFrameByName(str);
		frames.pushBack(frame);
	}
	frame = frameCache->getSpriteFrameByName("blank.png");
	frames.pushBack(frame);

	anim_black_in = Animation::createWithSpriteFrames(frames, 0.05f);
	anim_black_in->retain();
	frames.clear();

	// black out
	for (int i = 0; i <= 9; i++)
	{
		sprintf(str, "black_%d.png", i);
		frame = frameCache->getSpriteFrameByName(str);
		frames.pushBack(frame);
	}
	frame = frameCache->getSpriteFrameByName("blank.png");
	frames.pushBack(frame);

	anim_black_out = Animation::createWithSpriteFrames(frames, 0.05f);
	anim_black_out->retain();
	frames.clear();


	// spark blue I
	for (int i = 1; i <= 4; i++)
	{
		sprintf(str, "spark/spark_blue_I (%d).png", i);
		frame = frameCache->getSpriteFrameByName(str);
		frames.pushBack(frame);
	}

	anim_sparkBlueI = Animation::createWithSpriteFrames(frames, 0.05f);
	anim_sparkBlueI->retain();
	frames.clear();

	// spark blue II
	for (int i = 1; i <= 4; i++)
	{
		sprintf(str, "spark/spark_blue_II (%d).png", i);
		frame = frameCache->getSpriteFrameByName(str);
		frames.pushBack(frame);
	}

	anim_sparkBlueII = Animation::createWithSpriteFrames(frames, 0.05f);
	anim_sparkBlueII->retain();
	frames.clear();

	// spark red I
	for (int i = 1; i <= 4; i++)
	{
		sprintf(str, "spark/spark_red_I (%d).png", i);
		frame = frameCache->getSpriteFrameByName(str);
		frames.pushBack(frame);
	}

	anim_sparkRedI = Animation::createWithSpriteFrames(frames, 0.05f);
	anim_sparkRedI->retain();
	frames.clear();

	// spark red II
	for (int i = 1; i <= 4; i++)
	{
		sprintf(str, "spark/spark_red_II (%d).png", i);
		frame = frameCache->getSpriteFrameByName(str);
		frames.pushBack(frame);
	}

	anim_sparkRedII = Animation::createWithSpriteFrames(frames, 0.05f);
	anim_sparkRedII->retain();
	frames.clear();

	// spark yellow I
	for (int i = 1; i <= 4; i++)
	{
		sprintf(str, "spark/spark_yellow_I (%d).png", i);
		frame = frameCache->getSpriteFrameByName(str);
		frames.pushBack(frame);
	}

	anim_sparkYellowI = Animation::createWithSpriteFrames(frames, 0.05f);
	anim_sparkYellowI->retain();
	frames.clear();

	// spark yellow II
	for (int i = 1; i <= 4; i++)
	{
		sprintf(str, "spark/spark_yellow_II (%d).png", i);
		frame = frameCache->getSpriteFrameByName(str);
		frames.pushBack(frame);
	}

	anim_sparkYellowII = Animation::createWithSpriteFrames(frames, 0.05f);
	anim_sparkYellowII->retain();
	frames.clear();

	// blast // why 6 -> 10? Because the code like.
	for (int i = 6; i <= 10; i++)
	{
		sprintf(str, "blast_%d.png", i);
		frame = frameCache->getSpriteFrameByName(str);
		frames.pushBack(frame);
	}

	anim_blast = Animation::createWithSpriteFrames(frames, 0.05f);
	anim_blast->retain();
	frames.clear();

	// enegy explode
	for (int i = 6; i <= 31; i++)
	{
		sprintf(str, "enegy_explode/enegy_explode (%d).png", i);
		frame = frameCache->getSpriteFrameByName(str);
		frames.pushBack(frame);
	}

	anim_enegyExplode = Animation::createWithSpriteFrames(frames, 0.01f);
	anim_enegyExplode->retain();
	frames.clear();

	// lightning strike
	for (int i = 0; i <= 8; i++)
	{
		sprintf(str, "thunder_strike/thunder_strike_%d.png", i);
		frame = frameCache->getSpriteFrameByName(str);
		frames.pushBack(frame);
	}

	anim_lightningStrike = Animation::createWithSpriteFrames(frames, 0.05f);
	anim_lightningStrike->retain();
	frames.clear();

	// bolt blue
	for (int i = 0; i <= 1; i++)
	{
		sprintf(str, "t_bolt_%d.png", i);
		frame = frameCache->getSpriteFrameByName(str);
		frames.pushBack(frame);
	}
	for (int i = 0; i <= 1; i++)
	{
		sprintf(str, "t_bolt_%d.png", i);
		frame = frameCache->getSpriteFrameByName(str);
		frames.pushBack(frame);
	}
	frame = frameCache->getSpriteFrameByName("t_bolt_2.png");
	frames.pushBack(frame);

	anim_boltBlue = Animation::createWithSpriteFrames(frames, 0.1f);
	anim_boltBlue->retain();
	frames.clear();

	// bolt purple
	for (int i = 0; i <= 24; i++)
	{
		sprintf(str, "bolt_purple/bolt_violet_%d.png", i);
		frame = frameCache->getSpriteFrameByName(str);
		frames.pushBack(frame);
	}

	anim_boltPurple = Animation::createWithSpriteFrames(frames, 0.02f);
	anim_boltPurple->retain();
	frames.clear();

	// thunder head
	for (int i = 0; i <= 5; i++)
	{
		sprintf(str, "thunder_head/thunder_head_%d.png", i);
		frame = frameCache->getSpriteFrameByName(str);
		frames.pushBack(frame);
	}

	anim_thunderHead = Animation::createWithSpriteFrames(frames, 0.1f);
	anim_thunderHead->retain();
	frames.clear();

	// ########################################################################

	//// black in
	//		
	//frame = SpriteFrame::create("black.png", Rect(0, 0, 1, 1));

	//frames.pushBack(frame);
	//auto black_anim = Animation::createWithSpriteFrames(frames);

	//auto black_animate = Animate::create(black_anim);
	//auto fade_in = FadeTo::create(FADE_DURATION);
	//seq_black_in = Sequence::createWithTwoActions(black_animate, fade_in);
	//seq_black_in->retain();

	//// black out
	//auto fade_out = FadeOut::create(FADE_DURATION);
	//seq_black_out = Sequence::createWithTwoActions(black_animate, fade_out);
	//seq_black_out->retain();
}

void Effect::update(float delta)
{
	/*if (m_isTiming)
	{
		_timer -= delta;
		if (_timer <= 0)
		{
			this->setActive(false);
		}
	}*/

	if (act_anim)
	{
		if (act_anim->isDone())
		{
			//_sprite->setSpriteFrame("blank.png");
			this->setActive(false);
			
			act_anim->release();
			act_anim = nullptr;
		}
	}
	else
	{// just for safe
		unscheduleUpdate();
	}
}

/* create effect
* x, y is the center position
* width, height is the size of block
* physics of block created here*/
Effect* Effect::createEffect(EFFECT_TYPE type, float x, float y)
{
	Effect* result = Effect::create();
	switch (type)
	{
	case EFFECT_TYPE::FIRE_EXPLODE_SMALL:
	{
		result->act_anim = Animate::create(anim_fireExplodeSmall);
	}
		break;
	}

	if (result)
	{
		result->setPosition(Vec2(x, y));
	}

	// result must be added to world
	return result;
}

/* method call by the pool manager
* params[0], [1]: x,y : position
* params[2] : direct
* params[3] : EFFECT_TYPE
* params[4] : repeat times
* params[5] : scale*/
void Effect::reset(float params[10])
{
	this->setActive(true);
	this->setRotation(0);

	this->setPosition(params[0], params[1]);
	this->setScale(params[5] * params[2], params[5]);

	EFFECT_TYPE type = (EFFECT_TYPE)((int)params[3]);
	// ########################################################################
	//  ----------- SET EFFECT ANIMATION HERE ------------	
	switch (type)
	{
	case Effect::FIRE_EXPLODE_SMALL:
		act_anim = Animate::create(anim_fireExplodeSmall);
		break;
	case Effect::FIRE_EXPLODE_LARGE:
		act_anim = Animate::create(anim_fireExplodeLarge);
		break;
	case Effect::FIRE_ROCK:
		act_anim = Animate::create(anim_fireRock);
		break;
	case Effect::DRAGON_NOVA:
		act_anim = Animate::create(anim_dragonNova);
		break;

	case Effect::ICE_BALL_CRASH:
		act_anim = Animate::create(anim_iceBallCrash);
		break;
	case Effect::ICE_WALL_CRASH:
		act_anim = Animate::create(anim_iceWallCrash);
		break;
	case Effect::ICE_BLADE_STRIKE:
		act_anim = Animate::create(anim_iceBladeStrike);
		break;



	case Effect::BITE:
		act_anim = Animate::create(anim_bite);
		break;

	case Effect::FLASH_BLUE:
		act_anim = Animate::create(anim_flashBlue);
		break;
	case Effect::FLASH_ULTI:
		act_anim = Animate::create(anim_flashUlti);
		AUDIO::play2d("PL01_U_00024.ogg");
		break;
	case Effect::CHARGE_GOKU:
		act_anim = Animate::create(anim_chargeGoku);
		break;

	case Effect::SPARK_BLUE_I:
		act_anim = Animate::create(anim_sparkBlueI);
		break;
	case Effect::SPARK_BLUE_II:
		act_anim = Animate::create(anim_sparkBlueII);
		break;
	case Effect::SPARK_RED_I:
		act_anim = Animate::create(anim_sparkRedI);
		break;
	case Effect::SPARK_RED_II:
		act_anim = Animate::create(anim_sparkRedII);
		break;
	case Effect::SPARK_YELLOW_I:
		act_anim = Animate::create(anim_sparkYellowI);
		break;
	case Effect::SPARK_YELLOW_II:
		act_anim = Animate::create(anim_sparkYellowII);
		break;

	case Effect::BLAST:
		act_anim = Animate::create(anim_blast);
		break;
	case Effect::ENEGY_EXPLODE:
		act_anim = Animate::create(anim_enegyExplode);
		break;
	case Effect::LIGHTNING_STRIKE:
		AUDIO::play2d("bolt.ogg");
		act_anim = Animate::create(anim_lightningStrike);
		break;
	case Effect::BOLT_BLUE:
		act_anim = Animate::create(anim_boltBlue);
		break;
	case Effect::BOLT_PURPLE:
		act_anim = Animate::create(anim_boltPurple);
		break;
	case Effect::THUNDER_HEAD:
		act_anim = Animate::create(anim_thunderHead);
		break;

	case Effect::BLACK_IN:
		this->setScale(BLACK_IN_OUT_SCALE);
		act_anim = Animate::create(anim_black_in);
		break;
	case Effect::BLACK_OUT:
		this->setScale(BLACK_IN_OUT_SCALE);
		act_anim = Animate::create(anim_black_out);
		break;
	default:
		MessageBox("No animation of this type initilized!", "Effect::reset()");
		break;
	}
	// ###########################################################################
	
	// run animation & schedule update based on "repeat times"
	if ((int)params[4] > 0)
	{
		scheduleUpdate();

		if (((int)params[4] > 1) && (act_anim != nullptr))
		{
			act_anim = Repeat::create(act_anim, (int)params[4]);
		}
		
	}
	else// repeat times == 0 => forever
		if (((int)params[4] == 0 ) && (act_anim != nullptr))
		{
			act_anim = RepeatForever::create(act_anim);
		}
	

	if (act_anim)
	{
		_sprite->runAction(act_anim);
		act_anim->retain();
	}
}