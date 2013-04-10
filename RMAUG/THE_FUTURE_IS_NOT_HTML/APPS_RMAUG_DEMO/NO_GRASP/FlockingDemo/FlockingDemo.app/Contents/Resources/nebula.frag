uniform sampler2D nebulaTex;
uniform float alpha;
uniform vec3 color;

varying vec4 vVertex;

void main()
{	
	vec4 nebulaCol		= texture2D( nebulaTex, gl_TexCoord[0].st );
	
	gl_FragColor.rgb	= color * alpha;
	gl_FragColor.a		= nebulaCol.a * alpha;
}




