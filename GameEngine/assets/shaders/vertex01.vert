#version 420 
// vertex01.glsl

	
uniform mat4 matModel;		// M
uniform mat4 matModelInvTrans;	// inverse(transpose(matModel))
uniform mat4 matView;		// V
uniform mat4 matProj;		// P

in vec4 vColour;		// rgba   	was "attribute"
in vec4 vPosition;		// xyzw		was "attribute"
in vec4 vNormal;		// normal xyz
in vec4 vUV_x2;	
			
in vec4 vTanXYZ;			// Tangent to the surface
in vec4 vBiNormXYZ;		// bi-normal (or bi-tangent) to the surface

in vec4 vBoneID;		// really passing it as 4 values
in vec4 vBoneWeight;	


out vec4 color;			// exit to fragment
out vec4 vertPosWorld;	// "World space"
out vec4 vertNormal;	// "Model space"
out vec4 vertUV_x2;		// To the next shader stage
out vec4 vertTanXYZ;	// Tangent to the surface
out vec4 vertBiNormXYZ;	// bi-normal (or bi-tangent) to the surface
out vec4 viewSpace; //For fog



uniform sampler2D texHeightMap;
uniform bool bUseHeightMap;			
uniform float heightMapRatio;		
uniform float time;

// For skinned mesh
const int MAXNUMBEROFBONES = 100;
uniform mat4 bones[MAXNUMBEROFBONES];
// Pass the acutal number you are using to control how often this loops
uniform int numBonesUsed;			
uniform bool bIsASkinnedMesh;	// True to do the skinned mesh
void main()
{
	// Make a copy of the "model space" vertex
	vec3 posTemp = vPosition.xyz;
	
	// Apply vertex displacement?
	if ( bUseHeightMap )
	{

		vec2 tiledUV = vUV_x2.st * 15.0;
		float newTime = time * 0.00005;
		float height = texture( texHeightMap, vec2(vUV_x2.s + newTime, vUV_x2.t + newTime) ).r;
		//float height = clamp( HM_Color, 0.1, 1.0 );



		float HeightRatio = 0.05;
		height = height * HeightRatio;
		
		//posTemp.y = 0.0f;		// "Flatten" the mesh
		posTemp.y = height;		// Set the heigth

		// You could also "adjust" an existing mesh
	
	}//if ( bUseHeightMap )
	
    color = vColour;
	
	// Pass the texture coordinates out;
	vertUV_x2 = vUV_x2;
	
	// Also pass the bi-tangent (bi-normal) and tangent to fragment
	vertTanXYZ = vTanXYZ;		// Tangent to the surface
	vertBiNormXYZ = vBiNormXYZ;		// bi-normal (or bi-tangent) to the surface

	
	// Skinned mesh or not?
	if ( bIsASkinnedMesh )
	{	
		//*****************************************************************
		// Before I do the full MVP (screen space) calculation, 
		// I'm going to adjust where the vertex is based on the 
		// the bone locations
		
		mat4 BoneTransform 
		               = bones[ int(vBoneID[0]) ] * vBoneWeight[0];
		BoneTransform += bones[ int(vBoneID[1]) ] * vBoneWeight[1];
		BoneTransform += bones[ int(vBoneID[2]) ] * vBoneWeight[2];
		BoneTransform += bones[ int(vBoneID[3]) ] * vBoneWeight[3];
		// You could also do bBoneID.x, .y, etc.
				
	//	matrixWorld = BoneTransform * matrixWorld;
		
		vec4 vertPositionFromBones = BoneTransform * vec4(posTemp.xyz, 1.0);
		//*****************************************************************
	
		mat4 MVP = matProj * matView * matModel;
		gl_Position = MVP * vertPositionFromBones;			// ON SCREEN
		
		vertPosWorld = matModel * vec4(posTemp, 1.0);
		
		viewSpace = matView * matModel * vec4(posTemp,1);
		
		// Transforms the normal into "world space"
		// Remove all scaling and transformation from model
		// Leaving ONLY rotation... 
		
		// This only leaves rotation (translation and scale are removed)
		mat4 matBoneTransform_InTrans = inverse(transpose(BoneTransform));
		
		vec4 vNormBone = matBoneTransform_InTrans * vec4(normalize(vNormal.xyz),1.0f);
		
		vertNormal = matModelInvTrans * vNormBone;
		
		
		// And then you do the same for the binormal and bitangent
		vec4 vTanXYZ_Bone = matBoneTransform_InTrans * vTanXYZ;
		vertTanXYZ = matModelInvTrans * vTanXYZ_Bone;
		
		vec4 vBiNormXYZ_Bone = matBoneTransform_InTrans * vBiNormXYZ;
		vertBiNormXYZ = matModelInvTrans * vBiNormXYZ_Bone;
		
		
		// Debug: make green if this flag is set
		color.rgb = vec3(0.0f, 1.0f, 0.0f);
	}
	else
	{
		// Is a regular mesh
		// Note these are 'backwards'
		mat4 MVP = matProj * matView * matModel;
		gl_Position = MVP * vec4(posTemp, 1.0f);			// ON SCREEN
		
		vertPosWorld = matModel * vec4(posTemp, 1.0f);


		
		viewSpace = matView * matModel * vec4(posTemp,1);
		// Transforms the normal into "world space"
		// Remove all scaling and transformation from model
		// Leaving ONLY rotation... 
		vertNormal = matModelInvTrans * vec4(normalize(vNormal.xyz),1.0f);
	
	}//if ( bIsASkinnedMesh )
	
	
}



