#ifndef __DEATHZONE_H__
#define __DEATHZONE_H__

#include "Zone.h"

// a Zone that made player hurt when touched
class DeathZone : public Zone
{
public:
	

	virtual ~DeathZone();

	virtual bool init() override;

	CREATE_FUNC(DeathZone);

	float dmg;

	virtual void init(
		float x1, float y1
		, int start_index, int end_index
		, int i_value, int ID, int direction);

	virtual bool onContactBegin(const cocos2d::PhysicsContact &contact) override;

private:
	int m_direction;
	// zone no need to put on pool manager
	//virtual void reset(float params[10]) override;

};

#endif // !__DEATHZONE_H__
