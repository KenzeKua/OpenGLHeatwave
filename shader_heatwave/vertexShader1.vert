attribute vec4 vPosition;
attribute vec4 vColor;
attribute vec2 vTexCoord;

varying vec2 fTexCoord;
varying vec4 fColor;
varying vec2 bgTexCoord;

uniform mat4 uMvpMatrix;
uniform int myFlag1;
uniform float ufactor1;
//uniform sampler2D noiseTexture;
//uniform sampler2D radialTexture;

float rand(vec2 co)
{
    return fract(sin(dot(co.xy ,vec2(12.9898,78.233))) * 43758.5453);
}

void main()
{
	if(myFlag1 == 0)
	{
		fColor = vColor;
		fTexCoord = vTexCoord;
	
		gl_Position = uMvpMatrix * vPosition;
	}
	else if(myFlag1 == 1)
	{
		fColor = vColor;
		fTexCoord = vTexCoord;

		vec2 pos2 = vPosition.xy;
		float stg = -1.0*clamp(length(pos2),0.0,1.0) + 1.0;

		vec4 thePos = vPosition;
		float r1 = rand(thePos.xy) + ufactor1*6.0;
		float r2 = rand(thePos.yx)*8.0 + ufactor1*6.0;
		thePos += vec4(cos(r1), sin(r2), 0.0, 0.0) * stg * 0.08;
		vec4 clipSpace = uMvpMatrix * thePos;
		vec3 ndc = clipSpace.xyz / clipSpace.w; //normalized device coord
		bgTexCoord = ndc.xy*0.5 + 0.5; //get the background UV
		
		gl_Position = uMvpMatrix * vPosition;
	}


}
