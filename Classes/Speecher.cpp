#include "Speecher.h"

#include "WorldScene.h"
#include "MenuScene1.h"
#include "EnergyBar.h"
#include "Gold.h"
#include "PlayScene.h"
#include "ResourceManager.h"

#include "SpeechModel.h"


#if (CC_TARGET_PLATFORM == CC_PLATFORM_LINUX)
		#include "ui/CocosGUI.h"
#else
		#include "ui\CocosGUI.h"
#endif

#define PLAYER_MAX_HEALTH_TEXT "100/100"
#define HEALTH_FONT_SIZE 15.0f
#define LEGACY_FONT_SIZE 18.0f
#define NOTIFICATION_FONT_SIZE 20.0f

#define SCALE_MOVEMENT_BUTTON 0.5f
#define SCALE_ACTION_BUTTON 2.00f
#define SCALE_PAUSE_BUTTON 1.1f
#define SCALE_RESUME_MENU_BUTTON 1.4f

#define TIME_TRANSITION_MAIN_MENU 0.5f

#define DELAY_TIME_ACTIVATE_ADVANCED_SKILL 1.0f
#define DELAY_TIME_ACTIVATE_ULTIMATE_SKILL 1.0f

#define SCALE_FACTOR_HP_BAR 2.0f
#define SCALE_FACTOR_BORDER_AVATAR 0.36f
#define SCALE_FACTOR_PLAYER_AVATAR 1.2f

#define PLAYER_STATUS_Z_ORDER 2
#define BUTTON_Z_ORDER 4
#define PAUSE_COMPONENT_Z_ORDER 6

USING_NS_CC;


Speecher::~Speecher()
{
	m_camera = nullptr;
}


bool Speecher::init()
{
	if (!Node::init())
	{
		return false;
	}

	m_camera = nullptr;

	auto visibleSize = Director::getInstance()->getVisibleSize();
	auto visibleOrigin = Director::getInstance()->getVisibleOrigin();


	///////////////////////////
	// 1. Necessary variables
	float padding = visibleSize.width / 24;
	auto scaleFactor = Director::getInstance()->getContentScaleFactor();

	padding *= 1.0 / scaleFactor;


	return true;
}

void Speecher::update(float delta)
{	

    auto visibleOrigin = Director::getInstance()->getVisibleOrigin();
	auto visibleSize = Director::getInstance()->getVisibleSize();


	// Gets the current Camera
	if (m_camera == nullptr)
	{
		m_camera = Camera::getDefaultCamera();

	}

	// Calculate the "new" origin position for HUD layer in respect of the Camera's position
	auto myPos = m_camera->getPosition() - visibleOrigin;
	myPos.x -= visibleSize.width / 2;
	myPos.y -= visibleSize.height / 2;

	// Sets the position of HUD layer
	this->setPosition(myPos);


}


void Speecher::hideSpeecher()
{	
	// Sets visibility of all children of HUDLayer to FALSE
	for (auto child : this->getChildren())
	{
		child->setVisible(false);
	}
}

void Speecher::showSpeecher()
{
	

	// Sets visibility of all children of HUDLayer to TRUE (except avatar's border)
	for (auto child : this->getChildren())
	{
		if (child->getName() == "border_avatar")
			continue;

		child->setVisible(true);
	}
}

void Speecher::activeSpeech(const std::string& fileName)
{
	m_camera = Camera::getDefaultCamera();
	ResourceManager* resManager = ResourceManager::getInstance();
	if (resManager->isSpeechDatasContainKey(fileName))
	{
		m_currentModel = resManager->speechDatas[fileName];
	}

}

void Speecher::loadModels()
{

}