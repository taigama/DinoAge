#ifndef _CAST_LIGHTNINGSTRIKE_H_
#define _CAST_LIGHTNINGSTRIKE_H_

#include "Dummy.h"

// dummy that cast a row of yellow lightning bolt
class CastLightningStrike : public Dummy
{
public:

	virtual ~CastLightningStrike();
	CREATE_FUNC(CastLightningStrike);
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

	cocos2d::Vec2 m_posCamera;
	float info_halfScreenWidth;

	cocos2d::Camera* m_camera;
};
#endif // !_FIRE_H_
