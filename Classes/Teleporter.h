#ifndef __TELEPORTER_H__
#define __TELEPORTER_H__

#include "Zone.h"

// Zone that is, when player is standing in this zone for a while, moving player to the destination
class Teleporter : public Zone
{
public:
	

	virtual ~Teleporter();

	virtual bool init() override;

	virtual void update(float delta) override;

	CREATE_FUNC(Teleporter);

	virtual void init(float x1, float y1
		, int start_index, int end_index
		, int i_value, int ID, int direction) override;

protected:
	
	// when player enter the zone
	virtual void onPlayerEntered(Character*) override;

	virtual void onPlayerExit(Character*) override;

	// zone no need to put on pool manager
	//virtual void reset(float params[10]) override;
private:
	void initSprites() override;

	cocos2d::Vec2 m_targetLocation;

	Character* m_target;


	float m_timer;
};
#endif // !__DEATHZONE_H__
