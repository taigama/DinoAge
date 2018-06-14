#include "AppDelegate.h"

#include "IntroScene.h"
//#include "MenuScene1.h"
#include "AudioEngine.h"

#include "ResourceManager.h"

USING_NS_CC;

//static cocos2d::Size designResolutionSize = cocos2d::Size(480, 320);	// 4:3 aspect ratio

//static cocos2d::Size designResolutionSize = cocos2d::Size(569, 320);	// 16:9 aspect ratio
//static cocos2d::Size designResolutionSize = cocos2d::Size(640, 360); Thinh	// 16:9 aspect ratio
static cocos2d::Size designResolutionSize = cocos2d::Size(853.3333f, 480.0f);// K	// 16:9 aspect ratio

static cocos2d::Size smallResolutionSize = cocos2d::Size(480, 320);
static cocos2d::Size mediumResolutionSize = cocos2d::Size(1024, 768);
static cocos2d::Size largeResolutionSize = cocos2d::Size(2048, 1536);

//static cocos2d::Size customResolutionSize = cocos2d::Size(1024, 576);	// 16:9 aspect ratio
static cocos2d::Size customResolutionSize = cocos2d::Size(1280, 720);// K recommended	// 16:9 aspect ratio
//static cocos2d::Size customResolutionSize = cocos2d::Size(1050, 450);	// 21:9 aspect ratio

AppDelegate::AppDelegate() {

}

AppDelegate::~AppDelegate() 
{
	//experimental::AudioEngine::end();
	//delete ResourceManager::getInstance();
}

//if you want a different context,just modify the value of glContextAttrs
//it will takes effect on all platforms
void AppDelegate::initGLContextAttrs()
{
    //set OpenGL context attributions,now can only set six attributions:
    //red,green,blue,alpha,depth,stencil
    GLContextAttrs glContextAttrs = {8, 8, 8, 8, 24, 8};

    GLView::setGLContextAttrs(glContextAttrs);
}

// If you want to use packages manager to install more packages, 
// don't modify or remove this function
static int register_all_packages()
{
    return 0; //flag for packages manager
}

bool AppDelegate::applicationDidFinishLaunching()
{
    // initialize director
    auto director = Director::getInstance();

	// get open GL view
    auto glview = director->getOpenGLView();

	// if the director hasn't had open gl view yet
    if(!glview)
	{
		// create windows with thesse platforms
#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32) || (CC_TARGET_PLATFORM == CC_PLATFORM_MAC) || (CC_TARGET_PLATFORM == CC_PLATFORM_LINUX)
        glview = GLViewImpl::createWithRect("tkai9", Rect(0, 0, designResolutionSize.width, designResolutionSize.height));
		//glview = GLViewImpl::createWithRect("tkai9", Rect(0, 0, customResolutionSize.width, customResolutionSize.height));
#else
		// create full screen with the remain supported platforms
        glview = GLViewImpl::create("tkai9");
#endif
        director->setOpenGLView(glview);
    }

    // turn on display FPS
    director->setDisplayStats(true);

    // set FPS. the default value is 1.0/60 if you don't call this
    director->setAnimationInterval(1.0 / 60);

    // Set the design resolution
    //glview->setDesignResolutionSize(designResolutionSize.width, designResolutionSize.height, ResolutionPolicy::NO_BORDER);
	glview->setDesignResolutionSize(designResolutionSize.width, designResolutionSize.height, ResolutionPolicy::FIXED_HEIGHT);

    //Size frameSize = glview->getFrameSize();
    //// if the frame's height is larger than the height of medium size.
    //if (frameSize.height > mediumResolutionSize.height)
    //{        
    //    director->setContentScaleFactor(MIN(largeResolutionSize.height/designResolutionSize.height, largeResolutionSize.width/designResolutionSize.width));
    //}
    //// if the frame's height is larger than the height of small size.
    //else if (frameSize.height > smallResolutionSize.height)
    //{        
    //    director->setContentScaleFactor(MIN(mediumResolutionSize.height/designResolutionSize.height, mediumResolutionSize.width/designResolutionSize.width));
    //}
    //// if the frame's height is smaller than the height of medium size.
    //else
    //{        
    //    director->setContentScaleFactor(MIN(smallResolutionSize.height/designResolutionSize.height, smallResolutionSize.width/designResolutionSize.width));
    //}

    register_all_packages();


	// General search path for resources
	FileUtils::getInstance()->addSearchPath("res");

	// Search path for TiledMap(s)
	FileUtils::getInstance()->addSearchPath("res/tiledmap");

	// Search paths for Sprite(s)
	FileUtils::getInstance()->addSearchPath("res/sprites");
	FileUtils::getInstance()->addSearchPath("res/sprites/player");
	FileUtils::getInstance()->addSearchPath("res/sprites/enemy");
	FileUtils::getInstance()->addSearchPath("res/sprites/fires");
	FileUtils::getInstance()->addSearchPath("res/sprites/items");
	FileUtils::getInstance()->addSearchPath("res/sprites/ui-background");
	FileUtils::getInstance()->addSearchPath("res/sprites/skills-icons");
	FileUtils::getInstance()->addSearchPath("res/sprites/effects");
	FileUtils::getInstance()->addSearchPath("res/sprites/blocks");

	FileUtils::getInstance()->addSearchPath("res/sprites/intro");
	// Search path for Font(s)
	FileUtils::getInstance()->addSearchPath("fonts");


	// Search path for Sound(s)
	FileUtils::getInstance()->addSearchPath("res/sounds");
	FileUtils::getInstance()->addSearchPath("res/sounds/fx");
	FileUtils::getInstance()->addSearchPath("res/sounds/fx/creep");
	FileUtils::getInstance()->addSearchPath("res/sounds/fx/natural");
	FileUtils::getInstance()->addSearchPath("res/sounds/fx/strong");
	FileUtils::getInstance()->addSearchPath("res/sounds/fx/UI");

	// Search path for Speech(s)
	FileUtils::getInstance()->addSearchPath("res/speechs");
	

	// Keeps the scale...
	//director->setContentScaleFactor(1.0f);


	//auto audio = CocosDenshion::SimpleAudioEngine::getInstance();






	//// Sets initial values for CHARACTER'S SKILLS
	//UserDefault::getInstance()->setIntegerForKey("red_dino_skills_set", 31);			// value = 0x11000
	//UserDefault::getInstance()->setIntegerForKey("white_dino_skills_set", 31);			// value = 0x11000
	//UserDefault::getInstance()->setIntegerForKey("yellow_dino_skills_set", 31);			// value = 0x11000

	//// Sets initial values for PLAYER'S LEGACY
	//UserDefault::getInstance()->setIntegerForKey("player_legacy", 0);

	//UserDefault::getInstance()->flush();		// saves values


    // create a scene. it's an autorelease object
	//auto scene = MenuScene1::createScene();
	auto scene = IntroScene::createScene();

    // run
    director->runWithScene(scene);

    return true;
}

// This function will be called when the app is inactive. When comes a phone call,it's be invoked too
void AppDelegate::applicationDidEnterBackground() {
    Director::getInstance()->stopAnimation();

    // if you use SimpleAudioEngine, it must be pause
	//CocosDenshion::SimpleAudioEngine::getInstance()->pauseBackgroundMusic();
}

// this function will be called when the app is active again
void AppDelegate::applicationWillEnterForeground() {
    Director::getInstance()->startAnimation();

    // if you use SimpleAudioEngine, it must resume here
    //CocosDenshion::SimpleAudioEngine::getInstance()->resumeBackgroundMusic();
}
