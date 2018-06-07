#ifndef __CAMERA_HELPER_H__
#define __CAMERA_HELPER_H__

#include "cocos2d.h"

// SingleTon class
// should delete on destructor of the scene
// automatic update position of camera and provide Screen info for shorter get
class CameraHelper
{
public:

	~CameraHelper();
	CameraHelper();

	static CameraHelper* getInstance();

	// player will call this
	void updatePosition(cocos2d::Vec2);

	void setMargin(float marginLength);

	// Rect of the map
	CC_SYNTHESIZE_READONLY(cocos2d::Rect, _area, Area);

	// visible size of the screen
	CC_SYNTHESIZE_READONLY(cocos2d::Size, _visSize, VisibleSize);

	// current camera position info
	CC_SYNTHESIZE_READONLY(cocos2d::Vec2, _pos, Position);

private:
	

	static CameraHelper* m_instance;

	cocos2d::Camera* m_camera;

	float m_offsetY;
};

#endif // !__CAMERA_HELPER_H__
