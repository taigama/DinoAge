#ifndef __ICON_SKILL_H__
#define __ICON_SKILL_H__

#include "Projectile.h"

#if (CC_TARGET_PLATFORM == CC_PLATFORM_LINUX)
		#include "ui/CocosGUI.h"
#else
		#include "ui\CocosGUI.h"
#endif


class IconSkill : public cocos2d::Node
{
public:

	~IconSkill();

	bool init();

	/**
	 * Returns the icon for the specified skill
	 * @param type     : type of the skill
	 * @param unlock   : unlock state of the skill
	 */
	static IconSkill* createIconSkill(Projectile::PROJECTILE_TYPE type, bool unlock);

	// Unlock the skill -> make it available for Player to use
	void unlockSkill();

	void update(float delta);

	/** Sets opacity to 0 (invisible) or 255 (visible)
	 *	@param duration : duration for FADE-IN action for makeVisible()
	 */
	void makeInvisible();
	void makeVisible(float duration);

	/**
	 *	Enable glowing effect (when the IconSkill is selected)
	 *	by adding a blurry-border sprite
	 */
	void enableGlowEffect();
	void disableGlowEffect();

	// Returns the skill's description (title + summary + instruction);
	std::string getSummary();
	std::string getTitle();
	std::string getButtonInstruction();
	cocos2d::Sprite* getImageTutorial();

	// Sets the FULL description (summary + title + image tutorial) for the skill based on its type
	void setDescription();

	/**
	 * Re-load the textures of the current IconSkill object
	 * based on the given skill type
	 * @param type     : skill type
	 * @param unlock   : unlock/lock state of the skill
	 */
	void reloadTextures(Projectile::PROJECTILE_TYPE type, bool unlock);

	// Button component of the corresponding skill
	CC_SYNTHESIZE_READONLY(cocos2d::ui::RadioButton*, _button, Button);

	// Sprites for glowing effect
	CC_SYNTHESIZE_READONLY(cocos2d::Sprite*, _fireEffect, FireEffect);
	CC_SYNTHESIZE_READONLY(cocos2d::Sprite*, _iceEffect, IceEffect);
	CC_SYNTHESIZE_READONLY(cocos2d::Sprite*, _electricEffect, ElectricEffect);


	// Number of LEGACY (i.e. GOLD) to unlock the skill
	CC_SYNTHESIZE_READONLY(int, _unlockPrice, UnlockPrice);

	// Type of the skill
	CC_SYNTHESIZE(Projectile::PROJECTILE_TYPE, _skillType, SkillType);

	CREATE_FUNC(IconSkill);

protected:

	/**
	 * Initializes the button for the skill icon
	 * @param type     : type of the skill
	 * @param unlock   : unlock state of the skill
	 */
	bool initButton(Projectile::PROJECTILE_TYPE type, bool unlock);
	
	// Unlock state of the skill
	bool _isUnlock;

	// Description of the skill (summary + title + combo button)
	std::string _description;
	std::string _buttonDescription;		// button or combo of buttons to activate the skill
	std::string _title;

	// Tutorial (in image) on how to use the skill
	cocos2d::Sprite* _imageTutorial;

};


#endif // !__ICON_SKILL_H
