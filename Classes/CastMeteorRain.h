#ifndef _CAST_METEOURRAIN_H_
#define _CAST_METEOURRAIN_H_

#include "Dummy.h"

// dummy that cast rain of fires
class CastMeteorRain : public Dummy
{
public:

	virtual ~CastMeteorRain();
	CREATE_FUNC(CastMeteorRain);
	virtual bool init();

	virtual void update(float) override;

	/* Reset this object
	*params[0], [1] : position x, y of object
	*params[2] : direction
	*params[3] : team
	*params[4] : speed
	*params[5] : delay_per_cast
	*params[6] : repeat times
	*params[7] : width of area
	### METHOD CALLED BY POOL ###*/
	void reset(float params[10]) override;

private:
	void strike();

	int m_maxCountStrike;
	int m_countStrike;

	float m_delay;

	float m_posX;
	float m_posY;

	float m_halfWidthArea;

	float info_screenHeight;
	cocos2d::Camera* m_camera;
};
#endif // !_CAST_METEOURRAIN_H_
