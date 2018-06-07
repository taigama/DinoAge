#include "GameObject.h"

USING_NS_CC;

GameObject::~GameObject()
{
}

Rect GameObject::getBoundingBox() const
{ 
    if (_sprite)
        return getSprite()->getBoundingBox();
    return Rect();
}

bool GameObject::init()
{
	if (!Node::init())
		return false;

	// yeu cau cocos auto goi ham update
	//
	// Instructs Cocos2d to call "update()" function every frame
	this->scheduleUpdate();

	// ================ init start

	_type = GameObject::NONE;

	_isActive = true;

	_body = nullptr;
	_sprite = nullptr;
	_action = nullptr;

	initSprites();
	initPhysics();

	return true;
}

void GameObject::update(float delta) {}
void GameObject::initPhysics() {}
void GameObject::initSprites() {}


bool GameObject::isActive() { return _isActive; }
void GameObject::onEnable()
{
	//this->setVisible(true);
}
void GameObject::onDisable()
{
	// dem toi 1 noi xa xoi
	//
	// Moves the current object outside of the visible windows
	
	//this->setVisible(false); // ??

	// no need to do this
	//this->setPosition(Vec2(-9999, -9999));

	this->removeFromParent();

}

void GameObject::setActive(bool active)
{
	// neu thay doi trang thai active -> goi ham onEnable hoac onDisable
	//
	// If active state changes -> calls onEnable() if active == true; Otherwise, calls onDisable()
	if (_isActive != active)
	{
		if (active)
		{
			onEnable();
		}
		else
		{
			onDisable();
		}
		_isActive = active;
	}
}

bool GameObject::onContactBegin(const cocos2d::PhysicsContact &contact)
{

	log("contact begin! type: %d", (int) this->getType());

	return true;
}

bool GameObject::onContactPresolve(const cocos2d::PhysicsContact &contact)
{

	return true;
}

bool GameObject::onContactSeparate(const cocos2d::PhysicsContact &contact)
{

	return true;
}

void GameObject::message(std::string)
{

}