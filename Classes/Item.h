#ifndef __ITEM_H__
#define __ITEM_H__

#include "GameObject.h"
#include "Character.h"


/**
 * Item Object
 *
 * This type of item is kept visible event when out of screen
 * They can only disappear if being obtained by Player
 *
 */
class Item : public GameObject
{
public:
	enum ITEM_TYPE
	{
		NONE = 0,
		MEAT = 1,
		GOLD = 2,
		ENERGY_ORB = 3
	};

	virtual ~Item();

	virtual bool init();

	// Pool manager call this
	virtual void reset(float params[10]) override;
	
	// Returns an Item objects at the specified position
	// @param type : enum ITEM_TYPE ; specifies the type of the Item
	// @param x : float ; x poisition
	// @param y : float ; y position
	static Item* createItem(ITEM_TYPE type, float x, float y);

	CREATE_FUNC(Item);

	/**
	 * Specifies the item type of this object
	 *
	 * READ-ONLY property -> use getItemType() function to get
	 */
	CC_SYNTHESIZE_READONLY(ITEM_TYPE, _itemType, ItemType);


	/**
	 * Specifies the value of this item (health value, gold value, ...)
	 *
	 * READ-ONLY property -> use getValue() function to get
	 */
	CC_SYNTHESIZE_READONLY(int, _value, Value);


protected:

	// -----------------------------------------------------------------------------
	// Note: inherited classes should inherit methods initPhysics() & initSprites()
	// -----------------------------------------------------------------------------

	// Inherited physics helper functions
	virtual void setColliderBitMask();
	virtual bool onContactBegin(const cocos2d::PhysicsContact &contact) override;

	/**
	 * Carries out the corresponding effect when Player obtained the item
	 * @param target : the character that obtained this item
	 */
	virtual void onObtained(Character* target);

	/**
	 * Update Player's health and legacy everytime an item is obtained
	 */
	virtual void updateHealthHUD();
	virtual void updateLegacyHUD();

};

#endif // !__ITEM_H__
