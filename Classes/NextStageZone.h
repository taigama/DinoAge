#ifndef __NEXTSTAGEZONE_H__
#define __NEXTSTAGEZONE_H__

#include "Zone.h"

// Zone that is, when player is standing in this zone for a while, moving player to the destination
class NextStageZone : public Zone
{
public:

	virtual bool init() override;

	CREATE_FUNC(NextStageZone);


	virtual void message(std::string your_message) override;

	CC_SYNTHESIZE(std::string, _levelTitle, LevelTitle);
	CC_SYNTHESIZE(std::string, _stageTitle, StageTitle);

protected:
	
	// when player enter the zone
	virtual void onPlayerEntered(Character*) override;

	// zone no need to put on pool manager
	//virtual void reset(float params[10]) override;
private:
	void initSprites() override;

	// name of the map that you will load next
	std::string m_mapName;
};
#endif // !__DEATHZONE_H__
