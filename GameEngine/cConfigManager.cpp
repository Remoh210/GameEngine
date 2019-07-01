#include "cConfigManager.h"

#include <rapidjson/document.h>
#include <rapidjson/filereadstream.h>

cConfigManager::cConfigManager()
{
}

cConfigManager::~cConfigManager()
{
}

bool cConfigManager::loadConfig(std::string configFile)
{
	rapidjson::Document doc;
	std::string cofigFullPath = "config/" + configFile;
	FILE *fp = fopen(cofigFullPath.c_str(), "rb"); // non-Windows use "r"

	if(!fp) {
		return false;
	}
	char readBuffer[65536];
	rapidjson::FileReadStream is(fp, readBuffer, sizeof(readBuffer));
	doc.ParseStream(is);
	fclose(fp);
	rapidjson::Value Window(rapidjson::kObjectType);
	Window = doc["Window"];

	if (Window.HasMember("Width")) {
		mConfig.ScreenWidth = Window["Width"].GetInt();
	}
	else {
		mConfig.ScreenWidth = 800;
	}
	if (Window.HasMember("Height")) {
		mConfig.ScreenHeight = Window["Height"].GetInt();
	}
	else {
		mConfig.ScreenHeight = 600;
	}

	if (Window.HasMember("Title")) {
		mConfig.WindowTitle = Window["Title"].GetString();
	}
	else {
		mConfig.WindowTitle = "Title";
	}
	if (doc.HasMember("Scene")) {
		mConfig.SceneFileName = doc["Scene"].GetString();
	}
	else {
		return false;
	}
	if (doc.HasMember("Gravity")) {
		const rapidjson::Value &grArray = doc["Gravity"];
		for (int i = 0; i < 3; i++) {
			mConfig.Gravity[i] = grArray[i].GetFloat();
		}
	}
	else {
		mConfig.Gravity = glm::vec3(0.f, -10.f, 0.f);
	}

	if (Window.HasMember("FullScreen")) {
		mConfig.IsFullScreen = Window["FullScreen"].GetBool();
	}
	else {
		mConfig.IsFullScreen = false;
	}

	return true;
}

sConfig cConfigManager::getConfig()
{
	return mConfig;
}
