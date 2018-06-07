#ifndef __MY_POOLMANAGER_H__
#define __MY_POOLMANAGER_H__

#include "GameObject.h"
#include "Projectile.h"
#include "Item.h"
#include "Effect.h"
#include "Dummy.h"

#define MAKE_PROJECTILE(_type_, _x_, _y_, _direct_, _team_) MyPoolManager::getInstance()->makeProjectile(_type_, _x_, _y_, _direct_, _team_)
#define MAKE_EFFECT(_type_, _x_, _y_, _direct_, _repeat_times_, _scale_) MyPoolManager::getInstance()->makeEffect( _type_, _x_, _y_, _direct_, _repeat_times_, _scale_)
#define MAKE_ITEM(_type_, _x_, _y_, _is_disappear_) MyPoolManager::getInstance()->makeItem(_type_, _x_, _y_)
#define MAKE_DUMMY(_type_, _x_, _y_, _direct_, _team_, _speed_, _delay_per_cast_, _repeatTimes_, _widthArea_) MyPoolManager::getInstance()->makeDummy(_type_, _x_, _y_, _direct_, _team_, _speed_, _delay_per_cast_, _repeatTimes_, _widthArea_)





/* Make item and add to world
* type : item type
* x, y : position
* duration : the duration for item before it disappear
* world_layer : World::WORLD_LAYER*/
#define SPAWN_ITEM(_type_, _x_, _y_, _world_layer_) \
World::getCurrent()->addChild(MAKE_ITEM(_type_, _x_, _y_), _world_layer_)


/* Make projectile and add to world
* type : projectile type
* x, y : position
* direct : direction of projectile
* team : the team they belong to
* world_layer : World::WORLD_LAYER*/
#define SPAWN_PROJECTILE(_type_, _x_, _y_, _direct_, _team_, _world_layer_) \
World::getCurrent()->addChild(MAKE_PROJECTILE(_type_, _x_, _y_, _direct_, _team_), _world_layer_)
 //World::getInstance()->addChild(MAKE_PROJECTILE(_type_, _x_, _y_, _direct_, _team_), _world_layer_)


/* Make effect and add to world
* type : fx type
* x, y : position
* direct : direction of fx
* repeat_times : repeat times of the fx, 0 is forever
* scale : scale at 0, 1, 2, 3,... lol who define this?
* world_layer : World::WORLD_LAYER*/
#define SPAWN_EFFECT(_type_, _x_, _y_, _direct_, _repeat_times_, _scale_, _world_layer_) \
World::getCurrent()->addChild(MAKE_EFFECT(_type_, _x_, _y_, _direct_, _repeat_times_, _scale_), _world_layer_)

/* Make dummy and add to world
* type : dummy type
* x, y : position
* direct : direction (for this dummy fly forward)
* team : the team this dummy is fighting for
* speed : movement speed, (ex: fly forward speed)
* delay_per_cast : the delay after this dummy cast an ability
* repeatTimes : the number of casting ability (ex: number of lightning strike will be casted)
* widthArea : width of casting area (ex: rain of fire area)
* world_layer : World::WORLD_LAYER */
#define SPAWN_DUMMY(_type_, _x_, _y_, _direct_, _team_, _speed_, _delay_per_cast_, _repeatTimes_, _widthArea_, _world_layer_) \
World::getCurrent()->addChild(MAKE_DUMMY(_type_, _x_, _y_, _direct_, _team_, _speed_, _delay_per_cast_, _repeatTimes_, _widthArea_), _world_layer_)




// This is a SingleTon class
// Hold many GameObject, prevent re-locate memory
// Provide short-write #macro for calling
class MyPoolManager
{
public:
	enum POOL_TYPE
	{		
		CHARACTER,
		PROJECTILE,
		ITEM,
		EFFECT,
		DUMMY
		//BLOCK
		//ZONE
	};

	static MyPoolManager* getInstance();

	MyPoolManager();
	~MyPoolManager();





	/* add an GameObject to pool
	- supported object: CHARACTER, PROJECTILE, ITEM
	!!! WARNING !!! MUST PUT SUPPORTED TYPE !!!*/
	void addToPool(GameObject*);

	/* get a inactive object from pool
	* POOL_TYPE type: CHARACTER, PROJECTILE, ITEM
	* int subType: will convert to CHARACTER_TYPE, PROJECTILE_TYPE, ITEM_TYPE based on POOL_TYPE*/
	GameObject* getInactiveFromPool(POOL_TYPE, int);

	Projectile* makeProjectile(Projectile::PROJECTILE_TYPE type, float x, float y, int direct, int team);

	Effect* makeEffect(Effect::EFFECT_TYPE type, float x, float y, int direct, int repeatTimes, float scale);

	Item* makeItem(Item::ITEM_TYPE type, float x, float y);

	Dummy* makeDummy(Dummy::DUMMY_TYPE type, float x, float y, int direct, int team, float speed, float delayPerCast, float repeatTimes, float widthArea);

private:
	static MyPoolManager* m_instance;


	cocos2d::Vector<GameObject*> pools[5];
};



//GameObject* makeItem(Item::ITEM_TYPE type, int x, int y);

#endif // !__MY_POOLMANAGER_H__
