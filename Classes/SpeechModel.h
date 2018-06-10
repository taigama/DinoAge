#ifndef __SPEECH_MODEL_H__
#define __SPEECH_MODEL_H__

#include "cocos2d.h"


class SpeechModel
{
public:
	std::string pathImg;
	std::string text;
	float duration;
	bool isLeft;

	SpeechModel(std::string& input);
};

#endif // !__SPEECH_MODEL_H__
