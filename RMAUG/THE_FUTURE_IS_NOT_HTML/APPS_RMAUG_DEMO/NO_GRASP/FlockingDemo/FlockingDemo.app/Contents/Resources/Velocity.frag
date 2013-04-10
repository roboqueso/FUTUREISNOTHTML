#version 110
uniform sampler2D positionTex;
uniform sampler2D velocityTex;
uniform int fboDim;
uniform float invFboDim;
uniform float sphereRadius;

uniform float dt;

uniform sampler2D lanternsTex;
uniform float numLights;
uniform float invNumLights;
uniform float invNumLightsHalf;
uniform float att;

void reactToLanterns( inout vec3 acc, inout float crowded, vec3 _myPos )
{
	float index = invNumLightsHalf;
	for( float i=0.0; i<numLights; i+=1.0 ){
		vec4 LANTERN	= texture2D( lanternsTex, vec2( index, 0.25 ) );
		float AGGRO		= texture2D( lanternsTex, vec2( index, 0.75 ) ).a;
		vec3 pos		= LANTERN.xyz;
		float radius	= LANTERN.w;
		float minRad	= ( radius + 35.0 ) * ( radius + 35.0 );	// 50.0
		float maxRad	= ( radius + 200.0 ) * ( radius + 200.0 ); // + 700.0
		
		vec3 dirToLantern		= _myPos - pos;
		float distToLantern		= length( dirToLantern );
		float distToLanternSqrd	= distToLantern * distToLantern; // * 2.0
		
		if( AGGRO > 0.0 ){	// ATTRACTED TO LIGHT
			if( distToLanternSqrd > minRad && distToLanternSqrd < maxRad ){
				float distPer = ( ( maxRad - minRad ) / ( distToLanternSqrd ) );
				float speedPer = 0.0375/( numLights + 1.0 );
				acc -= normalize( dirToLantern ) * distPer * speedPer * dt; // * 0.01075
			}
		} else {			// REPELLED BY LIGHT
			if( distToLanternSqrd < maxRad ){
				acc -= normalize( dirToLantern ) * ( maxRad / distToLanternSqrd ) * -0.005 * dt;	// 0.01075
				float per = ( maxRad ) / ( distToLanternSqrd );
				crowded += per * 0.0015; // 0.15
			}
		}
		
		// IF TOO CLOSE, MOVE AWAY MORE RAPIDLY
//		if( distToLantern < radius * 1.1 )
//			acc += normalize( dirToLantern ) * AGGRO;
		
		index			+= invNumLights;
	}
}

void main()
{	
	// REALTIME MAC LAPTOP
	float zoneRadius	 = 10.0;
	float zoneRadiusSqrd = zoneRadius * zoneRadius;
	float minThresh		 = 0.6;//0.44
	float maxThresh		 = 0.75;//0.85
	float maxSpeed		 = 8.0;//6.1; //4.1
	float crowdMulti	 = 0.025;//0.1

	
	vec4 vPos			= texture2D( positionTex, gl_TexCoord[0].st );
	vec3 myPos			= vPos.xyz;
	float leadership	= vPos.a;
	
	vec4 vVel			= texture2D( velocityTex, gl_TexCoord[0].st );
	vec3 myVel			= vVel.xyz;
	float myCrowd		= vVel.a;
	
	vec3 acc			= vec3( 0.0, 0.0, 0.0 );
	float offset		= invFboDim * 0.5;
	
	int myX				= int( gl_TexCoord[0].s * float(fboDim) );
	int myY				= int( gl_TexCoord[0].t * float(fboDim) );
	float crowded		= 2.0;
	
	
	// APPLY THE ATTRACTIVE, ALIGNING, AND REPULSIVE FORCES
	for( int y=0; y<fboDim; y++ ){
		for( int x=0; x<fboDim; x++ ){
			if( x == myX && y == myY ){
				// Avoid comparing my sphere against my sphere
			} else {
				vec2 tc			= vec2( float(x), float(y) ) * invFboDim + offset;
				vec4 pos		= texture2D( positionTex, tc );
				vec3 dir		= myPos - pos.xyz;
				
				float dist		= length( dir );
				float distSqrd	= dist * dist;
				
				vec3 dirNorm	= normalize( dir );
				
				if( distSqrd < zoneRadiusSqrd ){
					float percent = distSqrd/zoneRadiusSqrd + 0.0000001;
					crowded += ( 1.0 - percent ) * crowdMulti;
					
					// IF FISH IS CLOSE, REPEL
					if( percent < minThresh ){
						float F  = ( minThresh/percent - 1.0 );
						acc		+= dirNorm * F * 0.1 * dt * leadership;
						
						// IF FISH IS IN THE SWEET SPOT, ALIGN	
					} else if( percent < maxThresh ){
						float threshDelta		= maxThresh - minThresh;
						float adjustedPercent	= ( percent - minThresh )/( threshDelta + 0.0000001 );
						float F					= ( 1.0 - ( cos( adjustedPercent * 6.28318 ) * -0.5 + 0.5 ) );
						
						acc += normalize( texture2D( velocityTex, tc ).xyz ) * F * 0.1 * dt * leadership;
						
						// IF FISH IS FAR, BUT WITHIN THE ACCEPTABLE ZONE, ATTRACT	
					} else if( dist < zoneRadius ){
						
						float threshDelta		= 1.0 - maxThresh;
						float adjustedPercent	= ( percent - maxThresh )/( threshDelta + 0.0000001 );
						float F					= ( 1.0 - ( cos( adjustedPercent * 6.28318 ) * -0.5 + 0.5 ) ) * 0.1 * dt * leadership;
						
						acc -= dirNorm * F;
						
					}
				}
			}
		}
	}
	
	reactToLanterns( acc, crowded, myPos );
	
	myCrowd -= ( myCrowd - crowded ) * ( 0.1 * dt );
	
	// MODULATE MYCROWD MULTIPLIER AND GRAVITY FOR INTERESTING DERIVATIONS
	
	myVel += acc * dt;
	float newMaxSpeed = maxSpeed + myCrowd * 0.03;			// CROWDING MAKES EM FASTER
	
	float velLength = length( myVel );						// GET READY TO IMPOSE SPEED LIMIT
	if( velLength > newMaxSpeed ){							// SPEED LIMIT FOR FAST
		myVel = normalize( myVel ) * newMaxSpeed;
	} else if( velLength < 3.0 ){
		myVel = normalize( myVel ) * 3.0;
	}
	
	
	// KEEP FROM WANDERING TOO FAR
	float distFromCenter = length( myPos );
	if( distFromCenter > sphereRadius )
		myVel -= normalize( myPos ) * 0.25;
	
	
//	vec3 tempNewPos		= myPos + myVel * dt;		// NEXT POSITION
	
//	// AVOID WALLS
//	float xPull	= tempNewPos.x/( roomBounds.x );
//	float yPull	= tempNewPos.y/( roomBounds.y );
//	float zPull	= tempNewPos.z/( roomBounds.z );
//	myVel -= vec3( xPull * xPull * xPull * xPull * xPull, 
//				  yPull * yPull * yPull * yPull * yPull, 
//				  zPull * zPull * zPull * zPull * zPull ) * 0.1;
//	//}
//	
//	bool hitWall = false;
//	vec3 wallNormal = vec3( 0.0 );
//	float myRadius = 4.0;
//	
//	if( tempNewPos.y - myRadius < -roomBounds.y ){
//		hitWall = true;
//		wallNormal += vec3( 0.0, 1.0, 0.0 );
//	} else if( tempNewPos.y + myRadius > roomBounds.y ){
//		hitWall = true;
//		wallNormal += vec3( 0.0,-1.0, 0.0 );
//	}
//	
//	if( tempNewPos.x - myRadius < -roomBounds.x ){
//		hitWall = true;
//		wallNormal += vec3( 1.0, 0.0, 0.0 );
//	} else if( tempNewPos.x + myRadius > roomBounds.x ){
//		hitWall = true;
//		wallNormal += vec3(-1.0, 0.0, 0.0 );
//	}
//	
//	if( tempNewPos.z - myRadius < -roomBounds.z ){
//		hitWall = true;
//		wallNormal += vec3( 0.0, 0.0, 1.0 );
//	} else if( tempNewPos.z + myRadius > roomBounds.z ){
//		hitWall = true;
//		wallNormal += vec3( 0.0, 0.0,-1.0 );
//	}
//	
//	// WARNING, THIS MAY BE FAULTY MATH. MIGHT EXPLAIN LOST PARTICLES
//	if( hitWall ){
//		vec3 reflect = 2.0 * wallNormal * ( wallNormal * myVel );
//		myVel -= reflect * 0.65;
//	}
	
	
	gl_FragColor.rgb	= myVel;
	gl_FragColor.a		= myCrowd;
}
