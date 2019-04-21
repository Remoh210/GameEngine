#version 420
layout(triangles) in;
layout(triangle_strip, max_vertices=3) out;




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
// Note: this is an array
// of undefined size, as the primitives could be 1 to 6 vertices in size
in sVSOut vsOutput[];


struct sGSOut
{
	vec4 color;		
	vec4 vertPosWorld;	// "World space"
	vec4 vertNormal;	// "Model space"
	vec4 vertUV_x2;		// To the next shader stage
    vec4 viewSpace;     //For fog
    vec4 TangentLightPos; 
    vec4 TangentViewPos;
    vec4 TangentFragPos;
};

out sGSOut gsOuput;

vec3 normal;


//out vec3 normal;

void main( void )
{

    //vsOutput = gsOutput



    for( int i=0; i<gl_in.length( ); ++i )
    {

        gsOuput.color 			= vsOutput[i].color;
		gsOuput.vertPosWorld	= vsOutput[i].vertPosWorld;
        gsOuput.vertNormal = vsOutput[i].vertNormal;
		gsOuput.vertUV_x2	= vsOutput[i].vertUV_x2;
        gsOuput.viewSpace	= vsOutput[i].viewSpace;
        gsOuput.TangentLightPos = vsOutput[i].TangentLightPos;
        gsOuput.TangentViewPos = vsOutput[i].TangentViewPos;
        gsOuput.TangentFragPos = vsOutput[i].TangentFragPos;

        gl_Position = gl_in[i].gl_Position;
        EmitVertex( );
    }

    EndPrimitive( );
}