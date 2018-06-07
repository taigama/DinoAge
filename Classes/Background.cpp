#include "Background.h"

#include "WorldScene.h"
#include "CameraHelper.h"

#include "ResourceManager.h"

USING_NS_CC;

//#define BASE_SCALE 1.0f
#define BACK_SCALE_FACTOR 0.35f

//#define BACK_OFFSET_Y 70.0f

Background::~Background()
{
}

bool Background::init()
{
	if (!Node::init())
		return false;	

	// ensure this background will be updated after the camera being updated
	this->scheduleUpdateWithPriority(99);
	//this->scheduleUpdate();

	_back = nullptr;
	_mid = nullptr;
	_front = nullptr;

	//m_camera = nullptr;
	//offsetBack = Vec2(0, BACK_OFFSET_Y / Director::getInstance()->getContentScaleFactor());
	m_scaleFactor = Director::getInstance()->getContentScaleFactor();

	return true;
}

// init new background with strings
void Background::initLayer(std::string back, std::string mid, std::string front)
{
	_back = Sprite::create(back);
	if (_back)
	{
		//_back->setScale(BASE_SCALE);
		Size mapSize = ResourceManager::getInstance()->getArea().size;
		Size mySize = _back->getBoundingBox().size;
		float scaleX = (mapSize.width / mySize.width) * BACK_SCALE_FACTOR;
		float scaleY = (mapSize.height / mySize.height) * BACK_SCALE_FACTOR;
		_back->setScale(MAX(scaleX, scaleY));

		this->addChild(_back, -1);

		m_midMap = Vec2(mapSize.width / 2.0f, mapSize.height / 2.0f);
	}
	else
	{
		MessageBox("Can't load back!", "Background::newBackground()");
	}

	// mid, front
}

void Background::update(float delta)
{


	/*if (m_camera)
	{*/
		if (_back)
		{
			//auto myPos = m_camera->getPosition() - Director::getInstance()->getVisibleOrigin();
			//auto myPos = World::getInstance()->getPlayer()->getPosition();
			//_back->setPosition(myPos);

			m_backPos = CameraHelper::getInstance()->getPosition();
			m_backPos -= (m_backPos - m_midMap) * BACK_SCALE_FACTOR * BACK_SCALE_FACTOR;
			//m_backPos.y += BACK_OFFSET_Y / m_scaleFactor;

			_back->setPosition(m_backPos);
		}
	/*}
	else
	{
		m_camera = Camera::getDefaultCamera();
	}*/
}
