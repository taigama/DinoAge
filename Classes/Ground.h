#ifndef _GROUND_H_
#define _GROUND_H_

#include "Block.h"

// invisible block, placed on tiled ground, prevent player moving through (except some special case)
class Ground : public Block
{
public:

	virtual ~Ground();

	virtual bool init();

	CREATE_FUNC(Ground);
};
#endif // !_BRICK_H_
