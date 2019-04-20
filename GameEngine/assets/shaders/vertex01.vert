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


struct sLight
{
	vec4 position;			
	vec4 diffuse;	
	vec4 specular;	// rgb = highlight colour, w = power
	vec4 atten;		// x = constant, y = linear, z = quadratic, w = DistanceCutOff
	vec4 direction;	// Spot, directional lights
	vec4 param1;	// x = lightType, y = inner angle, z = outer angle, w = TBD
	                // 0 = pointlight
					// 1 = spot light
					// 2 = directional light
	vec4 param2;	// x = 0 for off, 1 for on
};

uniform vec3 eyeLocation;		// This is in "world space"
const int NUMBEROFLIGHTS = 20; 
uniform sLight theLights[NUMBEROFLIGHTS];  	// 80 uniforms
uniform float texTiling;

struct sVSOut
{
	vec4 color;			// exit to fragment
	vec4 vertPosWorld;	// "World space"
	vec4 vertNormal;	// "Model space"
	vec4 vertUV_x2;		// To the next shader stage
    vec4 TangentLightPos; 
    vec4 TangentViewPos;
    vec4 TangentFragPos;
	vec4 viewSpace;     //For fog
};
out sVSOut vsOutput;



uniform sampler2D texHeightMap;
uniform sampler2D texNormalMap;
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

		vec2 tiledUV = vUV_x2.st * texTiling;
		float newTime = time * 0.00001;
		float height = texture( texHeightMap, vec2(tiledUV.s + newTime, tiledUV.t + newTime) ).r;
		//float height = clamp( HM_Color, 0.1, 1.0 );



		float HeightRatio = 0.008;
		height = height * HeightRatio;
		
		//posTemp.y = 0.0f;		// "Flatten" the mesh
		posTemp.y = height;		// Set the heigth

		// You could also "adjust" an existing mesh
	
	}//if ( bUseHeightMap )
	
    vsOutput.color = vColour;
	
	// Pass the texture coordinates out;
	vsOutput.vertUV_x2 = vUV_x2;





	
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
		
		vsOutput.vertPosWorld = matModel * vec4(posTemp, 1.0);
		
		vsOutput.viewSpace = matView * matModel * vec4(posTemp,1);
		
		// Transforms the normal into "world space"
		// Remove all scaling and transformation from model
		// Leaving ONLY rotation... 
		
		// This only leaves rotation (translation and scale are removed)
		mat4 matBoneTransform_InTrans = inverse(transpose(BoneTransform));
		
		vec4 vNormBone = matBoneTransform_InTrans * vec4(normalize(vNormal.xyz),1.0f);
		
		vsOutput.vertNormal = matModelInvTrans * vNormBone;
		
		
		// And then you do the same for the binormal and bitangent

		
		
		// Debug: make green if this flag is set
		vsOutput.color.rgb = vec3(0.0f, 1.0f, 0.0f);
	}
	else
	{
		// Is a regular mesh
		// Note these are 'backwards'
		mat4 MVP = matProj * matView * matModel;
		gl_Position = MVP * vec4(posTemp, 1.0f);			// ON SCREEN
		
		vsOutput.vertPosWorld = matModel * vec4(posTemp, 1.0f);


		
		vsOutput.viewSpace = matView * matModel * vec4(posTemp,1);
		// Transforms the normal into "world space"
		// Remove all scaling and transformation from model
		// Leaving ONLY rotation... 
		vsOutput.vertNormal = matModelInvTrans * vec4(normalize(vNormal.xyz),1.0f);


	
	}//if ( bIsASkinnedMesh )





for ( int index = 0; index < NUMBEROFLIGHTS; index++ )
	{	
		// ********************************************************
		// is light "on"
		if ( theLights[index].param2.x == 0.0f )
		{	// it's off
			continue;
		}


	mat3 normalMatrix = transpose(inverse(mat3(matModel)));
    vec3 T = normalize(normalMatrix * vTanXYZ.xyz);
    vec3 N = normalize(normalMatrix * vBiNormXYZ.xyz);
    T = normalize(T - dot(T, N) * N);
    vec3 B = cross(N, T);
	
    
    mat3 TBN = transpose(mat3(T, B, N));    
    vsOutput.TangentLightPos.xyz = TBN * theLights[index].position.xyz;
    vsOutput.TangentViewPos.xyz  = TBN * eyeLocation.xyz;
    vsOutput.TangentFragPos.xyz  = TBN * vsOutput.vertPosWorld.xyz;

	}

	
	
}



