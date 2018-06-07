#include "EnergyBar.h"

#define MAX_NUMBER_ORBS 5

#define ACTIVE_SPRITE_NAME "heal_orb12.png"
#define INACTIVE_SPRITE_NAME "heal_orb_silhouette.png"

#define ANIMATION_FRAME_START_INDEX 12
#define ANIMATION_FRAME_END_INDEX 19
#define DELAY_UNIT_TIME_ANIMATION 0.1f

#define SCALE_FACTOR 0.08f

#define TIME_FADE_OUT 0.5f
#define TIME_MOVE 0.8f

#define DEFAULT_ENERGY_LEVEL 3
#define NUMBER_ENERGY_ORBS_ADVANCED_SKILL 1

USING_NS_CC;


EnergyBar::~EnergyBar()
{
	unscheduleAllCallbacks();

	//CC_SAFE_DELETE(_updateAnimation);

	CC_SAFE_RELEASE_NULL(_updateAnimation);
}

EnergyBar* EnergyBar::createEnergyBar()
{
	return EnergyBar::create();
}

bool EnergyBar::init()
{
	// Super init
	if (!Node::init())
	{
		return false;
	}


	// Sets properties
	_maxOrbs = MAX_NUMBER_ORBS;
	_currentOrbs = DEFAULT_ENERGY_LEVEL;

	_isUpdating = false;


	// Initializes placeholders
	auto placeholder = Sprite::createWithSpriteFrameName(INACTIVE_SPRITE_NAME);
	
	placeholder->setScale(SCALE_FACTOR);

	auto padding = placeholder->getBoundingBox().size.width / 16;

	auto size = Size(MAX_NUMBER_ORBS * placeholder->getBoundingBox().size.width + (MAX_NUMBER_ORBS - 1) * padding, placeholder->getBoundingBox().size.height);
	this->setContentSize(size);		// sets size for the energy bar

	// Add placeholder into energy bar
	Sprite* orb;
	char str[20];
	for (int i = 0; i < MAX_NUMBER_ORBS; ++i)
	{
		// Based on the initial of Player's number of orbs -> initializes sprite with the corresponding texture
		if (i >= _currentOrbs)
		{
			orb = Sprite::createWithSpriteFrameName(INACTIVE_SPRITE_NAME);
		}
		else
		{
			orb = Sprite::createWithSpriteFrameName(ACTIVE_SPRITE_NAME);
		}

		sprintf(str, "orb%d", i);

		orb->setScale(SCALE_FACTOR);
		orb->setName(str);
		orb->setAnchorPoint(Vec2::ZERO);

		orb->setNormalizedPosition(Vec2(0 + i * (placeholder->getBoundingBox().size.width + padding) / getBoundingBox().size.width, 0));

		this->addChild(orb);
	}

	// Update animation
	auto spritecache = SpriteFrameCache::getInstance();
	Vector<SpriteFrame*> animFrames;
	Animation* animation;

	for (int i = ANIMATION_FRAME_START_INDEX; i < ANIMATION_FRAME_END_INDEX; ++i)
	{
		sprintf(str, "heal_orb%d.png", i);

		animFrames.pushBack(spritecache->getSpriteFrameByName(str));
	}
	animation = Animation::createWithSpriteFrames(animFrames, DELAY_UNIT_TIME_ANIMATION);

	_updateAnimation = Animate::create(animation);
	_updateAnimation->retain();


	return true;
}

// Increases the current energy level by ONE orb
void EnergyBar::increaseEnergy(Vec2 position)
{
	// If reaches maximum capacity already -> DO NOT do anything
	if (_currentOrbs == _maxOrbs)
	{
		CCLOG("ENERGY BAR IS FULL");
		return;
	}


	auto previousIndex = _currentOrbs;
	
	// Updates number of current orbs
	++_currentOrbs;
	if (_currentOrbs > _maxOrbs)
	{
		_currentOrbs == _maxOrbs;
	}

	// IF energy bar is FULL now -> update display of Player's avatar
	if (_currentOrbs == _maxOrbs)
	{
		auto hud = Director::getInstance()->getRunningScene()->getChildByName("play_scene")->getChildByName("hud_layer");
		auto borderAvatar = hud->getChildByName("border_avatar");

		borderAvatar->setVisible(true);
	}

	// Retrieves sprite of orb that needs to be "updated"
	char name[50];
	sprintf(name, "orb%d", previousIndex);

	auto sprite = dynamic_cast<Sprite*>(this->getChildByName(name));

	// Needed positions
	auto start = this->convertToNodeSpace(position);
	auto end = Vec2(sprite->getBoundingBox().getMidX(), sprite->getBoundingBox().getMidY());

	// Adds new sprite to run animation
	auto newSprite = Sprite::createWithSpriteFrameName(ACTIVE_SPRITE_NAME);

	newSprite->setScale(SCALE_FACTOR);
	newSprite->setAnchorPoint(Vec2(0.5, 0.5));
	newSprite->setPosition(start);

	sprintf(name, "animation_sprite%d", previousIndex);
	newSprite->setName("animation_sprite");
	
	this->addChild(newSprite, 2);		// on top of old sprite

	// Actions -> move energy orb from obtained position into the next-empty one in the bar
	auto moveHorizontal = EaseIn::create(MoveBy::create(TIME_MOVE, Vec3(end.x - start.x, 0, 0)), 0.2);
	auto moveVertical = MoveBy::create(TIME_MOVE, Vec3(0, end.y - start.y, 0));
	auto scale = ScaleBy::create(0.0f, 2.0f);

	auto spawn = Spawn::createWithTwoActions(moveHorizontal->clone(), moveVertical->clone());
	auto sequence = Sequence::create(spawn->clone(), scale->clone(), _updateAnimation, nullptr);

	newSprite->runAction(sequence->clone());		// run "effects"

	// Updates display for the orb
	auto func01 = [previousIndex, this](float delta) {

		// Retrieves sprite of orb that needs to be "updated"
		char name[25];
		sprintf(name, "orb%d", previousIndex);

		auto sprite = dynamic_cast<Sprite*>(this->getChildByName(name));

		// Updates new texture (active texture)
		sprite->initWithSpriteFrameName(ACTIVE_SPRITE_NAME);
		sprite->setAnchorPoint(Vec2::ZERO);

	};

	sprintf(name, "orb_appear_scheduler%d", previousIndex);
	scheduleOnce(func01, TIME_MOVE, name);

	// Removes (new) sprite that used to run animation
	auto func02 = [previousIndex, this](float delta) {

		char name[25];

		sprintf(name, "animation_sprite%d", previousIndex);
		this->removeChildByName("animation_sprite");

	};

	sprintf(name, "animation_removed_scheduler%d", previousIndex);
	scheduleOnce(func02, sequence->getDuration(), name);

}

void EnergyBar::decreaseEnergy(int nums)
{
	auto index = _currentOrbs - 1;

	// Sets state
	_isUpdating = true;

	// Updates number of current orbs
	_currentOrbs -= nums;
	if (_currentOrbs < 0)
	{
		_currentOrbs = 0;
	}

	// IF the energy bar was FULL previously -> update display of Player's avatar
	if (_currentOrbs + nums == _maxOrbs)
	{
		auto hud = Director::getInstance()->getRunningScene()->getChildByName("play_scene")->getChildByName("hud_layer");
		auto borderAvatar = hud->getChildByName("border_avatar");

		borderAvatar->setVisible(false);
	}

	auto fadeOut = FadeTo::create(TIME_FADE_OUT, 150);
	char str[50];

	// Effects
	for (int i = index, count = 0; i >= _currentOrbs; --i, ++count)
	{
		auto delay = DelayTime::create(count * TIME_FADE_OUT);
		auto sequence = Sequence::createWithTwoActions(delay->clone(), fadeOut->clone());

		// Retrieves sprite for orbs that needs to be "make in-active"
		sprintf(str, "orb%d", i);
		auto sprite = dynamic_cast<Sprite*>(this->getChildByName(str));

		// Runs effect for disappearing
		//sprite->runAction(sequence->clone());
		sprite->runAction(fadeOut->clone());

		// Schedules to re-load texture for that sprite
		auto func = [i, this](float delta) {

			char str[20];
			sprintf(str, "orb%d", i);

			auto sprite = dynamic_cast<Sprite*>(this->getChildByName(str));

			sprite->initWithSpriteFrameName(INACTIVE_SPRITE_NAME);
			sprite->setOpacity(255);
			sprite->setAnchorPoint(Vec2::ZERO);

		};

		sprintf(str, "orb_disappear_scheduler%d", i);
		//scheduleOnce(func, (count + 1) * TIME_FADE_OUT + 0.1, str);
		scheduleOnce(func, TIME_FADE_OUT, str);


		// Schedules to reset the state
		scheduleOnce([&](float delta) {

			this->setUpdatingState(false);

		}, TIME_FADE_OUT + 0.01, "reset_updating_state_scheduler");
	}
}

bool EnergyBar::isFull()
{
	return (_currentOrbs == _maxOrbs);
}

bool EnergyBar::activateAdvancedSkill()
{
	if (_currentOrbs >= NUMBER_ENERGY_ORBS_ADVANCED_SKILL)
	{
		decreaseEnergy(NUMBER_ENERGY_ORBS_ADVANCED_SKILL);
		return true;
	}
	return false;
}

bool EnergyBar::activateUltimateSkill()
{
	if (isFull())
	{
		decreaseEnergy(_maxOrbs);
		return true;
	}
	return false;
}
