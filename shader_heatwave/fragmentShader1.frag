precision mediump float;
varying vec4 fColor;
varying vec2 fTexCoord;
varying vec2 bgTexCoord;

uniform sampler2D sampler2d;
uniform sampler2D bgTexture;
uniform sampler2D bgDepthTexture;
uniform int myFlag1;
uniform float windowW;
uniform float windowH;


void main()
{
	if(myFlag1 == 0)
	{
		gl_FragColor = texture2D(sampler2d, fTexCoord);
	}
	else if(myFlag1 == 1)
	{
		float bgU = gl_FragCoord.x/windowW;
		float bgV = gl_FragCoord.y/windowH;
		float bgDepth = texture2D(bgDepthTexture, vec2(bgU,bgV)).r;

		if(bgDepth >= gl_FragCoord.z)
		{
			vec4 bgColor = texture2D(bgTexture, bgTexCoord);
			gl_FragColor = bgColor + 0.3*texture2D(sampler2d, fTexCoord);
			// gl_FragColor = bgColor + vec4(0.1);
		}
		else
		{
			discard;
		}
	}

}
