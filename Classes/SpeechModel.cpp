#include "SpeechModel.h"

SpeechModel::SpeechModel(std::string& input)
{
	std::stringstream ss(input);
	std::string substr;

	getline(ss, substr, ';');
	this->pathImg = substr;

	getline(ss, substr, ';');
	this->name = substr;

	getline(ss, substr, ';');
	this->text = substr;

	getline(ss, substr, ';');
	this->duration = std::stof(substr);

	getline(ss, substr);
	this->isLeft = std::stoi(substr);
}