#include "Ground.h"

USING_NS_CC;

Ground::~Ground() {}

bool Ground::init()
{
	if (!Block::init())
		return false;

	_blockType = BLOCK_TYPE::STATIC;

	return true;
}