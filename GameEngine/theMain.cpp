//     ___                 ___ _
//    / _ \ _ __  ___ _ _ / __| |
//   | (_) | '_ \/ -_) ' \ (_ | |__
//    \___/| .__/\___|_||_\___|____|
//         |_|
//
#include "common.h"
#include "PostEffect.h"

// Dll
HINSTANCE hGetProckDll = 0;
// typedef nPhysics::iPhysicsFactory*(*f_createPhysicsFactory)();
ePhysics physics_library = UNKNOWN;

nPhysics::iPhysicsFactory *gPhysicsFactory = NULL;
nPhysics::iPhysicsWorld *gPhysicsWorld = NULL;

GLuint program;
cDebugRenderer *g_pDebugRendererACTUAL = NULL;
iDebugRenderer *g_pDebugRenderer = NULL;
cSimpleDebugRenderer *g_simpleDubugRenderer = NULL;
cSoundManager *g_pSoundManager = NULL;
cLuaBrain *p_LuaScripts = NULL;
cTextRend *g_textRenderer = NULL;
cCharacterManager *g_pCharacterManager = NULL;

// cCommandGroup sceneCommandGroup;
int cou;
int nbFrames = 0;
int FPS = 0;
int gFogDensity = 10.0f;
int gameCounter = 0;

double currentTime = 0;
double deltaTime = 0;
double FPS_last_Time = 0;
bool bIsDebugMode = false;

// for collision
float time = 0.0f;
bool collided = false;
bool collidedA = false;
bool collidedB = false;
bool collidedC = false;
bool collidedD = false;
// for collision
std::vector<cGameObject *> vec_pObjectsToDraw;
// for physics
std::vector<cGameObject *> vec_controlable;
unsigned int numberOfObjectsToDraw = 0;



unsigned int SCR_WIDTH = 1000;
unsigned int SCR_HEIGHT = 900;


Camera camera(glm::vec3(0.0f, 0.0f, 0.0f));

bool distToCam(cGameObject *leftObj, cGameObject *rightObj) {
  return glm::distance(leftObj->position, camera.Position) >
         glm::distance(rightObj->position,
                       camera.Position); // here go your sort conditions
}

std::vector<cGameObject *> vec_sorted_drawObj;

glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
glm::vec3 g_CameraEye = glm::vec3(0.0, 0.0, 250.0f);


cShaderManager *pTheShaderManager = NULL;
cVAOMeshManager *g_pTheVAOMeshManager = NULL;
cSceneManager *g_pSceneManager = NULL;
cLightManager *LightManager = NULL;

std::vector<cGameObject *> vec_transObj;
std::vector<cGameObject *> vec_non_transObj;

cBasicTextureManager *g_pTheTextureManager = NULL;

static void error_callback(int error, const char *description) {
  fprintf(stderr, "Error: %s\n", description);
}



// Set up the off screen textures to draw to
GLuint g_FBO = 0;
GLuint g_FBO_colourTexture = 0;
GLuint g_FBO_depthTexture = 0;
GLint g_FBO_SizeInPixes = 512; 




int main(void) {

  cConfigManager configManager;
  if(!configManager.loadConfig("config.json"))
  {
	  std::cout << "Can't load config..";
	  exit(EXIT_FAILURE);
  }


  sConfig Config = configManager.getConfig();
  GLFWwindow *window;

  glfwSetErrorCallback(error_callback);

  if (!glfwInit()) {
    exit(EXIT_FAILURE);
  }
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

  SCR_WIDTH = Config.ScreenWidth;
  SCR_HEIGHT = Config.ScreenHeight;
  if(Config.IsFullScreen)
  {
	  window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, Config.WindowTitle.c_str(), glfwGetPrimaryMonitor(), NULL);
  }
  else
  {
	  window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, Config.WindowTitle.c_str(), NULL, NULL);
  }
  
  if (!window) {
    glfwTerminate();
    exit(EXIT_FAILURE);
  }

  glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
  glfwSetCursorPosCallback(window, mouse_callback);
  glfwSetKeyCallback(window, key_callback);

  glfwMakeContextCurrent(window);
  gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
  glfwSwapInterval(1);

  pTheShaderManager = new cShaderManager();
  pTheShaderManager->setBasePath("assets/shaders/");

  cShaderManager::cShader vertexShader;
  cShaderManager::cShader fragmentShader;
  cShaderManager::cShader geometryShader;

  vertexShader.fileName = "vertex01.vert";
  vertexShader.shaderType = cShaderManager::cShader::VERTEX_SHADER;

  fragmentShader.fileName = "fragment01.frag";
  fragmentShader.shaderType = cShaderManager::cShader::FRAGMENT_SHADER;

  geometryShader.fileName = "geometry.glsl";
  geometryShader.shaderType = cShaderManager::cShader::GEOMETRY_SHADER;

  if (pTheShaderManager->createProgramFromFile(
          "BasicUberShader", 
			vertexShader,
			geometryShader,	
          fragmentShader)) { // Shaders are OK
    std::cout << "Compiled shaders OK." << std::endl;
  } else {
    std::cout << "OH NO! Compile error" << std::endl;
    std::cout << pTheShaderManager->getLastError() << std::endl;
  }

  // Load the uniform location values (some of them, anyway)
  cShaderManager::cShaderProgram *pSP =
      ::pTheShaderManager->pGetShaderProgramFromFriendlyName("BasicUberShader");

  program = pTheShaderManager->getIDFromFriendlyName("BasicUberShader");

  ::g_pTheVAOMeshManager = new cVAOMeshManager();
  ::g_pTheVAOMeshManager->SetBasePath("assets/models");
  ::g_pTheTextureManager = new cBasicTextureManager();
  ::g_textRenderer = new cTextRend();
  ::g_pSceneManager = new cSceneManager();
  ::g_pSoundManager = new cSoundManager();
  ::g_pSceneManager->setBasePath("scenes");
  ::LightManager = new cLightManager();





  // Loading the uniform variables here (rather than the inner draw loop)
  GLint objectColour_UniLoc = glGetUniformLocation(program, "objectColour");
  GLint matModel_location = glGetUniformLocation(program, "matModel");
  GLint matView_location = glGetUniformLocation(program, "matView");
  GLint matProj_location = glGetUniformLocation(program, "matProj");
  GLint eyeLocation_location = glGetUniformLocation(program, "eyeLocation");

  GLint fogDensity = glGetUniformLocation(program, "FogDensity");

  ::g_pDebugRendererACTUAL = new cDebugRenderer();
  ::g_pDebugRenderer = (iDebugRenderer *)::g_pDebugRendererACTUAL;

  if (!::g_pDebugRendererACTUAL->initialize()) {
    std::cout << "Warning: couldn't init the debug renderer." << std::endl;
    std::cout << "\t" << ::g_pDebugRendererACTUAL->getLastError() << std::endl;
  } else {
    std::cout << "Debug renderer is OK" << std::endl;
  }
  glm::mat4x4 matProjection = glm::mat4(1.0f);
  glm::mat4x4 matView = glm::mat4(1.0f);

  // Physics Initialization
  hGetProckDll = LoadLibraryA("BulletPhysics.dll");
  physics_library = BULLET;
  f_createPhysicsFactory CreatePhysicsFactory =
      (f_createPhysicsFactory)GetProcAddress(hGetProckDll, "CreateFactory");
  gPhysicsFactory = CreatePhysicsFactory();
  gPhysicsWorld = gPhysicsFactory->CreatePhysicsWorld();
  gPhysicsWorld->SetGravity(Config.Gravity);

  LoadSkinnedMeshModel(::vec_pObjectsToDraw, program);
  g_pSoundManager->InitFmod();

  LoadModelTypes(::g_pTheVAOMeshManager, program);
  ::g_pSceneManager->loadScene(Config.SceneFileName);
  ::LightManager->LoadUniformLocations(program);
  g_pSoundManager->loadSounds("scenes/sound.json");
  LoadModelsIntoScene(::vec_pObjectsToDraw);
  g_simpleDubugRenderer =
      new cSimpleDebugRenderer(findObjectByFriendlyName("DebugCapsule"),
                               findObjectByFriendlyName("DebugSphere"),
                               findObjectByFriendlyName("DebugCube"), program);
  g_pCharacterManager = new cCharacterManager(vec_pObjectsToDraw);
  g_pCharacterManager->setActiveChar("chan");



  double lastTime = glfwGetTime();

  cLightHelper *pLightHelper = new cLightHelper();

  // FBO
  int renderPassNumber = 1;
  GLint renderPassNumber_UniLoc =
      glGetUniformLocation(program, "renderPassNumber");

  cGameObject *player = g_pCharacterManager->getActiveChar();
  camera.setThirdPerson(player);
#pragma endregion
  using namespace PostEffect;
  ScreenQuad screen_quad(SCR_WIDTH, SCR_HEIGHT);

  const float bloom_downscale_factor = 0.35f;
  BloomEffect bloom_effect(screen_quad, bloom_downscale_factor);

  ChromAberrationEffect ChromAbEffect(screen_quad);


  RenderTarget scene_texture(SCR_WIDTH, SCR_HEIGHT);
  TextureRenderTarget bloom_result(SCR_WIDTH, SCR_HEIGHT);
  TextureRenderTarget ChromAberrationEffect(SCR_WIDTH, SCR_HEIGHT);




  float bloom_strength = 10.0f;
  int bloom_blur_iterations = 4;
  float bloom_threshold = 0.58f;



  for (int i = 0; i < vec_pObjectsToDraw.size(); i++) 
  {
	  cGameObject* go = vec_pObjectsToDraw[i];

	  go->InitPosition = go->position;

	  if (go->bSave)
	  {
		  vec_controlable.push_back(go);
	  }
	  
  }

#pragma region AI set up

  BehaviourManager* behavManager = new BehaviourManager();

  cGameObject* pPlayer = findObjectByFriendlyName("chan");
  cGameObject* pWanderEnemy1 = findObjectByFriendlyName("mutant-1");
  cGameObject* pWanderEnemy2 = findObjectByFriendlyName("mutant-2");
  cGameObject* pWanderEnemy3 = findObjectByFriendlyName("mutant-3");



  WanderBehaviour* wander1 = new WanderBehaviour(pWanderEnemy1, 20.2f, 400.2f, 4.0f, glm::vec3(0.0f), 100.0f, -100.0f, pPlayer); //(Agent, Target, maxSpeed, WanderOrigin , UpLimit, DownLimit)
  WanderBehaviour* wander2 = new WanderBehaviour(pWanderEnemy2, 20.2f, 400.2f, 4.0f, glm::vec3(0.0f), 100.0f, -100.0f, pPlayer); //(Agent, Target, maxSpeed, WanderOrigin , UpLimit, DownLimit)
  WanderBehaviour* wander3 = new WanderBehaviour(pWanderEnemy3, 20.2f, 400.2f, 4.0f, glm::vec3(0.0f), 100.0f, -100.0f, pPlayer); //(Agent, Target, maxSpeed, WanderOrigin , UpLimit, DownLimit)
  behavManager->SetBehaviour(pWanderEnemy1, wander1);
  behavManager->SetBehaviour(pWanderEnemy2, wander2);
  behavManager->SetBehaviour(pWanderEnemy3, wander3);






  cGameObject* boat = findObjectByFriendlyName("boat");

  while (!glfwWindowShouldClose(window)) {
	  ::pTheShaderManager->useShaderProgram("BasicUberShader");
      

      
     
#pragma region main render
	  
		 

		glUniform1f(fogDensity, gFogDensity/10000.0f);
		RenderTarget::State render_target_state(scene_texture);
        glViewport(0, 0, g_FBO_SizeInPixes, g_FBO_SizeInPixes);
        GLfloat zero = 0.0f;
        GLfloat one = 1.0f;
        glClearBufferfv(GL_COLOR, 0, &zero);
        glClearBufferfv(GL_DEPTH, 0, &one);
		float ratio;
		int width, height;

        matProjection = glm::mat4(1.0f);
        matView = glm::mat4(1.0f);

        glfwGetFramebufferSize(window, &width, &height);
        ratio = width / (float)height;
        glViewport(0, 0, width, height);

        glEnable(GL_DEPTH);      // Enables the KEEPING of the depth information
        glEnable(GL_DEPTH_TEST); // When drawing, checked the existing depth
        glEnable(GL_CULL_FACE);  // Discared "back facing" triangles

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


        matProjection = glm::perspective(1.0f,      // FOV
                                         ratio,     // Aspect ratio
                                         0.1f,      // Near clipping plane
                                         15000.0f); // Far clipping plane


		glUniform1f(renderPassNumber_UniLoc, 1.0f);


        matView = camera.GetViewMatrix();

        glUniform3f(eyeLocation_location, camera.Position.x, camera.Position.y,
                    camera.Position.z);


        glUniformMatrix4fv(matView_location, 1, GL_FALSE,
                           glm::value_ptr(matView));
        glUniformMatrix4fv(matProj_location, 1, GL_FALSE,
                           glm::value_ptr(matProjection));
        LightManager->CopyLightValuesToShader();

		
        DrawScene_Simple(vec_pObjectsToDraw, program, 1);
        


#pragma endregion
      
    
    // Post VFX
    {
      

	  bloom_effect.execute(bloom_result, scene_texture.color, bloom_strength, bloom_blur_iterations, bloom_threshold);
	  ChromAbEffect.execute(ChromAberrationEffect, bloom_result.texture);
      
 
 
	  //Draw main screen quad
      screen_quad.draw_with_texture(ChromAberrationEffect.texture);
    }
    


#pragma region text_rendering



        double FPS_currentTime = glfwGetTime();
        nbFrames++;
        if (FPS_currentTime - FPS_last_Time >=
            1.0) 
		{ 
          FPS = nbFrames * 1;
          nbFrames = 0;
          FPS_last_Time += 1.0;
        }
        g_textRenderer->drawText(screen_quad.width, screen_quad.height,
                                 ("FPS: " + std::to_string(FPS)).c_str());

       
        std::string strhited;
		std::string artCount = "Artifacts found : " + std::to_string(gameCounter) + " out of 3";
		g_textRenderer->drawText(screen_quad.width, screen_quad.height, artCount.c_str(), 150.0f);
		if (gameCounter > 3) {
			g_textRenderer->drawText(screen_quad.width, screen_quad.height, ("Conradulations! You Win!"), 200.0f);
		}

#pragma endregion

    {
      glm::mat4x4 view_matrix = camera.GetViewMatrix();
      glm::mat4x4 projection_matrix =  glm::perspective(1.0f, screen_quad.width / (float) screen_quad.height, 0.1f, 15000.0f);
      
      ::g_pDebugRendererACTUAL->RenderDebugObjects(view_matrix, projection_matrix,
      deltaTime);
    }

#pragma region Update physics
    currentTime = glfwGetTime();
    deltaTime = currentTime - lastTime;

    double MAX_DELTA_TIME = 0.1; 
    if (deltaTime > MAX_DELTA_TIME) {
      deltaTime = MAX_DELTA_TIME;
    }
    // update the "last time"
    lastTime = currentTime;

    for (unsigned int objIndex = 0;
         objIndex != (unsigned int)vec_pObjectsToDraw.size(); objIndex++) {
      cGameObject *pCurrentMesh = vec_pObjectsToDraw[objIndex];

      pCurrentMesh->Update(deltaTime);

    } // for ( unsigned int objIndex = 0;

    gPhysicsWorld->Update(deltaTime);

		


	std::string PairF = gPhysicsWorld->GetLastColPair().first;
	std::string PairS = gPhysicsWorld->GetLastColPair().second;

	if (!collidedA) {
		if (PairF == "artifact_1" && PairS == "ghostSphere1" ||
			PairS == "artifact_1" && PairF == "ghostSphere1") {

			collidedA = true;
			cGameObject* artifact = findObjectByFriendlyName("artifact_1");
			artifact->bIsVisible = false;
			gPhysicsWorld->RemoveBody(artifact->rigidBody);
			artifact->rigidBody->~iRigidBody();
			artifact->rigidBody = NULL;
			gameCounter += 1;
		}
	}

	if (!collidedB) {
		if (PairF == "artifact_2" && PairS == "ghostSphere1" ||
			PairS == "artifact_2" && PairF == "ghostSphere1") {

			collidedB = true;
			cGameObject* artifact = findObjectByFriendlyName("artifact_2");
			artifact->bIsVisible = false;
			gPhysicsWorld->RemoveBody(artifact->rigidBody);
			artifact->rigidBody->~iRigidBody();
			artifact->rigidBody = NULL;
			gameCounter += 1;
		}
	}

	if (!collidedC) {
		if (PairF == "artifact_3" && PairS == "ghostSphere1" ||
			PairS == "artifact_3" && PairF == "ghostSphere1") {

			collidedC = true;
			cGameObject* artifact = findObjectByFriendlyName("artifact_3");
			artifact->bIsVisible = false;
			gPhysicsWorld->RemoveBody(artifact->rigidBody);
			artifact->rigidBody->~iRigidBody();
			artifact->rigidBody = NULL;
			gameCounter += 1;
		}
	}

	if (!collidedD) {
		if (PairF == "artifact_4" && PairS == "ghostSphere1" ||
			PairS == "artifact_4" && PairF == "ghostSphere1") {

			collidedD = true;
			cGameObject* artifact = findObjectByFriendlyName("artifact_4");
			
			artifact->bIsVisible = false;
			gPhysicsWorld->RemoveBody(artifact->rigidBody);
			artifact->rigidBody->~iRigidBody();
			artifact->rigidBody = NULL;
			gameCounter += 1;
		}
	}





	if (PairF == "mutant-1" && PairS == "chan" ||
		PairS == "mutant-1" && PairF == "chan") {

		for (int i = 0; i < vec_pObjectsToDraw.size(); i++) {
			vec_pObjectsToDraw[i]->position = vec_pObjectsToDraw[i]->InitPosition;
		}
		gameCounter = 0;
	}
	if (PairF == "mutant-2" && PairS == "chan" ||
		PairS == "mutant-2" && PairF == "chan") {

		for (int i = 0; i < vec_pObjectsToDraw.size(); i++) {
			vec_pObjectsToDraw[i]->position = vec_pObjectsToDraw[i]->InitPosition;
		}

		gameCounter = 0;
	}
	if (PairF == "mutant-3" && PairS == "chan" ||
		PairS == "mutant-3" && PairF == "chan") {

		for (int i = 0; i < vec_pObjectsToDraw.size(); i++) {
			if (vec_pObjectsToDraw[i]->rigidBody != NULL && vec_pObjectsToDraw[i]->rigidBody->GetMass() != 0.0f)
			{
				vec_pObjectsToDraw[i]->rigidBody->SetPosition(vec_pObjectsToDraw[i]->InitPosition);
			}
			
		}
		gameCounter = 0;
	}





    for (int i = 0; i < vec_pObjectsToDraw.size(); i++) {
      cGameObject *curMesh = vec_pObjectsToDraw[i];

      if (curMesh->rigidBody != NULL &&
          curMesh->rigidBody->GetMass() !=
              0.0f) {

        if (curMesh->rigidBody->GetShape()->GetShapeType() ==
            nPhysics::SHAPE_TYPE_CAPSULE) {
          float Totalheight;
          Totalheight =
              curMesh->rigidBody->GetShape()->GetCapsuleRadius() + 3.8f;
          curMesh->position = curMesh->rigidBody->GetPosition();
          curMesh->position.y =
              curMesh->rigidBody->GetPosition().y - Totalheight;

        } else {
          curMesh->position = curMesh->rigidBody->GetPosition();
          curMesh->m_meshQOrientation =
              glm::mat4(curMesh->rigidBody->GetMatRotation());
        }
      }
    }
	
	if (boat->position.y < 40.0f)
	{
		boat->rigidBody->ApplyImpulse(glm::vec3(0.0f, 180.0f, 0.0f));
	}



#pragma endregion
#pragma region whatever

    //::p_LuaScripts->UpdateCG(deltaTime);
    //::p_LuaScripts->Update(deltaTime);

	behavManager->update(deltaTime);

    player = g_pCharacterManager->getActiveChar();
    if (camera.mCameraType == THIRD_PERSON)

    {
      camera.updateCameraVectors();
      glm::vec3 lookDirection = camera.Position - player->position;
      lookDirection.y = 0.0f;
      lookDirection = glm::normalize(lookDirection);
      glm::vec3 worldUP(0.0f, 1.0f, 0.0f);
      glm::mat4 finalOrientation = glm::inverse(
          glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), lookDirection, worldUP));
      player->m_meshQOrientation = glm::toQuat(finalOrientation);

    }

	if (camera.mCameraType == AIM)
	{
		camera.updateCameraVectors();
		//camera.updateCameraVectors();
		glm::vec3 lookDirection = camera.Position + camera.Front;
		lookDirection = glm::normalize(camera.Front);
		lookDirection.y = 0.0f;
		glm::vec3 worldUP(0.0f, 1.0f, 0.0f);
		glm::mat4 finalOrientation = glm::inverse(
			glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), -lookDirection * 10.0f, worldUP));
		player->m_meshQOrientation = glm::toQuat(finalOrientation);

	}


	if (camera.mCameraType == FREE)
	{
		camera.updateCameraVectors();
		player->rigidBody->SetVelocity(glm::vec3(0.f));
	}

    // Update collision flag
    for (int i = 0; i < vec_pObjectsToDraw.size(); i++) {
      cGameObject *CurGo = vec_pObjectsToDraw[i];
      if (CurGo->rigidBody != NULL) {
        CurGo->bHadCollision = CurGo->rigidBody->GetCollision();
        CurGo->rigidBody->SetCollision(false);
      }
    }



	//Updaters
	g_pCharacterManager->updateCharAnimControllers();
	g_pSoundManager->Update();



#pragma endregion
    glfwSwapBuffers(window); 
    glfwPollEvents();

    ProcessAsynKeys(window);
  } 
  

  delete pTheShaderManager;
  delete ::g_pTheVAOMeshManager;
  delete ::g_pTheTextureManager;
  delete ::g_pCharacterManager;
  delete ::g_pDebugRenderer;

  glfwDestroyWindow(window);
  glfwTerminate();
  exit(EXIT_SUCCESS);
}




cGameObject *findObjectByFriendlyName(std::string theNameToFind) {
  for (unsigned int index = 0; index != vec_pObjectsToDraw.size(); index++) {
    if (vec_pObjectsToDraw[index]->friendlyName == theNameToFind) {
      return vec_pObjectsToDraw[index];
    }
  }

  // Didn't find it.
  return NULL; // 0 or nullptr
}

cGameObject *findObjectByUniqueID(unsigned int ID_to_find) {
  for (unsigned int index = 0; index != vec_pObjectsToDraw.size(); index++) {
    if (vec_pObjectsToDraw[index]->getUniqueID() == ID_to_find) {
      return vec_pObjectsToDraw[index];
    }
  }

  return NULL; 
}



