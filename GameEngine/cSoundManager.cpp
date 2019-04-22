#include "cSoundManager.h"


#include "globalStuff.h"
#include <cstdio>
#include <fstream>
#include <iostream>


#include <rapidjson/document.h>
#include <rapidjson/filereadstream.h>
#include <rapidjson/filewritestream.h>
#include <rapidjson/writer.h>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/prettywriter.h>

#include <rapidjson/writer.h>
#include <cstdio>

cSoundManager::cSoundManager()
{
}

bool cSoundManager::InitFmod()
{
	//Create fmod system
	this->mResult = FMOD::System_Create(&this->mSystem);
	if (this->mResult != FMOD_OK)
	{
		fprintf(stderr, "FMOD error! (%d) %s\n", this->mResult, FMOD_ErrorString(this->mResult));
		system("pause");
		exit(-1);
	}
	//Initialize fmod system
	this->mResult = mSystem->init(512, FMOD_INIT_NORMAL, 0);    // Initialize FMOD.
	if (this->mResult != FMOD_OK)
	{
		fprintf(stderr, "FMOD error! (%d) %s\n", this->mResult, FMOD_ErrorString(this->mResult));
		system("pause");
		exit(-1);
	}
	fprintf(stderr, "FMOD Init OK!\n");
	return true;
}

void cSoundManager::loadSounds(std::string file)
{


	//std::string fileToLoadFullPath = this->m_basePath + "/" + filename;

	//vec_pObjectsToDraw.clear();
	//LightManager->vecLights.clear();
	::g_pTheTextureManager->SetBasePath("assets/textures");
	rapidjson::Document doc;
	FILE* fp = fopen("scenes/sound.json", "rb"); 
	char readBuffer[65536];
	rapidjson::FileReadStream is(fp, readBuffer, sizeof(readBuffer));
	doc.ParseStream(is);
	fclose(fp);


	const rapidjson::Value& Sounds = doc["Sounds"];


	for (rapidjson::SizeType i = 0; i < Sounds.Size(); i++)
	{
		cSound* curSound = new cSound();
		curSound->name = Sounds[i]["Name"].GetString();
		curSound->FileName = Sounds[i]["FileName"].GetString();
		

		FMOD::Sound* fmSound = NULL;
		curSound->fmodSound = fmSound;
		mSounds.push_back(fmSound);
		FMOD::Channel* channel = NULL;
		curSound->fmodChannel = channel;
		this->mChannels.push_back(channel);

		if (Sounds[i]["Type"].GetString() == "3D")
		{
			const rapidjson::Value& PositionArray = Sounds[i]["Position"];
			for (rapidjson::SizeType i = 0; i < PositionArray.Size(); i++) {

				curSound->position[i] = PositionArray[i].GetFloat();

			}

			FMOD_VECTOR SoundPos;
			SoundPos.x = curSound->position.x;
			SoundPos.y = curSound->position.y;
			SoundPos.z = curSound->position.z;
			FMOD_VECTOR sound_velocity = { 0.0f, 0.0f, 0.0f };



			mResult = mSystem->createSound(curSound->FileName.c_str(), FMOD_3D, 0, &mSounds[i]);
			mErrorCheck(mResult);
			mResult = mSounds[i]->setMode(FMOD_CREATESTREAM);
			mErrorCheck(mResult);

			mResult = mSounds[i]->set3DMinMaxDistance(0.2f, 5000.0f);


			mResult = mSystem->playSound(mSounds[i], 0, false, &mChannels[i]);
			mErrorCheck(mResult);
			mResult = mChannels[i]->set3DAttributes(&SoundPos, &sound_velocity);
			mErrorCheck(mResult);
			mResult = mChannels[i]->setPaused(false);
			mErrorCheck(mResult);
			mResult = mChannels[i]->setVolume(1.0f);
			mErrorCheck(mResult);
			mErrorCheck(mResult);
		}
		else
		{
			if (Sounds[i]["Loop"].GetBool())
			{
				mResult = mSystem->createSound(curSound->FileName.c_str(), FMOD_LOOP_NORMAL, 0, &mSounds[i]);//FMOD_DEFAULT FMOD_LOOP_NORMAL
			}
			else
			{
				mResult = mSystem->createSound(curSound->FileName.c_str(), FMOD_DEFAULT, 0, &mSounds[i]);//FMOD_DEFAULT FMOD_LOOP_NORMAL
			}
			
			mErrorCheck(mResult);
			mResult = mSounds[i]->setMode(FMOD_CREATESTREAM);
			mErrorCheck(mResult);

			if (Sounds[i]["PlayFromStart"].GetBool())
			{
				mResult = mSystem->playSound(mSounds[i], 0, false, &mChannels[i]);
				mErrorCheck(mResult);
			}
			else
			{
				mResult = mSystem->playSound(mSounds[i], 0, true, &mChannels[i]);
				mErrorCheck(mResult);
			}


			// Set volume
			float vol = Sounds[i]["Volume"].GetInt() / 100.0f;
			mResult = mChannels[i]->setVolume(vol);
			mErrorCheck(mResult);

			// Set mute
			//mResult = mchannels[i]->setMute(this->soundObjects[i]->getMute());
			//errorcheck(mresult);
		}

		vec_Sounds.push_back(curSound);

	}

}

void cSoundManager::playSound(std::string name)
{
	for (int i = 0; i < vec_Sounds.size(); i++)
	{
		if (vec_Sounds[i]->name == name)
		{
			mResult = mSystem->playSound(mSounds[i], 0, false, &mChannels[i]);
			return;
		}
	}
	std::cout << "no sound " << name << " was fond" << std::endl;
}

void cSoundManager::Update()
{
	FMOD_VECTOR Listener;
	FMOD_VECTOR Velocity;
	FMOD_VECTOR Up;
	

	Up.x = 0.0f;
	Up.y = 1.0f;
	Up.z = 0.0f;


	Velocity.x = 0.0f;
	Velocity.y = 0.0f;
	Velocity.z = 0.0f;
	


	//_listener_position = { camera.Position.x, camera.Position.y, camera.Position.z };
	//glm::vec3 horizon = glm::normalize(glm::cross(camera.Front, glm::vec3(0.0f, 1.0f, 0.0f)));
	//glm::vec3 newup = glm::normalize(glm::cross(horizon, camera.Front));
	//_up = { newup.x, newup.y, newup.z };
	//_forward = { camera.getDirectionVector().x, camera.getDirectionVector().y, camera.getDirectionVector().z };
	//mResult = mSystem->set3DListenerAttributes(0, &_listener_position, &Velocity, &_forward, &_up);
	//mErrorCheck(mResult);


}

void cSoundManager::mErrorCheck(FMOD_RESULT result)
{
	if (result != FMOD_OK)
	{
		fprintf(stderr, "FMOD error! (%d) %s\n", result, FMOD_ErrorString(result));
		system("pause");
		exit(-1);
	}
}



