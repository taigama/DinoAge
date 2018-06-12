#include "IntroScene.h"

#include "MenuScene1.h"
#include "ResourceManager.h"


#define SIZE_MARGIN 10.0f

#define FONT_SIZE 30.0f

#define SKIP_BUTTON_SCALE 0.8f
#define SKIP_BUTTON_PADDING 20.0f





#define FADE_TIME 2.0f

#define DELAY_STORY 10.0f


//-------------
//#define FRAME1_SCALE 0.6f
//#define FRAME1_OFFSET_Y -20.0f

#define MAX_FRAME 12









USING_NS_CC;

IntroScene::~IntroScene()
{
}

Scene* IntroScene::createScene()
{
	auto scene = Scene::create();

	auto layer = IntroScene::create();

	scene->addChild(layer);

	return scene;
}

bool IntroScene::init()
{
	//////////////////////////
	// 1. Super init first
	if (!Layer::init())
	{
		return false;
	}

	m_visibleSize = Director::getInstance()->getVisibleSize();
	m_origin = Director::getInstance()->getVisibleOrigin();

	m_center = m_origin + Vec2(m_visibleSize.width / 2, m_visibleSize.height / 2);


	Sprite* background = Sprite::create("black.png");
	background->setPosition(m_center);
	background->setScaleX(m_visibleSize.width + SIZE_MARGIN);
	background->setScaleY(m_visibleSize.height + SIZE_MARGIN);
	background->setName("LoadingBackground");
	this->addChild(background, -1);

	Sprite* logo = Sprite::create("game-logo.jpg");
	logo->setPosition(m_center);
	logo->setName("logo");
	this->addChild(logo, 0);

	m_scaleFactor = Director::getInstance()->getContentScaleFactor();

	Label* txtLoading = Label::createWithTTF("Loading", "ArcadeClassic.ttf", FONT_SIZE / m_scaleFactor);
	txtLoading->setPosition(m_center);
	txtLoading->setName("LoadingText");
	this->addChild(txtLoading, -2);





	// ---------------------------

	scheduleOnce([this](float delay) {
		this->loading();
	}, 0.0f, "start_loading");


	return true;
}

int counter = 0;
bool IntroScene::loading()
{
	////////////////////////////////
	// 1. Load resources
	auto resourceManager = ResourceManager::getInstance();
	resourceManager->loadSprites();

	resourceManager->loadSounds();


	///////////////////////////////
	// 2. Buttons

	auto rightPadding = m_visibleSize.width - SKIP_BUTTON_PADDING / m_scaleFactor;
	auto topPadding = m_visibleSize.height - SKIP_BUTTON_PADDING / m_scaleFactor;

	// -------------- Skip button ---------------

	auto skipButton = ui::Button::create("skip.png");

	skipButton->setName("skipButton");
	skipButton->setAnchorPoint(Vec2(1, 1));
	skipButton->setScale(SKIP_BUTTON_SCALE);
	skipButton->setPosition(m_origin + Vec2(rightPadding, topPadding));
	skipButton->addTouchEventListener(CC_CALLBACK_2(IntroScene::onSkipCallback, this));

	this->addChild(skipButton, 20);
	Size s = skipButton->getBoundingBox().size;

	auto skipLabel = Label::createWithTTF("skip", "ArcadeClassic.ttf", FONT_SIZE / m_scaleFactor);
	skipLabel->setPosition(s.width / 2.0f, s.height / 2.0f);
	skipLabel->setOpacity(0);
	skipButton->addChild(skipLabel);
	skipLabel->runAction(RepeatForever::create(Sequence::createWithTwoActions(
		FadeIn::create(FADE_TIME)
		, FadeOut::create(FADE_TIME)
	)));

	//this->removeChild(m_curBackground);
	//this->removeChildByName("LoadingText");

	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// 3. animations:

	char spriteName[15];
	Sprite* sprFrm;	
	Sequence* seq;

	for (int i = 1; i <= MAX_FRAME; i++)
	{
		sprintf(spriteName, "frame %d.jpg", i);
		sprFrm = Sprite::create(spriteName);
		sprFrm->setPosition(m_origin + m_center);
		sprFrm->setOpacity(0);
		this->addChild(sprFrm, i);

		seq = Sequence::createWithTwoActions(DelayTime::create((i - 1) * DELAY_STORY), FadeIn::create(FADE_TIME));
		sprFrm->runAction(seq);
	}


	////-------------------------------------------------------------------------------

	//auto sprFrm1 = Sprite::create("frame1.jpg");
	//sprFrm1->setPosition(m_origin + m_center + Vec2(0, FRAME1_OFFSET_Y));
	//sprFrm1->setScale(FRAME1_SCALE);
	//sprFrm1->setOpacity(0);

	//this->addChild(sprFrm1, 0);
	//sprFrm1->runAction(m_fadeIn->clone());
	m_soundID = AUDIO::INVALID_AUDIO_ID;
#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)
	m_soundID = AUDIO::play2d("digimon-rumble-arena_mid_stress.mp3");
#else// for android
	m_soundID = AUDIO::play2d("digimon-rumble-arena_mid_stress.ogg");
#endif


	scheduleOnce([this](float delay) {
		this->changeScene();
	}, ((MAX_FRAME - 1) * DELAY_STORY) + FADE_TIME, "end_intro0");

	// set the volume down
	int soundID = m_soundID;
	scheduleOnce([soundID, this](float delay) {
		AUDIO::setVolume(soundID, 0.6f);

		this->getChildByName("skipButton")->runAction(FadeOut::create(FADE_TIME));
	}, ((MAX_FRAME - 1) * DELAY_STORY), "intro_volume_down0");

	scheduleOnce([soundID](float delay) {
		AUDIO::setVolume(soundID, 0.3f);
	}, ((MAX_FRAME - 1) * DELAY_STORY) + FADE_TIME / 2.0f, "intro_volume_down1");


	return true;
}

void IntroScene::onSkipCallback(Ref* sender, cocos2d::ui::Widget::TouchEventType type)
{
	// Changing scene only if user release ON button
	if (type != ui::Widget::TouchEventType::ENDED)
	{
		return;
	}

	AUDIO::setVolume(m_soundID, 0.6f);

	int soundID = m_soundID;
	scheduleOnce([soundID](float delay) {
		AUDIO::setVolume(soundID, 0.3f);
	}, FADE_TIME / 2.0f, "intro_volume_down2");

	scheduleOnce([this](float delay) {
		changeScene();
	}, FADE_TIME, "end_intro1");
	

	Sprite* sprFrm = Sprite::create("frame 12.jpg");
	sprFrm->setPosition(m_origin + m_center);
	sprFrm->setOpacity(0);
	this->addChild(sprFrm, 12);
	sprFrm->runAction(FadeIn::create(FADE_TIME));

	this->getChildByName("skipButton")->stopAllActions();
	this->getChildByName("skipButton")->runAction(FadeOut::create(FADE_TIME));
}

void IntroScene::changeScene()
{
	AUDIO::stop(m_soundID);

	// change to menu scene	
	Scene* menuScene1 = MenuScene1::createScene();
	Director::getInstance()->replaceScene(menuScene1);

}