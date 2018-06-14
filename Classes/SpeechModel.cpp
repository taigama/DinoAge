#include "SpeechModel.h"

//std::stringstream SpeechModel::processStream;
//std::stringstream SpeechModel::tmpStream;
std::string SpeechModel::tmpString;
//int SpeechModel::tmpI = 0;

SpeechModel::SpeechModel(std::string& input)
{
	std::stringstream processStream(input);

	getline(processStream, tmpString, ';');
	this->pathImg = tmpString;

	getline(processStream, tmpString, ';');
	this->name = tmpString;

	getline(processStream, tmpString, ';');
	this->text = tmpString;

	getline(processStream, tmpString, ';');
	this->duration = strtod(tmpString.c_str(), nullptr);// ndk 15 not supported stof
	//tmpStream.str(tmpString);
	//tmpStream >> this->duration;

	getline(processStream, tmpString, '\r');
	this->isLeft = strtol(tmpString.c_str(), nullptr, 10);// ndk 15 not supported stoi
	//tmpStream.str(tmpString);
	//tmpStream >> tmpI;
	//this->isLeft = tmpI;
}