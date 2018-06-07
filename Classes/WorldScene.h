#ifndef __WORLD_SCENE_H__
#define __WORLD_SCENE_H__

#include "cocos2d.h"
#include "Player.h"
#include "CharacterSelectionScene.h"

class World : public cocos2d::Layer
{
public:
	World();
	~World();

	enum WORLD_LAYER
	{		
		BACKGROUND = -3,
		EFFECT_BACKGROUND = -2,
		TILED_MAP = -1,
		GROUND = 0,
		EFFECT_BACK = 1,
		OBJECT = 2,
		PLAYER = 3,
		EFFECT_FRONT = 4
	};



    static cocos2d::Layer* createLayer();

	// new create layer function
	static cocos2d::Layer* createLayer(std::string mapName);
    virtual bool init();

	// new init() function
	virtual bool init(std::string mapName);

	/**
	 * Sets the Character for user to player as he/she had selected
	 * @param selection : selected Character by user in the Character Selection scene (CharacterSelectionScene class)
	 */
	void setPlayer(CharacterSelectionScene::SELECTION selection);
    
    // a menu close callback
    void exitGame(cocos2d::Ref* pSender);
    
    // implement the "static create()" method manually
    CREATE_FUNC(World);

	void update(float delta);

	// Returns the main player of the game
	Player* getPlayer();

	//static World* getInstance();
	static World* getCurrent();
	static void setCurrent(World*);

	//cocos2d::Vector<GameObject*> zones;
	cocos2d::Map<int, GameObject*> zones;



	void addContactListener();
	virtual bool onContactBegin(const cocos2d::PhysicsContact &contact);
	virtual bool onContactPresolve(cocos2d::PhysicsContact &contact);
	virtual bool onContactSeparate(const cocos2d::PhysicsContact &contact);
	



private:

	Player* m_mainPlayer;

	//static World* m_instance;
	static World* m_current;

	bool m_check1;
	bool m_check2;
};

#endif // __WORLD_SCENE_H__
