#ifndef __GAME_OBJECT_H__
#define __GAME_OBJECT_H__


#include "AudioEngine.h"
#define AUDIO experimental::AudioEngine

#include "cocos2d.h"

// base class of all GameObject
class GameObject : public cocos2d::Node
{
public:

	enum OBJECT_TYPE
	{
		NONE = 0,
		CHARACTER = 1,			// 0000 0001
		PROJECTILE = 1 << 1,	// 0000 0010
		BLOCK = 1 << 2,			// 0000 0100
		ITEM = 1 << 3,			// 0000 1000
		ZONE = 1 << 4,			// 0001 0000
		EFFECT = 1 << 5,		// 0010 0000
		DUMMY = 1 << 6,			// 0100 0000
		TEXT_EFFECT = 1 << 7	// 1000 0000
	};

	virtual ~GameObject();

	CREATE_FUNC(GameObject);
	
	/**
	 * Initializes the class ; called automatically by "create" function
	 */
	virtual bool init();

	/**
	 * Called every frame if "scheduleUpdate()" is callled and the node is "live"
	 * Overrides the "update()" function of Cocos2d
	 * @param delta : float; time in seconds between each frame
	 */
	virtual void update(float) override;

	/**
	 * Returns the Rect bouding the node
	 */
	virtual cocos2d::Rect getBoundingBox() const;

	/**
 	 * For pool manager management ("RE-USE" concept)
 	 * Enemy dies? -> Make it in-active
 	 * @param active : bool; determine to enable (make active) or disable (make in-active) the object
	 */
	void setActive(bool active);
	bool isActive();
	
	/*Implemented by inherited classes that are handle by PoolManager
	 *
	 * @param params :	0 - x position 
	 *					1 - y position
	 *					2 - based on type
	 */
	virtual void reset(float params[10]) {};// if use PURE here => can't use CREATE_FUNC


	virtual bool onContactBegin(const cocos2d::PhysicsContact &contact);
	virtual bool onContactPresolve(const cocos2d::PhysicsContact &contact);
	virtual bool onContactSeparate(const cocos2d::PhysicsContact &contact);


	// -------------------

	// _type : enum OBJECT_TYPE ; specifies the type of object
	// To get -> getType()
	CC_SYNTHESIZE_READONLY(OBJECT_TYPE, _type, Type);

	// _body : cocos2d::PhysicsBody* ; contains the physics body of current object (sprite/node)
	// To get -> getBody()
	CC_SYNTHESIZE_READONLY(cocos2d::PhysicsBody*, _body, Body);

	// _sprite : cocos2d::Sprite* ; contains the current sprite of the object
	// To get -> getSprite()
	CC_SYNTHESIZE_READONLY(cocos2d::Sprite*, _sprite, Sprite);

	// send a message to this object
	virtual void message(std::string your_message);

protected:

	// For pool manager management
	bool _isActive;

	// Current Action of the object (init -> nullptr)
	cocos2d::ActionInterval* _action;

	// Runs on active
	virtual void onEnable();
	// Runs on in-active
	virtual void onDisable();

	// called in GameObject::init()
	virtual void initPhysics();
	// called in GameObject::init()
	virtual void initSprites();
};

#endif // !__GAME_OBJECT_H__
