#ifndef _CAST_THUNDERBODY_H_
#define _CAST_THUNDERBODY_H_

#include "Dummy.h"

// dummy that cast a row of yellow lightning bolt
class CastThunderBody : public Dummy
{
public:

	virtual ~CastThunderBody();
	CREATE_FUNC(CastThunderBody);
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

	// calculate position in world
	cocos2d::Vec2 m_myPos;
};
#endif // !_CAST_THUNDERBODY_H_
