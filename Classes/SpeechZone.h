#ifndef __SPEECH_ZONE_H__
#define __SPEECH_ZONE_H__

#include "Zone.h"

// Zone that is, when player is standing in this zone for a while, moving player to the destination
class SpeechZone : public Zone
{
public:
	virtual bool init() override;

	CREATE_FUNC(SpeechZone);


	virtual void message(std::string your_message) override;

protected:
	
	// when player enter the zone
	virtual void onPlayerEntered(Character*) override;

	// zone no need to put on pool manager
	//virtual void reset(float params[10]) override;
private:

	// name of the speech file that you will load next
	std::string m_fileName;
};
#endif // !__SPEECH_ZONE_H__
