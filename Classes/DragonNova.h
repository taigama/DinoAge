#ifndef _DRAGONNOVA_H_
#define _DRAGONNOVA_H_

#include "Projectile.h"

// just a projectile that fill the screen
class DragonNova : public Projectile
{
public:
	CREATE_FUNC(DragonNova);
	virtual bool init();

	virtual void update(float) override;


	void reset(float params[10]) override;
protected:
	void initPhysics() override;
};
#endif // !_DRAGONNOVA_H_
