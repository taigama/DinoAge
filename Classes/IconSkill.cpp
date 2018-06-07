#include "IconSkill.h"

#define BUTTON_SCALE 0.60f

#define BUTTON_Z_ORDER 1
#define EFFECT_Z_ORDER 0

USING_NS_CC;


IconSkill::~IconSkill()
{
	_button = nullptr;

	// Releaeses effects
	CC_SAFE_RELEASE_NULL(_fireEffect);
	CC_SAFE_RELEASE_NULL(_iceEffect);
	CC_SAFE_RELEASE_NULL(_electricEffect);

	// Releases image tutorial
	CC_SAFE_RELEASE_NULL(_imageTutorial);
}

IconSkill* IconSkill::createIconSkill(Projectile::PROJECTILE_TYPE type, bool unlock)
{
	IconSkill* icon = IconSkill::create();
	
	icon->initButton(type, unlock);

	return icon;
}

bool IconSkill::init()
{
	// Super init
	if (!Node::init())
	{
		return false;
	}

	_isUnlock = false;		// initialized unlocked (almost)


	// Full description
	_description = "DESCRIPTION FOR SKILL";
	_buttonDescription = "COMBO BUTTONS FOR SKILL";
	_title = "TITLE FOR SKILL";
	//_imageTutorial = nullptr;


	// Sprites for GLOWING EFFECT (when being selected)

	// Fire
	_fireEffect = Sprite::create("fire_border_glowing.png");

	_fireEffect->setName("fire_effect");
	_fireEffect->setScale(BUTTON_SCALE);
	_fireEffect->setAnchorPoint(Vec2(0.5, 0.5));
	_fireEffect->setNormalizedPosition(Vec2(0.5, 0.5));

	CC_SAFE_RETAIN(_fireEffect);

	// Ice
	_iceEffect = Sprite::create("ice_border_glowing.png");

	_iceEffect->setName("ice_effect");
	_iceEffect->setScale(BUTTON_SCALE);
	_iceEffect->setAnchorPoint(Vec2(0.5, 0.5));
	_iceEffect->setNormalizedPosition(Vec2(0.5, 0.5));

	CC_SAFE_RETAIN(_iceEffect);

	// Electric
	_electricEffect = Sprite::create("electric_border_glowing.png");

	_electricEffect->setName("electric_effect");
	_electricEffect->setScale(BUTTON_SCALE);
	_electricEffect->setAnchorPoint(Vec2(0.5, 0.5));
	_electricEffect->setNormalizedPosition(Vec2(0.5, 0.5));

	CC_SAFE_RETAIN(_electricEffect);


	return true;
}

bool IconSkill::initButton(Projectile::PROJECTILE_TYPE type, bool unlock)
{
	_skillType = type;

	// Initializes textures' names for IconSkill
	char str[50];

	// FIRE SKILLS
	if ((int)type >= (int)Projectile::PROJECTILE_TYPE::FIRE0 && (int)type <= (int)Projectile::PROJECTILE_TYPE::FIRE4)
	{
		if (unlock)
			sprintf(str, "fire%d.png", (int)type - (int)Projectile::PROJECTILE_TYPE::FIRE0);
		else
			sprintf(str, "fire%d_locked.png", (int)type - (int)Projectile::PROJECTILE_TYPE::FIRE0);
	}

	// ICE SKILLS
	else if ((int)type >= (int)Projectile::PROJECTILE_TYPE::ICE0 && (int)type <= (int)Projectile::PROJECTILE_TYPE::ICE4)
	{
		if (unlock)
			sprintf(str, "ice%d.png", (int)type - (int)Projectile::PROJECTILE_TYPE::ICE0);
		else
			sprintf(str, "ice%d_locked.png", (int)type - (int)Projectile::PROJECTILE_TYPE::ICE0);
	}

	// ELECTRIC SKILLS
	else if ((int)type >= (int)Projectile::PROJECTILE_TYPE::ELECTRIC0 && (int)type <= (int)Projectile::PROJECTILE_TYPE::ELECTRIC4)
	{
		if (unlock)
			sprintf(str, "thunder%d.png", (int)type - (int)Projectile::PROJECTILE_TYPE::ELECTRIC0);
		else
			sprintf(str, "thunder%d_locked.png", (int)type - (int)Projectile::PROJECTILE_TYPE::ELECTRIC0);
	}


	_button = ui::RadioButton::create(str, str, str, str, str, ui::Widget::TextureResType::PLIST);
	_button->setScale(BUTTON_SCALE);	// scale button

	// Sets size for the IconSkill
	this->setContentSize(_button->getBoundingBox().size);

	// Setup for button
	_button->setAnchorPoint(Vec2(0.5, 0.5));
	_button->setPosition(Vec2(this->getBoundingBox().getMidX(), this->getBoundingBox().getMidY()));

	this->addChild(_button, BUTTON_Z_ORDER);

	// Sets description for the skill
	setDescription();

	// Sets price to unlock the skill
	if ((int)type % 100 == 2)				// THIRD skill
	{
		_unlockPrice = 50;
	}
	else if ((int)type % 100 == 3)			// FOURTH skill
	{
		_unlockPrice = 100;
	}
	else if ((int)type % 100 == 4)			// FIFTH skill
	{
		_unlockPrice = 200;
	}
	else
	{
		_unlockPrice = 0;
	}


	return true;
}

void IconSkill::unlockSkill()
{
	if (!_isUnlock)
	{
		_isUnlock = true;

		// Changes textures' names
		char str[50];

		// FIRE SKILLS
		if ((int)_skillType >= (int)Projectile::PROJECTILE_TYPE::FIRE0 && (int)_skillType <= (int)Projectile::PROJECTILE_TYPE::FIRE4)
		{
			sprintf(str, "fire%d.png", (int)_skillType - (int)Projectile::PROJECTILE_TYPE::FIRE0);
		}

		// ICE SKILLS
		else if ((int)_skillType >= (int)Projectile::PROJECTILE_TYPE::ICE0 && (int)_skillType <= (int)Projectile::PROJECTILE_TYPE::ICE4)
		{
			sprintf(str, "ice%d.png", (int)_skillType - (int)Projectile::PROJECTILE_TYPE::ICE0);
		}

		// ELECTRIC SKILLS
		else if ((int)_skillType >= (int)Projectile::PROJECTILE_TYPE::ELECTRIC0 && (int)_skillType <= (int)Projectile::PROJECTILE_TYPE::ELECTRIC4)
		{
			sprintf(str, "thunder%d.png", (int)_skillType - (int)Projectile::PROJECTILE_TYPE::ELECTRIC0);
		}


		_button->loadTextures(str, str, str, str, str, ui::Widget::TextureResType::PLIST);

		//// Change texture for the button
		//switch (_skillType)
		//{
		//// FIRE SKILLS
		//case Projectile::PROJECTILE_TYPE::FIRE0:
		//	_button = ui::RadioButton::create("fire0.png", "fire0.png", "fire0.png", "fire0.png", "fire0.png", ui::Widget::TextureResType::PLIST);
		//	
		//	break;
		//case Projectile::PROJECTILE_TYPE::FIRE1:
		//	_button = ui::RadioButton::create("fire1.png", "fire1.png", "fire1.png", "fire1.png", "fire1.png", ui::Widget::TextureResType::PLIST);

		//	break;
		//case Projectile::PROJECTILE_TYPE::FIRE2:
		//	_button = ui::RadioButton::create("fire2.png", "fire2.png", "fire2.png", "fire2.png", "fire2.png", ui::Widget::TextureResType::PLIST);

		//	break;
		//case Projectile::PROJECTILE_TYPE::FIRE3:
		//	_button = ui::RadioButton::create("fire3.png", "fire3.png", "fire3.png", "fire3.png", "fire3.png", ui::Widget::TextureResType::PLIST);

		//	break;
		//case Projectile::PROJECTILE_TYPE::FIRE4:
		//	_button = ui::RadioButton::create("fire4.png", "fire4.png", "fire4.png", "fire4.png", "fire4.png", ui::Widget::TextureResType::PLIST);

		//	break;

		//// ICE SKILLS
		//case Projectile::PROJECTILE_TYPE::ICE0:
		//	_button = ui::RadioButton::create("ice0.png", "ice0.png", "ice0.png", "ice0.png", "ice0.png", ui::Widget::TextureResType::PLIST);

		//	break;
		//case Projectile::PROJECTILE_TYPE::ICE1:
		//	_button = ui::RadioButton::create("ice1.png", "ice1.png", "ice1.png", "ice1.png", "ice1.png", ui::Widget::TextureResType::PLIST);

		//	break;
		//case Projectile::PROJECTILE_TYPE::ICE2:
		//	_button = ui::RadioButton::create("ice2.png", "ice2.png", "ice2.png", "ice2.png", "ice2.png", ui::Widget::TextureResType::PLIST);

		//	break;
		//case Projectile::PROJECTILE_TYPE::ICE3:
		//	_button = ui::RadioButton::create("ice3.png", "ice3.png", "ice3.png", "ice3.png", "ice3.png", ui::Widget::TextureResType::PLIST);

		//	break;
		//case Projectile::PROJECTILE_TYPE::ICE4:
		//	_button = ui::RadioButton::create("ice4.png", "ice4.png", "ice4.png", "ice4.png", "ice4.png", ui::Widget::TextureResType::PLIST);

		//	break;
		//default:
		//	break;
		//}
	}
}

void IconSkill::update(float delta)
{
	
}

void IconSkill::makeInvisible()
{
	_button->setOpacity(0);
}

void IconSkill::makeVisible(float duration)
{
	auto fadeIn = FadeIn::create(duration);

	_button->runAction(fadeIn);
}

void IconSkill::reloadTextures(Projectile::PROJECTILE_TYPE type, bool unlock)
{
	char str[50];

	// FIRE SKILLS
	if ((int)type >= (int)Projectile::PROJECTILE_TYPE::FIRE0 && (int)type <= (int)Projectile::PROJECTILE_TYPE::FIRE4)
	{
		if (unlock)
			sprintf(str, "fire%d.png", (int)type - (int)Projectile::PROJECTILE_TYPE::FIRE0);
		else
			sprintf(str, "fire%d_locked.png", (int)type - (int)Projectile::PROJECTILE_TYPE::FIRE0);
	}

	// ICE SKILLS
	else if ((int)type >= (int)Projectile::PROJECTILE_TYPE::ICE0 && (int)type <= (int)Projectile::PROJECTILE_TYPE::ICE4)
	{
		if (unlock)
			sprintf(str, "ice%d.png", (int)type - (int)Projectile::PROJECTILE_TYPE::ICE0);
		else
			sprintf(str, "ice%d_locked.png", (int)type - (int)Projectile::PROJECTILE_TYPE::ICE0);
	}

	// ELECTRIC SKILLS
	else if ((int)type >= (int)Projectile::PROJECTILE_TYPE::ELECTRIC0 && (int)type <= (int)Projectile::PROJECTILE_TYPE::ELECTRIC4)
	{
		if (unlock)
			sprintf(str, "thunder%d.png", (int)type - (int)Projectile::PROJECTILE_TYPE::ELECTRIC0);
		else
			sprintf(str, "thunder%d_locked.png", (int)type - (int)Projectile::PROJECTILE_TYPE::ELECTRIC0);
	}


	_button->loadTextures(str, str, str, str, str, ui::Widget::TextureResType::PLIST);
}

std::string IconSkill::getSummary()
{
	return _description;
}

std::string IconSkill::getTitle()
{
	return _title;
}

std::string IconSkill::getButtonInstruction()
{
	return _buttonDescription;
}

Sprite* IconSkill::getImageTutorial()
{
	return _imageTutorial;
}

void IconSkill::setDescription()
{
	//sprintf(_description, "Some description for now %d\0", (int)_skillType);


	switch (_skillType)
	{
	// FIRE SKILLS
	case Projectile::PROJECTILE_TYPE::FIRE0:
	case Projectile::PROJECTILE_TYPE::JAW_ATTACK:

		// Sets title
		_title = "MIGHTER JAW";

		// Sets description summary
		_description = "A powerful jaw attack that can frighten any human-... oops, monster-being.\nA direct attack to enemy in front of Player.";

		// Sets button description
		_buttonDescription = "Press PHYSICAL-ATTACK button.";

		// Sets image tutorial
		_imageTutorial = Sprite::create("red_dino_bite.png");

		break;

	case Projectile::PROJECTILE_TYPE::FIRE1:
	case Projectile::PROJECTILE_TYPE::FIRE:

		// Sets title
		_title = "FIRE BREATH";

		// Sets description summary
		_description = "Physical attack being too mainstream for dinosaurs? N-O W-O-R-R-I-E-S-! NOW, Tyro can \"spit\" fire to the enemy's face.\nPlayer shoots a fire projectile along its currently facing direction.";

		// Sets button description
		_buttonDescription = "PRESS PROJECTILE-ATTACK button.";

		// Sets image tutorial
		_imageTutorial = Sprite::create("red_dino_fire.png");

		break;

	case Projectile::PROJECTILE_TYPE::FIRE2:
	case Projectile::PROJECTILE_TYPE::LAVA_TOTEM:

		// Sets title
		_title = "LAVA TOTEM";

		// Sets description summary
		_description = "The mighty stomp from Tyro splits open the earth, causing the underground lava to erupt and burn any nearby creatures.\nA series of lava-rock raise from the ground in front of Player, greatly damage any enemies in the path.";

		// Sets button description
		_buttonDescription = "HOLD PHYSICAL-ATTACK button until charging effect appears, then RELEASE.";

		// Sets image tutorial
		_imageTutorial = Sprite::create("red_dino_lava_totem.png");

		break;

	case Projectile::PROJECTILE_TYPE::FIRE3:
	case Projectile::PROJECTILE_TYPE::FIREBALL:

		// Sets title
		_title = "METEOR AVALANCHE";

		// Sets description summary
		_description = "The leaking magic corrupted the current atmosphere. Focusing the inner mind, Tyro can call forth a \"flaming-avalanche\" to severe the opposing force.\nA rain of small meteors bombard the front area, dealing little damage each but great in number.";

		// Sets button description
		_buttonDescription = "HOLD PROJECTILE-ATTACK button until charging effect appears, then RELEASE.";

		// Sets image tutorial
		_imageTutorial = Sprite::create("red_dino_fire_rain.png");

		break;

	case Projectile::PROJECTILE_TYPE::FIRE4:
	case Projectile::PROJECTILE_TYPE::DRAGON_NOVA:

		// Sets title
		_title = "DRAGON NOVA";

		// Sets description summary
		_description = "Heeding the call from the Dragon bloodline, the two kings of the Old World - Latios and Latias - appear and kill everyone on their sight.\nA full scale attack that requires large energy, severly damages all enemies in sight.";

		// Sets button description
		_buttonDescription = "PRESS JUMP button two times (DOUBLE-JUMP), then PRESS PHYSICAL-ATTACK button.";

		// Sets image tutorial
		_imageTutorial = Sprite::create("red_dino_twin_dragon.png");

		break;


	// ICE SKILLS
	case Projectile::PROJECTILE_TYPE::ICE0:
	case Projectile::PROJECTILE_TYPE::HORN_ATTACK:

		// Sets title
		_title = "TRI HEAD";

		// Sets description summary
		_description = "A direct horn attack from the thought-to-be-innocent Trino. Not very ... innocent indeed.\nA headbutt to the enemy ahead.";

		// Sets button description
		_buttonDescription = "Press PHYSICAL-ATTACK button.";

		// Sets image tutorial
		_imageTutorial = Sprite::create("white_dino_horn.png");

		break;

	case Projectile::PROJECTILE_TYPE::ICE1:
	case Projectile::PROJECTILE_TYPE::ICE_SHARD:

		// Sets title
		_title = "ICE SHARD";

		// Sets description summary
		_description = "A solid ice shard flies straight from Trino's head.\nAn ice projectile is shot, damaging enemy.";

		// Sets button description
		_buttonDescription = "PRESS PROJECTILE-ATTACK button.";

		// Sets image tutorial
		_imageTutorial = Sprite::create("white_dino_ice_shard.png");

		break;

	case Projectile::PROJECTILE_TYPE::ICE2:
	case Projectile::PROJECTILE_TYPE::ICE_DRAGON:

		// Sets title
		_title = "DRAGON RUSH";

		// Sets description summary
		_description = "Bearing the power of the Frozen land, Trino can call for help from the underground Guardian.\nAn aqua dragon emerges from the ground, rushes ahead and damages all creatures in its way.";

		// Sets button description
		_buttonDescription = "HOLD PHYSICAL-ATTACK button until charging effect appears, then RELEASE.";

		// Sets image tutorial
		_imageTutorial = Sprite::create("white_dino_ice_dragon.png");

		break;

	case Projectile::PROJECTILE_TYPE::ICE3:
	case Projectile::PROJECTILE_TYPE::ICE_WALL:

		// Sets title
		_title = "ICE WALL";

		// Sets description summary
		_description = "Exploiting water from the surrounding air, Trino can cover its essence using ice walls. AN ABSOLUTE DEFENCE.\nTwo icy blocks are created protecting both sides of Player , dealing damage as they are raising.";

		// Sets button description
		_buttonDescription = "HOLD PROJECTILE-ATTACK button until charging effect appears, then RELEASE.";

		// Sets image tutorial
		_imageTutorial = Sprite::create("white_dino_ice_wall.png");

		break;

	case Projectile::PROJECTILE_TYPE::ICE4:
	case Projectile::PROJECTILE_TYPE::ICE_BLADE:

		// Sets title
		_title = "HEAVENLY SWORDS";

		// Sets description summary
		_description = "The power of ice warped even the flow of time. Highly weapons from the far future has been summoned to punish the opposing force attempting to harm Trino.\nSeveral of high damage blades are dropped from the sky, causing icy explosion at wherever they touch.";

		// Sets button description
		_buttonDescription = "PRESS JUMP button two times (DOUBLE-JUMP), then PRESS PHYSICAL-ATTACK button.";

		// Sets image tutorial
		_imageTutorial = Sprite::create("white_dino_ice_blade.png");

		break;

	
	// ELECTRIC SKILLS
	case Projectile::PROJECTILE_TYPE::ELECTRIC0:
	case Projectile::PROJECTILE_TYPE::YCLAW_ATTACK:

		// Sets title
		_title = "BIG FOOT";

		// Sets description summary
		_description = "Gifted with a big big foot, Ptero uses it to push away any enemies ahead of it.\nAn ... enormous kick to the enemy.";

		// Sets button description
		_buttonDescription = "Press PHYSICAL-ATTACK button.";

		// Sets image tutorial
		_imageTutorial = Sprite::create("yellow_dino_kick.png");

		break;

	case Projectile::PROJECTILE_TYPE::ELECTRIC1:
	case Projectile::PROJECTILE_TYPE::ELECT_ORB:

		// Sets title
		_title = "ElECTRIC ORB";

		// Sets description summary
		_description = "Able to re-charge from the underground (WHO KNOWS WHY?), Ptero can shoot an orb full of static to the enemy electrocuting the target.\nAn electrical orb projectile is shot from Ptero's foot damaging enemy.";

		// Sets button description
		_buttonDescription = "PRESS PROJECTILE-ATTACK button.";

		// Sets image tutorial
		_imageTutorial = Sprite::create("yellow_dino_orb.png");

		break;

	case Projectile::PROJECTILE_TYPE::ELECTRIC2:
	case Projectile::PROJECTILE_TYPE::LIGHTNING_STRIKE:

		// Sets title
		_title = "LIGHTNING STRIKE";

		// Sets description summary
		_description = "SHINES LIKE A LIGHTNING! Calling from the heaven, Ptero can summon a rain of thunder punishing opposing force.\nA series of lightning bolt are struck from the sky swiping enemies ahead of Ptero.";

		// Sets button description
		_buttonDescription = "HOLD PHYSICAL-ATTACK button until charging effect appears, then RELEASE.";

		// Sets image tutorial
		_imageTutorial = Sprite::create("yellow_dino_thunder.png");

		break;

	case Projectile::PROJECTILE_TYPE::ELECTRIC3:
	case Projectile::PROJECTILE_TYPE::THUNDER_BODY:

		// Sets title
		_title = "STATIC AURA";

		// Sets description summary
		_description = "As if normal attack is no where to be enough, Ptero can morph itself into a moving-charging bullet that will electrify any creatures nearby.\nA static aura cover Ptero's body, damaging enemies in the near area.";

		// Sets button description
		_buttonDescription = "HOLD PROJECTILE-ATTACK button until charging effect appears, then RELEASE.";

		// Sets image tutorial
		_imageTutorial = Sprite::create("yellow_dino_blast.png");

		break;

	case Projectile::PROJECTILE_TYPE::ELECTRIC4:
	case Projectile::PROJECTILE_TYPE::ZEUS_WRATH:

		// Sets title
		_title = "WRATH OF ZEUS";

		// Sets description summary
		_description = "THE WRATH OF THE GOD! The mighty god puts on Ptero the Mark of Thunder, outburts tons of lightning pushing enemies in the sight.\nA chain of lightning bounces between enemies in Ptero's sight and greatly cripples them.";

		// Sets button description
		_buttonDescription = "PRESS JUMP button two times (DOUBLE-JUMP), then PRESS PHYSICAL-ATTACK button.";

		// Sets image tutorial
		_imageTutorial = Sprite::create("yellow_dino_zeus_wrath.png");

		break;

	default:
		break;
	}

	CC_SAFE_RETAIN(_imageTutorial);
}

void IconSkill::enableGlowEffect()
{
	// FIRE skill
	if (_skillType >= Projectile::PROJECTILE_TYPE::FIRE0 && _skillType <= Projectile::PROJECTILE_TYPE::FIRE4)
	{
		this->addChild(_fireEffect, EFFECT_Z_ORDER);
	}

	// ICE skill
	else if (_skillType >= Projectile::PROJECTILE_TYPE::ICE0 && _skillType <= Projectile::PROJECTILE_TYPE::ICE4)
	{
		this->addChild(_iceEffect, EFFECT_Z_ORDER);
	}

	// ELECTRIC skill
	else if (_skillType >= Projectile::PROJECTILE_TYPE::ELECTRIC0 && _skillType <= Projectile::PROJECTILE_TYPE::ELECTRIC4)
	{
		this->addChild(_electricEffect, EFFECT_Z_ORDER);
	}
}

void IconSkill::disableGlowEffect()
{
	if (this->getChildByName("fire_effect") != nullptr)
	{
		this->removeChildByName("fire_effect");
	}
	else if (this->getChildByName("ice_effect") != nullptr)
	{
		this->removeChildByName("ice_effect");
	}
	else if (this->getChildByName("electric_effect") != nullptr)
	{
		this->removeChildByName("electric_effect");
	}
}
