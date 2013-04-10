uniform sampler2D glowTex;
uniform float alpha;
uniform vec3 color;

varying vec4 vVertex;

void main()
{
	vec4 glowCol		= texture2D( glowTex, gl_TexCoord[0].st );

	gl_FragColor.rgb	= color;
	gl_FragColor.a		= glowCol.a * alpha;
}




