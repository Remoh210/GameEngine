#pragma once

#include <string> 
#include <glm/vec3.hpp>

struct sConfig
{
	std::string WindowTitle;
	std::string SceneFileName;
	unsigned int ScreenWidth;
	unsigned int ScreenHeight;
	glm::vec3 Gravity;
	bool IsFullScreen;

};

class cConfigManager
{
public:
	cConfigManager();
	~cConfigManager();

	bool loadConfig(std::string configFile);
	sConfig getConfig();

private:
	sConfig mConfig;
};



