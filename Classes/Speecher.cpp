#include "Speecher.h"


#include "PlayScene.h"
#include "ResourceManager.h"

#include "SpeechModel.h"


#if (CC_TARGET_PLATFORM == CC_PLATFORM_LINUX)
		#include "ui/CocosGUI.h"
#else
		#include "ui\CocosGUI.h"
#endif

#define BACKGROUND_ALPHA 150
#define BACKGROUND_HEIGHT 30 // by percent (%)

//
#define LEFT_AVA_PADDING 10// by pixel
#define RIGHT_AVA_PADDING 10// by pixel

#define AVA_SIZE 50// by pixel


//
#define TEXT_NAME_SCALE_X 1.1f

#define TEXT_NAME_SIZE 20
#define TEXT_CONTENT_SIZE 18
#define TEXT_NAME_HEIGHT 30

#define FONT_FILE "verdana.ttf"

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

	///////////////////////////
	// 1. Necessary variables
	auto visibleSize = Director::getInstance()->getVisibleSize();
	auto visibleOrigin = Director::getInstance()->getVisibleOrigin();
	auto scaleFactor = Director::getInstance()->getContentScaleFactor();

	///////////////////////////
	// 2. Back ground
	m_background = LayerColor::create(Color4B(255, 255, 255, BACKGROUND_ALPHA), visibleSize.width, (BACKGROUND_HEIGHT/100.0f) *visibleSize.height);
	m_background->setAnchorPoint(Vec2(0.5f, 1.0f));
	m_background->setPosition(Vec2(visibleSize.width / 2.0f, visibleSize.height) + visibleOrigin);
	this->addChild(m_background, 1);
	m_background->setVisible(false);

	///////////////////////////
	// 3. left

	// ----- ava --------
	m_avaLeft = Sprite::create();
	m_avaLeft->setAnchorPoint(Vec2(0, 1));
	Vec2 vecPos = Vec2(LEFT_AVA_PADDING / scaleFactor, visibleSize.height - LEFT_AVA_PADDING / scaleFactor);
	m_avaLeft->setPosition(vecPos + visibleOrigin);
	this->addChild(m_avaLeft, 2);
	m_avaLeft->setVisible(false);

	// ---- name --------
	m_lbNameLeft = Label::create("", FONT_FILE, TEXT_NAME_SIZE);
	m_lbNameLeft->setAnchorPoint(Vec2(0, 1));
	m_lbNameLeft->setScaleX(TEXT_NAME_SCALE_X);
	vecPos.x += AVA_SIZE / scaleFactor;
	m_lbNameLeft->setPosition(visibleOrigin + vecPos);
	m_lbNameLeft->enableWrap(true);
	auto maxLineWidth = visibleSize.width - vecPos.x - RIGHT_AVA_PADDING / scaleFactor;
	m_lbNameLeft->setMaxLineWidth(maxLineWidth);

	this->addChild(m_lbNameLeft, 2);
	m_lbNameLeft->setVisible(false);

	// ---- content -----
	m_txtLeft = Label::create("", FONT_FILE, TEXT_CONTENT_SIZE);
	m_txtLeft->setAnchorPoint(Vec2(0, 1));
	vecPos.y -= TEXT_NAME_HEIGHT / scaleFactor;
	m_txtLeft->setPosition(visibleOrigin + vecPos);
	m_txtLeft->enableWrap(true);
	m_txtLeft->setMaxLineWidth(maxLineWidth);

	this->addChild(m_txtLeft, 2);
	m_txtLeft->setVisible(false);



	///////////////////////////
	// 4. right

	// ----- ava --------
	m_avaRight = Sprite::create();
	m_avaRight->setAnchorPoint(Vec2(1, 1));
	vecPos = Vec2(visibleSize.width - RIGHT_AVA_PADDING / scaleFactor, visibleSize.height - RIGHT_AVA_PADDING / scaleFactor);
	m_avaRight->setPosition(vecPos + visibleOrigin);
	this->addChild(m_avaRight, 2);
	m_avaRight->setVisible(false);

	// ---- name --------
	m_lbNameRight = Label::create("", FONT_FILE, TEXT_NAME_SIZE);
	m_lbNameRight->setAnchorPoint(Vec2(1, 1));
	m_lbNameRight->setScaleX(TEXT_NAME_SCALE_X);
	vecPos.x -= AVA_SIZE / scaleFactor;
	m_lbNameRight->setPosition(visibleOrigin + vecPos);
	m_lbNameRight->enableWrap(true);
	m_lbNameRight->setMaxLineWidth(maxLineWidth);

	this->addChild(m_lbNameRight, 2);
	m_lbNameRight->setVisible(false);

	// ---- content -----
	m_txtRight = Label::create("", FONT_FILE, TEXT_CONTENT_SIZE);
	m_txtRight->setAnchorPoint(Vec2(1, 1));
	vecPos.y -= TEXT_NAME_HEIGHT / scaleFactor;
	m_txtRight->setPosition(visibleOrigin + vecPos);
	m_txtRight->enableWrap(true);
	m_txtRight->setMaxLineWidth(maxLineWidth);

	this->addChild(m_txtRight, 2);
	m_txtRight->setVisible(false);



	///////////////////////////
	// 5. event
	addTouchListener();

	return true;
}

void Speecher::update(float delta)
{	
	m_duration -= delta;
	if (m_duration <= 0)
	{
		next();
	}
}


void Speecher::hideSpeecher()
{	
	PlayScene::resumeRecursive(World::getCurrent());
	Director::getInstance()->getRunningScene()->getPhysicsWorld()->setSpeed(1.0f);
	((PlayScene*)this->getParent())->getHUD()->unhideHUDLayer();

	hideComponent();
}

void Speecher::showSpeecher()
{
	PlayScene::pauseRecursive(World::getCurrent());
	Director::getInstance()->getRunningScene()->getPhysicsWorld()->setSpeed(0.0f);
	((PlayScene*)this->getParent())->getHUD()->hideHUDLayer();

	showComponent();
}

void Speecher::showComponent()
{
	m_camera = Camera::getDefaultCamera();

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




	this->next();
	this->scheduleUpdate();
	//_eventDispatcher->resumeEventListenersForTarget(this);
}

void Speecher::hideComponent()
{
	m_lbNameRight->setVisible(false);
	m_avaRight->setVisible(false);
	m_txtRight->setVisible(false);

	m_lbNameLeft->setVisible(false);
	m_avaLeft->setVisible(false);
	m_txtLeft->setVisible(false);

	m_background->setVisible(false);

	this->unscheduleUpdate();
	//_eventDispatcher->pauseEventListenersForTarget(this);
}

void Speecher::activeSpeech(const std::string& fileName)
{
	ResourceManager* resManager = ResourceManager::getInstance();
	if (!resManager->isSpeechDatasContainKey(fileName))
	{
		return;
	}

	m_currentModels = resManager->speechDatas[fileName];
	m_currentIter = m_currentModels->begin();
	showSpeecher();
}

void Speecher::loadModels()
{
	
}

void Speecher::addTouchListener()
{
	EventListenerTouchOneByOne* listener = EventListenerTouchOneByOne::create();
	listener->setSwallowTouches(true);
	listener->onTouchBegan = CC_CALLBACK_2(Speecher::onTouch, this);
	//_eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);
	//_eventDispatcher->pauseEventListenersForTarget(this);
}

bool Speecher::onTouch(cocos2d::Touch *touch, cocos2d::Event *event)
{
	next();
	return true;// swallow
}

void Speecher::next()
{
	if (m_currentModels == nullptr)//safe
	{
		return;
	}


	SpeechModel* model = (*m_currentIter);

	std::string imgName = model->pathImg;
	if (imgName == "PLAYER")
	{
		auto playerType = World::getCurrent()->getPlayer()->getCharType();
		switch (playerType)
		{
		case Character::CHARACTER_TYPE::RED_DINOSAUR:
			imgName = "tyro_status_normal.png";
			break;
		case Character::CHARACTER_TYPE::WHITE_DINOSAUR:
			imgName = "trino_status_normal.png";
			break;
		case Character::CHARACTER_TYPE::YELLOW_DINOSAUR:
			imgName = "ptero_status_normal.png";
			break;
		default:
			log("ERROR: INVALID CHARACTER TYPE");
			break;
		}
	}
	if (model->isLeft) // is left?
	{
		m_lbNameLeft->setString(model->name);
		m_avaLeft->setTexture(ResourceManager::getInstance()->getSprite(imgName)->getTexture());
		m_txtLeft->setString(model->text);
	}
	else // or right
	{
		m_lbNameRight->setString(model->name);
		m_avaRight->setTexture(ResourceManager::getInstance()->getSprite(imgName)->getTexture());
		m_txtRight->setString(model->text);
	}

	m_lbNameRight->setVisible(!model->isLeft);
	m_avaRight->setVisible(!model->isLeft);
	m_txtRight->setVisible(!model->isLeft);

	m_lbNameLeft->setVisible(model->isLeft);
	m_avaLeft->setVisible(model->isLeft);
	m_txtLeft->setVisible(model->isLeft);

	m_duration = model->duration;


	m_currentIter++;
	if (m_currentIter == m_currentModels->end())
	{
		m_duration += 10;// sure, it will never be iter++ before this speecher hide
		scheduleOnce([&](float delay) {
			this->hideSpeecher();
		}, model->duration, "hideSpeecher");
		return;
	}
}