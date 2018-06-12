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
#define LEFT_AVA_PADDING 20// by pixel
#define RIGHT_AVA_PADDING 20// by pixel

#define AVA_SIZE 80// by pixel


//
#define TEXT_NAME_SCALE_X 1.1f
#define TEXT_CONTENT_SCALE_X 1.0f

#define TEXT_NAME_SIZE 18
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
	m_resManager = ResourceManager::getInstance();

	m_visibleSize = _director->getVisibleSize();
	m_visibleOrigin = _director->getVisibleOrigin();
	auto scaleFactor = _director->getContentScaleFactor();

	///////////////////////////
	// 2. Back ground
	auto paddingBackground = LEFT_AVA_PADDING / 2.0f / scaleFactor;
	m_background = LayerColor::create(Color4B::BLACK);
	m_background->setOpacity(200);
	m_background->setContentSize(Size(m_visibleSize.width - paddingBackground * 2, (BACKGROUND_HEIGHT / 100.0f) *m_visibleSize.height));
	m_background->setPosition(Vec2(m_visibleOrigin.x + paddingBackground, m_visibleSize.height + m_visibleOrigin.y - (BACKGROUND_HEIGHT / 100.0f) *m_visibleSize.height - paddingBackground));
	this->addChild(m_background, 1);
	m_background->setVisible(false);

	///////////////////////////
	// 3. left

	// ----- ava --------
	m_avaLeft = Sprite::create("fire_voxtex_1.png");
	auto contentAvaSize = m_avaLeft->getContentSize();
	auto scaleAvaX = (AVA_SIZE / scaleFactor) / contentAvaSize.width;
	auto scaleAvaY = (AVA_SIZE / scaleFactor) / contentAvaSize.height;
	m_avaLeft->setScale(scaleAvaX, scaleAvaX);
	//m_avaLeft->setContentSize(Size(AVA_SIZE / scaleFactor, AVA_SIZE / scaleFactor));
	m_avaLeft->setAnchorPoint(Vec2(0, 1));
	Vec2 vecPos = Vec2(LEFT_AVA_PADDING / scaleFactor, m_visibleSize.height - LEFT_AVA_PADDING / scaleFactor);
	m_avaLeft->setPosition(vecPos + m_visibleOrigin);
	this->addChild(m_avaLeft, 2);
	m_avaLeft->setVisible(false);

	// ---- name --------
	m_lbNameLeft = Label::create("", FONT_FILE, TEXT_NAME_SIZE);
	m_lbNameLeft->setAnchorPoint(Vec2(0, 1));
	m_lbNameLeft->setScaleX(TEXT_NAME_SCALE_X);
	m_lbNameLeft->setTextColor(Color4B::BLACK);
	m_lbNameLeft->enableOutline(Color4B::WHITE, 2);
	vecPos.x += AVA_SIZE / scaleFactor + LEFT_AVA_PADDING / scaleFactor;
	m_lbNameLeft->setPosition(m_visibleOrigin + vecPos);
	m_lbNameLeft->enableWrap(true);
	auto maxLineWidth = m_visibleSize.width - vecPos.x - RIGHT_AVA_PADDING / scaleFactor;
	m_lbNameLeft->setMaxLineWidth(maxLineWidth);

	this->addChild(m_lbNameLeft, 2);
	m_lbNameLeft->setVisible(false);

	// ---- content -----
	m_txtLeft = Label::create("", FONT_FILE, TEXT_CONTENT_SIZE);
	m_txtLeft->setAnchorPoint(Vec2(0, 1));
	m_txtLeft->setScaleX(TEXT_CONTENT_SCALE_X);
	vecPos.y -= TEXT_NAME_HEIGHT / scaleFactor;
	m_txtLeft->setPosition(m_visibleOrigin + vecPos);
	m_txtLeft->enableWrap(true);
	m_txtLeft->setMaxLineWidth(maxLineWidth);

	this->addChild(m_txtLeft, 2);
	m_txtLeft->setVisible(false);



	///////////////////////////
	// 4. right

	// ----- ava --------
	m_avaRight = Sprite::create("fire_voxtex_1.png");
	m_avaRight->setScale(scaleAvaX, scaleAvaX);
	//m_avaRight->setContentSize(Size(AVA_SIZE / scaleFactor, AVA_SIZE / scaleFactor));
	m_avaRight->setAnchorPoint(Vec2(1, 1));
	vecPos = Vec2(m_visibleSize.width - RIGHT_AVA_PADDING / scaleFactor, m_visibleSize.height - RIGHT_AVA_PADDING / scaleFactor);
	m_avaRight->setPosition(vecPos + m_visibleOrigin);
	this->addChild(m_avaRight, 2);
	m_avaRight->setVisible(false);

	// ---- name --------
	m_lbNameRight = Label::create("", FONT_FILE, TEXT_NAME_SIZE);
	m_lbNameRight->setAnchorPoint(Vec2(1, 1));
	m_lbNameRight->setScaleX(TEXT_NAME_SCALE_X);
	m_lbNameRight->setTextColor(Color4B::BLACK);
	m_lbNameRight->enableOutline(Color4B::WHITE, 2);
	vecPos.x -= AVA_SIZE / scaleFactor + RIGHT_AVA_PADDING / scaleFactor;
	m_lbNameRight->setPosition(m_visibleOrigin + vecPos);
	m_lbNameRight->enableWrap(true);
	m_lbNameRight->setMaxLineWidth(maxLineWidth);

	this->addChild(m_lbNameRight, 2);
	m_lbNameRight->setVisible(false);

	// ---- content -----
	m_txtRight = Label::create("", FONT_FILE, TEXT_CONTENT_SIZE);
	m_txtRight->setAnchorPoint(Vec2(1, 1));
	m_txtRight->setScaleX(TEXT_CONTENT_SCALE_X);
	vecPos.y -= TEXT_NAME_HEIGHT / scaleFactor;
	m_txtRight->setPosition(m_visibleOrigin + vecPos);
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
	// Calculate the "new" origin position for HUD layer in respect of the Camera's position
	auto myPos = m_camera->getPosition() - m_visibleOrigin;
	myPos.x -= m_visibleSize.width / 2;
	myPos.y -= m_visibleSize.height / 2;
	// Sets the position of HUD layer
	this->setPosition(myPos);

	m_duration -= delta;
	if (m_duration <= 0)
	{
		if (m_currentIter == m_currentModels->end())
		{
			scheduleOnce([&](float delay) {
				this->hideSpeecher();
			}, 0, "hideSpeecher");
			return;
		}

		next();
	}
}


void Speecher::hideSpeecher()
{	
	//PlayScene::resumeRecursive(World::getCurrent());
	//_director->getRunningScene()->getPhysicsWorld()->setSpeed(1.0f);
	((PlayScene*)this->getParent())->getHUD()->unhideHUDLayer();

	hideComponent();
}

void Speecher::showSpeecher()
{
	//PlayScene::pauseRecursive(World::getCurrent());
	//_director->getRunningScene()->getPhysicsWorld()->setSpeed(0.0f);
	((PlayScene*)this->getParent())->getHUD()->hideHUDLayer();

	showComponent();
}

void Speecher::showComponent()
{
	m_camera = Camera::getDefaultCamera();

	m_background->setVisible(true);






	((PlayScene*)this->getParent())->getHUD()->pauseAllEventListener();
	m_listener->setEnabled(true);

	this->scheduleUpdate();
	this->next();
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

	((PlayScene*)this->getParent())->getHUD()->resumeAllEventListener();
	m_listener->setEnabled(false);
}

void Speecher::activeSpeech(const std::string& fileName)
{
	if (!m_resManager->isSpeechDatasContainKey(fileName))
	{
		return;
	}

	m_currentModels = m_resManager->speechDatas[fileName];
	m_currentIter = m_currentModels->begin();
	showSpeecher();
}

void Speecher::loadModels()
{
	
}

void Speecher::addTouchListener()
{
	m_listener = EventListenerTouchOneByOne::create();
	m_listener->setSwallowTouches(true);
	m_listener->onTouchBegan = CC_CALLBACK_2(Speecher::onTouch, this);
	_eventDispatcher->addEventListenerWithSceneGraphPriority(m_listener, this);
	m_listener->setEnabled(false);
}

bool Speecher::onTouch(cocos2d::Touch *touch, cocos2d::Event *event)
{
	if (m_currentIter == m_currentModels->end())
	{
		scheduleOnce([&](float delay) {
			this->hideSpeecher();
		}, 0, "hideSpeecher");
		return true;
	}

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

	Sprite* currentAva;
	if (model->isLeft) // is left?
	{
		m_lbNameLeft->setString(model->name);
		currentAva = m_avaLeft;
		m_txtLeft->setString(model->text);
	}
	else // or right
	{
		m_lbNameRight->setString(model->name);
		currentAva = m_avaRight;
		m_txtRight->setString(model->text);
	}


	currentAva->setSpriteFrame(m_resManager->getSprite(imgName)->getSpriteFrame());
	auto contentAvaSize = currentAva->getContentSize();
	auto scaleFactor = _director->getContentScaleFactor();
	auto scaleAvaX = (AVA_SIZE / scaleFactor) / contentAvaSize.width;
	auto scaleAvaY = (AVA_SIZE / scaleFactor) / contentAvaSize.height;
	currentAva->setScale(scaleAvaX, scaleAvaY);

	m_lbNameRight->setVisible(!model->isLeft);
	m_avaRight->setVisible(!model->isLeft);
	m_txtRight->setVisible(!model->isLeft);

	m_lbNameLeft->setVisible(model->isLeft);
	m_avaLeft->setVisible(model->isLeft);
	m_txtLeft->setVisible(model->isLeft);

	m_duration = model->duration;


	m_currentIter++;
}