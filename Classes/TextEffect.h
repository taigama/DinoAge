// abstract class for effect
// no collision

#ifndef __TEXT_EFFECT_H__
#define __TEXT_EFFECT_H__

#include "GameObject.h"

// for create string effect, auto destroy.
//* this is far different from Effect, which is managed my MyPoolManager
class TextEffect : public cocos2d::Label
{
public:
	enum TEXT_TYPE
	{
		DAMAGED,
		SHOUT,
		YELL,
		SPEAK,
		SPEAK_LOUD
	};

	enum PATH_TYPE
	{
		NONE,
		FLY_UP,
		THROW
	};


	TextEffect(cocos2d::TextHAlignment hAlignment = cocos2d::TextHAlignment::LEFT,
		cocos2d::TextVAlignment vAlignment = cocos2d::TextVAlignment::TOP);

	virtual ~TextEffect();

	virtual bool init();

	virtual void update(float) override;

	//CREATE_FUNC(TextEffect);

	
	/*static TextEffect* create()
	{
		TextEffect *pRet = new(std::nothrow) TextEffect();
			if (pRet && pRet->init())
			{
				pRet-> Ref::autorelease();
				return pRet;
			}
			else
			{
				delete pRet;
				pRet = nullptr;
				return nullptr;
			}
	}*/

	static TextEffect* createWithTTF(const std::string& text, const std::string& fontFile, float fontSize, const cocos2d::Size& dimensions = cocos2d::Size::ZERO , cocos2d::TextHAlignment hAlignment  = cocos2d::TextHAlignment::LEFT , cocos2d::TextVAlignment vAlignment  = cocos2d::TextVAlignment::TOP );

	
	float maxTime;


	/* method call by the pool manager
	* params[0], [1]: x,y : position
	* params[2] : TEXT_TYPE
	* params[3] : PATH_TYPE
	* params[4] : //repeat times
	* params[5] : duration
	* params[6, 7, 8] : color (red, green, blue)*/
	void reset(float params[10]);

private:
	float m_timer;

	static cocos2d::Vec2 m_deltaPos;
};

#endif // !__TEXT_EFFECT_H__
