//DrawOjbect_calls.cpp
#include "globalOpenGLStuff.h"		// For GLFW and glad (OpenGL calls)
#include "globalStuff.h"
#include "GlobalCharacterControlls.h"

#include <glm/glm.hpp>
#include <glm/vec3.hpp> // glm::vec3
#include <glm/vec4.hpp> // glm::vec4
#include <glm/mat4x4.hpp> // glm::mat4
#include <glm/gtc/matrix_transform.hpp> // glm::translate, glm::rotate, glm::scale, glm::perspective
#include <glm/gtc/type_ptr.hpp> // glm::value_ptr

#include "cShaderManager.h"

#include "cGameObject.h"

#include "cAnimationState.h"

#include <iostream>


//std::map<std::string /*name*/, cParticleEmitter* > g_map_pParticleEmitters;




bool HACK_bTextureUniformLocationsLoaded = false;
GLint tex00_UniLoc = -1;
GLint tex01_UniLoc = -1;
GLint tex02_UniLoc = -1;
GLint tex03_UniLoc = -1;
GLint tex04_UniLoc = -1;
GLint tex05_UniLoc = -1;
GLint tex06_UniLoc = -1;
GLint tex07_UniLoc = -1;

GLint texBW_0_UniLoc = -1;
GLint texBW_1_UniLoc = -1;

// Texture sampler for off screen texture
GLint texPass1OutputTexture_UniLoc = -1;



// Will bind the textures in use for this object on this draw call
void BindTextures(cGameObject* pCurrentMesh, GLuint shaderProgramID)
{

	if (!HACK_bTextureUniformLocationsLoaded)
	{
		tex00_UniLoc = glGetUniformLocation(shaderProgramID, "texture00");		// uniform sampler2D texture00;
		tex01_UniLoc = glGetUniformLocation(shaderProgramID, "texture01");		// uniform sampler2D texture01;
		tex02_UniLoc = glGetUniformLocation(shaderProgramID, "texture02");		// uniform sampler2D texture02;
		tex03_UniLoc = glGetUniformLocation(shaderProgramID, "texture03");		// uniform sampler2D texture03;
		tex04_UniLoc = glGetUniformLocation(shaderProgramID, "texture04");		// uniform sampler2D texture04;
		tex05_UniLoc = glGetUniformLocation(shaderProgramID, "texture05");		// uniform sampler2D texture05;
		tex06_UniLoc = glGetUniformLocation(shaderProgramID, "texture06");		// uniform sampler2D texture06;
		tex07_UniLoc = glGetUniformLocation(shaderProgramID, "texture07");		// uniform sampler2D texture07;

		texBW_0_UniLoc = glGetUniformLocation(shaderProgramID, "texBlendWeights[0]");	// uniform vec4 texBlendWeights[2];
		texBW_1_UniLoc = glGetUniformLocation(shaderProgramID, "texBlendWeights[1]");	// uniform vec4 texBlendWeights[2];

		HACK_bTextureUniformLocationsLoaded = true;


		texPass1OutputTexture_UniLoc = glGetUniformLocation(shaderProgramID, "texPass1OutputTexture");

	}



	if (pCurrentMesh->b_HACK_UsesOffscreenFBO)
	{


		int FBO_Texture_Unit_Michael_Picked = 1;

		glActiveTexture(GL_TEXTURE0 + FBO_Texture_Unit_Michael_Picked);

		glBindTexture(GL_TEXTURE_2D, ::g_pFBOMain->colourTexture_0_ID);

		glUniform1i(texPass1OutputTexture_UniLoc, FBO_Texture_Unit_Michael_Picked);

		return;
	}

	float blendWeights[8] = { 0 };


	for (int texBindIndex = 0; texBindIndex != pCurrentMesh->vecTextures.size(); texBindIndex++)
	{
		// Bind to the the "texBindIndex" texture unit
		glActiveTexture(GL_TEXTURE0 + texBindIndex);

		// Connect the specific texture to THIS texture unit
		std::string texName = pCurrentMesh->vecTextures[texBindIndex].name;

		GLuint texID = ::g_pTheTextureManager->getTextureIDFromName(texName);

		glBindTexture(GL_TEXTURE_2D, texID);

		//the samplers can't be in an array
		switch (texBindIndex)
		{
		case 0:		// uniform sampler2D texture00  AND texBlendWeights[0].x;
			glUniform1i(tex00_UniLoc, texBindIndex);
			break;
		case 1:		// uniform sampler2D texture01  AND texBlendWeights[0].y;
			glUniform1i(tex01_UniLoc, texBindIndex);
			break;
		case 2:
			glUniform1i(tex02_UniLoc, texBindIndex);
			break;
		case 3:
			glUniform1i(tex03_UniLoc, texBindIndex);
			break;
		case 4:		// uniform sampler2D texture04  AND texBlendWeights[1].x;
			glUniform1i(tex04_UniLoc, texBindIndex);
			break;
		case 5:
			glUniform1i(tex05_UniLoc, texBindIndex);
			break;
		case 6:
			glUniform1i(tex06_UniLoc, texBindIndex);
			break;
		case 7:
			glUniform1i(tex07_UniLoc, texBindIndex);
			break;
		}//switch ( texBindIndex )

		// Set the blend weight (strengty)
		blendWeights[texBindIndex] = pCurrentMesh->vecTextures[texBindIndex].strength;

	}//for ( int texBindIndex

	// Set the weights (strengths) in the shader
	glUniform4f(texBW_0_UniLoc, blendWeights[0], blendWeights[1], blendWeights[2], blendWeights[3]);
	glUniform4f(texBW_1_UniLoc, blendWeights[4], blendWeights[5], blendWeights[6], blendWeights[7]);

	return;
}





void DrawScene_Simple(std::vector<cGameObject*> vec_pMeshSceneObjects,
	GLuint shaderProgramID,
	unsigned int passNumber)
{
	for (unsigned int objIndex = 0;
		objIndex != (unsigned int)vec_pMeshSceneObjects.size();
		objIndex++)
	{
		cGameObject* pCurrentMesh = vec_pMeshSceneObjects[objIndex];
		

		if(pCurrentMesh->softBody != NULL)
		{
			size_t nodes = pCurrentMesh->softBody->NumNodes();
			
			for (size_t nodeIndex = 0; nodeIndex < nodes; nodeIndex++)
			{
				glm::vec3 nodePosition;
				pCurrentMesh->softBody->GetNodePostion(nodeIndex, nodePosition); 
				pCurrentMesh->bIsWireFrame = true;
				pCurrentMesh->bDontLight = true;
				pCurrentMesh->position = nodePosition;
				float radius;
				pCurrentMesh->softBody->GetNodeRadius(nodeIndex, radius);
				pCurrentMesh->setUniformScale(radius);
				glm::mat4x4 matModel = glm::mat4(1.0f);
				DrawObject(pCurrentMesh, matModel, shaderProgramID);
			}
			
		}

		else
		{
			glm::mat4x4 matModel = glm::mat4(1.0f);
			DrawObject(pCurrentMesh, matModel, shaderProgramID);
		}
		

	}

	return;
}

static float playTime = 0.0f;
std::string prevAnim = "Idle";
bool exited = false;

void DrawObject(cGameObject* pCurrentMesh,
	glm::mat4x4 &matModel,
	GLuint shaderProgramID)
{

	// Is this object visible
	if (!pCurrentMesh->bIsVisible)
	{
		return;
	}



	// Set up the texture binding for this object
	BindTextures(pCurrentMesh, shaderProgramID);



	glm::mat4 matTranslation = glm::translate(glm::mat4(1.0f),
		pCurrentMesh->position);
	matModel = matModel * matTranslation;		// matMove

	glm::quat qRotation = pCurrentMesh->getQOrientation();
	// Generate the 4x4 matrix for that
	glm::mat4 matQrotation = glm::mat4(qRotation);

	matModel = matModel * matQrotation;

	glm::mat4 matModelInvTrans = glm::inverse(glm::transpose(matModel));

	// And now scale

	glm::mat4 matScale = glm::scale(glm::mat4(1.0f),
		pCurrentMesh->nonUniformScale);
	matModel = matModel * matScale;


	glUseProgram(shaderProgramID);

	GLint objectDiffuse_UniLoc = glGetUniformLocation(shaderProgramID, "objectDiffuse");
	GLint objectSpecular_UniLoc = glGetUniformLocation(shaderProgramID, "objectSpecular");
	GLint lightPos_UniLoc = glGetUniformLocation(shaderProgramID, "lightPos");
	GLint lightBrightness_UniLoc = glGetUniformLocation(shaderProgramID, "lightBrightness");
	GLint useVertexColour_UniLoc = glGetUniformLocation(shaderProgramID, "useVertexColour");
	GLint matModel_location = glGetUniformLocation(shaderProgramID, "matModel");
	GLint matModelInvTrans_location = glGetUniformLocation(shaderProgramID, "matModelInvTrans");
	GLint matView_location = glGetUniformLocation(shaderProgramID, "matView");
	GLint matProj_location = glGetUniformLocation(shaderProgramID, "matProj");
	GLint bDontUseLighting_UniLoc = glGetUniformLocation(shaderProgramID, "bDontUseLighting");

	glUniformMatrix4fv(matModel_location, 1, GL_FALSE, glm::value_ptr(matModel));
	glUniformMatrix4fv(matModelInvTrans_location, 1, GL_FALSE, glm::value_ptr(matModelInvTrans));


	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);	


	glEnable(GL_BLEND);


	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	GLint wholeObjectAlphaTransparency_LocID = glGetUniformLocation(shaderProgramID,
		"wholeObjectAlphaTransparency");
	glUniform1f(wholeObjectAlphaTransparency_LocID, pCurrentMesh->materialDiffuse.a);


	glUniform4f(objectDiffuse_UniLoc,
		pCurrentMesh->materialDiffuse.r,
		pCurrentMesh->materialDiffuse.g,
		pCurrentMesh->materialDiffuse.b,
		pCurrentMesh->materialDiffuse.a);
	glUniform4f(objectSpecular_UniLoc,
		pCurrentMesh->materialSpecular.r,
		pCurrentMesh->materialSpecular.g,
		pCurrentMesh->materialSpecular.b,
		pCurrentMesh->materialSpecular.a);

	if (pCurrentMesh->bUseVertexColour)
	{
		glUniform1f(useVertexColour_UniLoc, (float)GL_TRUE);
	}
	else
	{
		glUniform1f(useVertexColour_UniLoc, (float)GL_FALSE);
	}

	if (pCurrentMesh->bDontLight)
	{
		glUniform1f(bDontUseLighting_UniLoc, (float)GL_TRUE);
	}
	else
	{
		glUniform1f(bDontUseLighting_UniLoc, (float)GL_FALSE);
	}

	if (pCurrentMesh->bIsWireFrame)
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		glDisable(GL_CULL_FACE);	// Discared "back facing" triangles
	}
	else
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		glEnable(GL_CULL_FACE);	// Discared "back facing" triangles
	}

	// *****************************************************************
	//  ___ _   _                  _ __  __        _    
	// / __| |_(_)_ _  _ _  ___ __| |  \/  |___ __| |_  
	// \__ \ / / | ' \| ' \/ -_) _` | |\/| / -_|_-< ' \ 
	// |___/_\_\_|_||_|_||_\___\__,_|_|  |_\___/__/_||_|
	//                                                  
	GLint bIsASkinnedMesh_LocID = glGetUniformLocation(shaderProgramID,
		"bIsASkinnedMesh");

	// Is this a skinned mesh model or a "regular" static one?
	sModelDrawInfo modelInfo;
	if (pCurrentMesh->pSimpleSkinnedMesh == NULL)
	{
		// It's a "regular" mesh
		modelInfo.meshFileName = pCurrentMesh->meshName;

		glUniform1f(bIsASkinnedMesh_LocID, (float)GL_FALSE);
	}
	else
	{
		//if(pCurrentMesh->pSimpleSkinnedMesh->GetAnimationInfo(pCurrentMesh->currentAnimation)->bHasExitTime){}
		std::string CurAnim = g_pCharacterController->GetCurrentAnimation();

		if (CurAnim != prevAnim)
		{
			playTime = 0.0f;
		}


		// It ++IS++ skinned mesh
		modelInfo.meshFileName = pCurrentMesh->pSimpleSkinnedMesh->fileName;

		glUniform1f(bIsASkinnedMesh_LocID, (float)GL_TRUE);

		// Also pass up the bone information...
		std::vector< glm::mat4x4 > vecFinalTransformation;	// Replaced by	theMesh.vecFinalTransformation
		std::vector< glm::mat4x4 > vecOffsets;

		//		cAnimationState* pAniState = pCurrentMesh->pAniState->;
				// Are there any animations in the queue?
		//		if ( pCurrentMesh->pAniState->vecAnimationQueue.empty() )

		pCurrentMesh->pSimpleSkinnedMesh->BoneTransform(
			//0.0f,	// curFrameTime,
			playTime,	// curFrameTime,
		CurAnim,
		vecFinalTransformation,		// Final bone transforms for mesh
		pCurrentMesh->vecObjectBoneTransformation,  // final location of bones
		vecOffsets);                 // local offset for each bone


		playTime += deltaTime;		// Frame time, but we are going at 60HZ
		float dur = pCurrentMesh->pSimpleSkinnedMesh->GetDurationInSec(CurAnim);
		if (playTime >= dur)
		{
			
			//playTime = 0;

		}

		unsigned int numberOfBonesUsed = static_cast<unsigned int>(vecFinalTransformation.size());

		GLint numBonesUsed_UniLoc = glGetUniformLocation(shaderProgramID, "numBonesUsed");
		glUniform1i(numBonesUsed_UniLoc, numberOfBonesUsed);

		//		const unsigned int TOTALNUMBEROFBONESTOPASSINTOTHESHADERASIDENTIYMATRIXVALUES = 99;
		//		for ( unsigned int index = 0; index != numberOfBonesUsed; index++ )
		//		{
		//			vecFinalTransformation.push_back( glm::mat4(1.0f) );
		//		}

		glm::mat4x4* pBoneMatrixArray = &(vecFinalTransformation[0]);

		// UniformLoc_bonesArray is the getUniformLoc of "bones[0]" from
		//	uniform mat4 bones[MAXNUMBEROFBONES] 
		// in the shader
		GLint bones_UniLoc = glGetUniformLocation(shaderProgramID, "bones");
		//		std::cout << "bones_UniLoc: " << bones_UniLoc << std::endl;	std::cout.flush();
		glUniformMatrix4fv(bones_UniLoc, numberOfBonesUsed, GL_FALSE,
			(const GLfloat*)glm::value_ptr(*pBoneMatrixArray));


		for (unsigned int boneIndex = 0; boneIndex != numberOfBonesUsed; boneIndex++)
		{
			glm::mat4 boneLocal = pCurrentMesh->vecObjectBoneTransformation[boneIndex];

			float scale = 1.0f;	// For now
			boneLocal = glm::scale(boneLocal, glm::vec3(pCurrentMesh->nonUniformScale.x,
				pCurrentMesh->nonUniformScale.y,
				pCurrentMesh->nonUniformScale.z));

			glm::vec4 boneBallLocation = boneLocal * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
			boneBallLocation *= scale;




			// Update the extents of the mesh
			if (boneIndex == 0)
			{
				// For the 0th bone, just assume this is the extent
				pCurrentMesh->minXYZ_from_SM_Bones = glm::vec3(boneBallLocation);
				pCurrentMesh->maxXYZ_from_SM_Bones = glm::vec3(boneBallLocation);
			}
			else
			{	// It's NOT the 0th bone, so compare with current max and min
				if (pCurrentMesh->minXYZ_from_SM_Bones.x < boneBallLocation.x) { pCurrentMesh->minXYZ_from_SM_Bones.x = boneBallLocation.x; }
				if (pCurrentMesh->minXYZ_from_SM_Bones.y < boneBallLocation.y) { pCurrentMesh->minXYZ_from_SM_Bones.y = boneBallLocation.y; }
				if (pCurrentMesh->minXYZ_from_SM_Bones.z < boneBallLocation.z) { pCurrentMesh->minXYZ_from_SM_Bones.z = boneBallLocation.z; }

				if (pCurrentMesh->maxXYZ_from_SM_Bones.x > boneBallLocation.x) { pCurrentMesh->maxXYZ_from_SM_Bones.x = boneBallLocation.x; }
				if (pCurrentMesh->maxXYZ_from_SM_Bones.y > boneBallLocation.y)
				{
					pCurrentMesh->maxXYZ_from_SM_Bones.y = boneBallLocation.y;
				}
				if (pCurrentMesh->maxXYZ_from_SM_Bones.z > boneBallLocation.z)
				{
					pCurrentMesh->maxXYZ_from_SM_Bones.z = boneBallLocation.z;
				}
			}//if ( boneIndex == 0 )
			

		}

		prevAnim = CurAnim;
		
		

	}//if ( pCurrentMesh->pSimpleSkinnedMesh == NULL )
//  ___ _   _                  _ __  __        _    
// / __| |_(_)_ _  _ _  ___ __| |  \/  |___ __| |_  
// \__ \ / / | ' \| ' \/ -_) _` | |\/| / -_|_-< ' \ 
// |___/_\_\_|_||_|_||_\___\__,_|_|  |_\___/__/_||_|
//                                                  
// *****************************************************************


	if (g_pTheVAOMeshManager->FindDrawInfoByModelName(modelInfo))
	{
		//glDrawArrays(GL_TRIANGLES, 0, bunnyInfo.numberOfIndices );

		glBindVertexArray(modelInfo.VAO_ID);

		glDrawElements(GL_TRIANGLES,
			modelInfo.numberOfIndices,
			GL_UNSIGNED_INT,
			0);

		glBindVertexArray(0);

	}
	else
	{
		std::cout << pCurrentMesh->meshName << " was not found" << std::endl;
	}


	for (unsigned int childMeshIndex = 0; childMeshIndex != pCurrentMesh->vec_pChildObjectsToDraw.size(); childMeshIndex++)
	{
		glm::mat4 matWorldParent = matModel;
		DrawObject(pCurrentMesh->vec_pChildObjectsToDraw[childMeshIndex], matWorldParent, shaderProgramID);
	}

	return;
}//void DrawObject(void)





// Draws any particle emitters that are active
//void updateAndDrawParticles(double deltaTime,
//	GLuint shaderProgramID,
//	glm::vec3 cameraEye)
//{
//
//	// These GetUniformLocation() calls should NOT be in the draw call
//	// (you should get these at the start and cache them in the cShaderObject, perhaps)
//	GLint bIsParticleImposter_UniLoc = glGetUniformLocation(shaderProgramID, "bIsParticleImposter");
//	GLint ParticleImposterAlphaOverride_UniLoc = glGetUniformLocation(shaderProgramID, "ParticleImposterAlphaOverride");
//	GLint ParticleImposterBlackThreshold_UniLoc = glGetUniformLocation(shaderProgramID, "ParticleImposterBlackThreshold");
//
//	// Black threshold is where the imposter will discard 
//	// i.e. At or below this value, the imposter isn't draw. 
//	// (range is from 0.0 to 1.0)
//	glUniform1f(ParticleImposterBlackThreshold_UniLoc, 0.25f);
//
//	//// STARTOF: Star shaped smoke particle
//	//std::map<std::string /*name*/, cParticleEmitter* >::iterator itPE_Smoke01
//	//	= ::g_map_pParticleEmitters.find("Smoke01");
//
//	//if (itPE_Smoke01 != ::g_map_pParticleEmitters.end())
//	//{
//
//	//	cParticleEmitter* pPE_Smoke01 = itPE_Smoke01->second;
//
//	//	// Update the particle emitter
//	//	cGameObject* pParticleMesh = findObjectByFriendlyName("SmokeObjectStar");
//	//	glm::mat4 matParticleIndentity = glm::mat4(1.0f);
//	//	glm::vec3 oldPosition = pParticleMesh->position;
//	//	glm::quat oldOrientation = pParticleMesh->getQOrientation();
//	//	glm::vec3 oldScale = pParticleMesh->nonUniformScale;
//
//	//	pParticleMesh->setMeshOrientationEulerAngles(0.0f, 0.0f, 0.0f);
//	//	pParticleMesh->bIsVisible = true;
//
//
//	//	// Set up the shader
//	//	glUniform1f(bIsParticleImposter_UniLoc, (float)GL_TRUE);
//
//
//	//	pPE_Smoke01->Update(deltaTime);
//
//	//	std::vector<sParticle> vecParticlesToDraw;
//	//	pPE_Smoke01->getAliveParticles(vecParticlesToDraw);
//
//	//	pPE_Smoke01->sortParticlesBackToFront(vecParticlesToDraw, cameraEye);
//
//	//	unsigned int numParticles = (unsigned int)vecParticlesToDraw.size();
//	//	//			std::cout << "Drawing " << numParticles << " particles" << std::end;
//
//	//	unsigned int count = 0;
//	//	for (unsigned int index = 0; index != numParticles; index++)
//	//	{
//	//		if (vecParticlesToDraw[index].lifeRemaining > 0.0f)
//	//		{
//	//			// Draw it
//	//			pParticleMesh->position = vecParticlesToDraw[index].position;
//	//			pParticleMesh->setUniformScale(vecParticlesToDraw[index].scale);
//	//			pParticleMesh->setQOrientation(vecParticlesToDraw[index].qOrientation);
//
//	//			// This is for the "death" transparency
//	//			glUniform1f(ParticleImposterAlphaOverride_UniLoc, vecParticlesToDraw[index].transparency);
//
//	//			DrawObject(pParticleMesh, matParticleIndentity, shaderProgramID);
//	//			count++;
//	//		}
//	//	}
//	//	//			std::cout << "Drew " << count << " particles" << std::endl;
//	//	pParticleMesh->bIsVisible = false;
//	//	pParticleMesh->position = oldPosition;
//	//	pParticleMesh->setQOrientation(oldOrientation);
//	//	pParticleMesh->nonUniformScale = oldScale;
//	//	glUniform1f(bIsParticleImposter_UniLoc, (float)GL_FALSE);
//	//	glUniform1f(ParticleImposterAlphaOverride_UniLoc, 1.0f);
//	//	// ***************************************************************************
//	//}
//	//// ENDOF: Star shaped smoke particle
//
//
//	//// STARTOF: flat 2D smoke particle
//	//std::map<std::string /*name*/, cParticleEmitter* >::iterator itPE_Smoke02
//	//	= ::g_map_pParticleEmitters.find("Smoke02");
//
//	//if (itPE_Smoke02 != ::g_map_pParticleEmitters.end())
//	//{
//
//	//	cParticleEmitter* pPE_Smoke02 = itPE_Smoke02->second;
//
//	//	// Update the particle emitter
//	//	cGameObject* pParticleMesh = findObjectByFriendlyName("SmokeObjectQuad");
//	//	glm::mat4 matParticleIndentity = glm::mat4(1.0f);
//	//	glm::vec3 oldPosition = pParticleMesh->position;
//	//	glm::quat oldOrientation = pParticleMesh->getQOrientation();
//	//	glm::vec3 oldScale = pParticleMesh->nonUniformScale;
//
//	//	pParticleMesh->setMeshOrientationEulerAngles(0.0f, 0.0f, 0.0f);
//	//	pParticleMesh->bIsVisible = true;
//
//
//	//	// Set up the shader
//	//	glUniform1f(bIsParticleImposter_UniLoc, (float)GL_TRUE);
//
//
//	//	pPE_Smoke02->Update(deltaTime);
//
//	//	std::vector<sParticle> vecParticlesToDraw;
//	//	pPE_Smoke02->getAliveParticles(vecParticlesToDraw);
//
//	//	pPE_Smoke02->sortParticlesBackToFront(vecParticlesToDraw, cameraEye);
//
//	//	unsigned int numParticles = (unsigned int)vecParticlesToDraw.size();
//	//	//			std::cout << "Drawing " << numParticles << " particles" << std::end;
//
//	//	unsigned int count = 0;
//	//	for (unsigned int index = 0; index != numParticles; index++)
//	//	{
//	//		if (vecParticlesToDraw[index].lifeRemaining > 0.0f)
//	//		{
//	//			// Draw it
//	//			pParticleMesh->position = vecParticlesToDraw[index].position;
//	//			pParticleMesh->setUniformScale(vecParticlesToDraw[index].scale);
//	//			pParticleMesh->setQOrientation(vecParticlesToDraw[index].qOrientation);
//
//	//			// This is for the "death" transparency
//	//			glUniform1f(ParticleImposterAlphaOverride_UniLoc, vecParticlesToDraw[index].transparency);
//
//	//			DrawObject(pParticleMesh, matParticleIndentity, shaderProgramID);
//	//			count++;
//	//		}
//	//	}
//	//	//			std::cout << "Drew " << count << " particles" << std::endl;
//	//	pParticleMesh->bIsVisible = false;
//	//	pParticleMesh->position = oldPosition;
//	//	pParticleMesh->setQOrientation(oldOrientation);
//	//	pParticleMesh->nonUniformScale = oldScale;
//	//	glUniform1f(bIsParticleImposter_UniLoc, (float)GL_FALSE);
//	//	glUniform1f(ParticleImposterAlphaOverride_UniLoc, 1.0f);
//	//	// ***************************************************************************
//	//}
//	//// ENDOF: Star shaped smoke particle
//
//
//
//
//	//// STARTOF: flat 2D plasma explosion
//	//std::map<std::string /*name*/, cParticleEmitter* >::iterator itPE_Plasma_01
//	//	= ::g_map_pParticleEmitters.find("PlasmaExplosion");
//
//	//if (itPE_Plasma_01 != ::g_map_pParticleEmitters.end())
//	//{
//
//	//	cParticleEmitter* pPE_Plasma_01 = itPE_Plasma_01->second;
//
//	//	// Update the particle emitter
//	//	cGameObject* pParticleMesh = findObjectByFriendlyName("PlasmaRingImposterObject");
//	//	glm::mat4 matParticleIndentity = glm::mat4(1.0f);
//	//	glm::vec3 oldPosition = pParticleMesh->position;
//	//	glm::quat oldOrientation = pParticleMesh->getQOrientation();
//	//	glm::vec3 oldScale = pParticleMesh->nonUniformScale;
//
//	//	pParticleMesh->setMeshOrientationEulerAngles(0.0f, 0.0f, 0.0f);
//	//	pParticleMesh->bIsVisible = true;
//
//
//	//	// Set up the shader
//	//	glUniform1f(bIsParticleImposter_UniLoc, (float)GL_TRUE);
//
//
//	//	pPE_Plasma_01->Update(deltaTime);
//
//	//	std::vector<sParticle> vecParticlesToDraw;
//	//	pPE_Plasma_01->getAliveParticles(vecParticlesToDraw);
//
//	//	pPE_Plasma_01->sortParticlesBackToFront(vecParticlesToDraw, cameraEye);
//
//	//	unsigned int numParticles = (unsigned int)vecParticlesToDraw.size();
//	//	//			std::cout << "Drawing " << numParticles << " particles" << std::end;
//
//	//	unsigned int count = 0;
//	//	for (unsigned int index = 0; index != numParticles; index++)
//	//	{
//	//		if (vecParticlesToDraw[index].lifeRemaining > 0.0f)
//	//		{
//	//			// Draw it
//	//			pParticleMesh->position = vecParticlesToDraw[index].position;
//	//			pParticleMesh->setUniformScale(vecParticlesToDraw[index].scale);
//	//			pParticleMesh->setQOrientation(vecParticlesToDraw[index].qOrientation);
//
//	//			// This is for the "death" transparency
//	//			glUniform1f(ParticleImposterAlphaOverride_UniLoc, vecParticlesToDraw[index].transparency);
//
//	//			DrawObject(pParticleMesh, matParticleIndentity, shaderProgramID);
//	//			count++;
//	//		}
//	//	}
//	//	//			std::cout << "Drew " << count << " particles" << std::endl;
//	//	pParticleMesh->bIsVisible = false;
//	//	pParticleMesh->position = oldPosition;
//	//	pParticleMesh->setQOrientation(oldOrientation);
//	//	pParticleMesh->nonUniformScale = oldScale;
//	//	glUniform1f(bIsParticleImposter_UniLoc, (float)GL_FALSE);
//	//	glUniform1f(ParticleImposterAlphaOverride_UniLoc, 1.0f);
//	//	// ***************************************************************************
//	//}
//	// ENDOF: flat 2D plasma explosion
//	return;
//}