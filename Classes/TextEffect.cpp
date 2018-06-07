#include "TextEffect.h"
#include "WorldScene.h"

USING_NS_CC;

#define BASE_MAX_TIME 1.0f
#define MOVE_UP_DISTANCE 50.0f

Vec2 TextEffect::m_deltaPos = Vec2(0, MOVE_UP_DISTANCE);

TextEffect::TextEffect(TextHAlignment hAlignment /* = TextHAlignment::LEFT */,
	TextVAlignment vAlignment /* = TextVAlignment::TOP */) : Label(hAlignment, vAlignment)
{
}

TextEffect::~TextEffect()
{
	
}

bool TextEffect::init()
{
	if (!Label::init())
		return false;
	
	return true;
}

TextEffect* TextEffect::createWithTTF(const std::string& text, const std::string& fontFile, float fontSize, const Size& dimensions /* = Size::ZERO */, TextHAlignment hAlignment /* = TextHAlignment::LEFT */, TextVAlignment vAlignment /* = TextVAlignment::TOP */)
{
	auto result = new (std::nothrow) TextEffect(hAlignment, vAlignment);

	if (result && result->initWithTTF(text, fontFile, fontSize, dimensions, hAlignment, vAlignment))
	{
		result-> Label::autorelease();
		return result;
	}

	CC_SAFE_DELETE(result);
	return nullptr;
}


void TextEffect::update(float delta)
{
	/*if (m_isTiming)
	{
		_timer -= delta;
		if (_timer <= 0)
		{
			this->setActive(false);
		}
	}*/

	m_timer -= delta;
	if (m_timer <= 0)
	{
		//this->setActive(false);
		// because of diamond inheritance
		World::getCurrent()->removeChild((Label*)this);
	}
}


/* method call by the pool manager
* params[0], [1]: x,y : position
* params[2] : TEXT_TYPE
* params[3] : PATH_TYPE
* params[4] : //repeat times
* params[5] : duration
* params[6, 7, 8, 9] : color (red, green, blue, alpha)*/
void TextEffect::reset(float params[10])
{
	//this->setActive(true);
	maxTime = params[5];
	m_timer = maxTime;
	this->Label::scheduleUpdate();

	this-> Label::setPosition(params[0], params[1]);

	TEXT_TYPE textType = (TEXT_TYPE)((int)params[2]);
	PATH_TYPE pathType = (PATH_TYPE)((int)params[3]);
	
	
	switch (pathType)
	{
	case TextEffect::NONE:
		break;
	case TextEffect::FLY_UP:
		this->Label::runAction(MoveBy::create(maxTime, m_deltaPos));
		break;
	case TextEffect::THROW:
		break;
	default:
		break;
	}

	this->Label::runAction(FadeOut::create(maxTime));

	this->setTextColor(Color4B(params[6], params[7], params[8], params[9]));

}