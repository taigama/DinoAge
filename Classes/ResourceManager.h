#ifndef _RESOURCEMANAGER_H_
#define _RESOURCEMANAGER_H_

#include "cocos2d.h"
#include "SpeechModel.h"

/** A Singleton class
 * Handles resource management
 */
class ResourceManager
{
public:

	ResourceManager();
	~ResourceManager();

	// Returns the ResourceManager instance
	static ResourceManager* getInstance();
	
	// Loads / Unloads all sprites in the game
	void loadSprites();
	void unloadSprites();

	void loadSounds();

	// Loads / Unloads all tiled maps in the game
	bool loadTiledMap(std::string, cocos2d::Node*);
	bool unloadTiledMap();

	// Helper functions to load the tiled map
	void loadItems();
	void loadEnemies();		// template for now

	// Starting location in each map
	cocos2d::Vec2 m_startLocation;

	cocos2d::TMXTiledMap* getMap();
	cocos2d::Rect getArea();

	int backgroundSongID;

	bool readyDelete;

	std::map<std::string, std::vector<SpeechModel*>*> speechDatas;
	void loadSpeechDatas(std::string fileName);
	bool isSpeechDatasContainKey(std::string key);
	void clearSpeechDatas();
private:
	static ResourceManager* instance;

	cocos2d::TMXTiledMap* m_map;
	cocos2d::Rect m_rect_map;

	//CREATE_FUNC(Ground);

	// load 2nd in loading tiled map
	void loadZones(cocos2d::Node*);
	// load 3rd in loading tiled map
	void loadBlocks(cocos2d::Node*);

	void loadIllusions(cocos2d::Node*);

	void loadText(cocos2d::Node*);

	void parseSpeechDatas(std::vector<std::string>* lineDatas, std::string& fileName);
};
#endif // !_RESOURCEMANAGER_H_
