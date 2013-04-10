uniform vec3 eyePos;
uniform float radius;
uniform vec3 color;


uniform float charge;

varying vec3 vEyeDir;
varying vec4 vVertex;
varying vec3 vNormal;


void main()
{
	vec3 darkRoomColor	= vec3( color );
	
	gl_FragColor.rgb	= darkRoomColor;
	gl_FragColor.a		= 0.0;
}