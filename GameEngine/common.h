#pragma once
#include "globalOpenGLStuff.h"
#include "globalStuff.h"

#include "Camera.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/mat4x4.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <iostream>
#include <rapidjson/document.h>
#include <rapidjson/filereadstream.h>
#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include "BehaviourManager.h"
#include "WanderBehaviour.h"

#define GLM_ENABLE_EXPERIMENTAL
#include <Interfaces/iRigidBody.h>
#include <glm/gtx/norm.hpp>
#include <glm/gtx/quaternion.hpp>

#include "cGameObject.h"
#include "cShaderManager.h"
#include "cSoundManager.h"
#include "cConfigManager.h"
#include "cVAOMeshManager.h"
#include <algorithm>
#include <windows.h>

#include "DebugRenderer/cDebugRenderer.h"
#include "cLightHelper.h"