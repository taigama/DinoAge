#ifndef __SPEECH_MODEL_H__
#define __SPEECH_MODEL_H__

#include "cocos2d.h"


class SpeechModel
{
public:
	static std::stringstream processStream;
	//static std::stringstream tmpStream;
	static std::string tmpString;
	//static int tmpI;

	std::string pathImg;
	std::string name;
	std::string text;
	float duration;
	bool isLeft;

	SpeechModel(std::string& input);
};

#endif // !__SPEECH_MODEL_H__
