#pragma once
#include <glm\vec3.hpp>
#include <fmod\fmod.hpp>
#include <fmod\fmod_errors.h>
#include <string>
#include <vector>



class cSound
{
public:
	cSound()
		:bIsPaused(false)
	{
	}
	bool bIsPaused;
	glm::vec3 position;
	std::string name;
	std::string FileName;
	FMOD::Channel* fmodChannel;
	FMOD::Sound* fmodSound;

};



class cSoundManager 
{
public:
	cSoundManager();
	bool InitFmod();
	void loadSounds(std::string file);
	void playSound(std::string file);
	//void ProcessAllSounds();
	void Update();

private:
	std::vector<cSound*> vec_Sounds;
	FMOD_RESULT mResult;
	FMOD::System* mSystem;
	std::vector<FMOD::Sound*> mSounds;
	std::vector<FMOD::Channel*> mChannels;
	void mErrorCheck(FMOD_RESULT result);


	FMOD_VECTOR _listener_position;
	FMOD_VECTOR _forward;
	FMOD_VECTOR _up;
	// Channel groups
	FMOD::ChannelGroup* mGroups[5];
	FMOD::ChannelGroup* mMasterGroup;

};