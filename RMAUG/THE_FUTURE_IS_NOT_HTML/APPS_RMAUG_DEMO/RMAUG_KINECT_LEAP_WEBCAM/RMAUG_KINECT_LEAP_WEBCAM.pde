import librarytests.*;
import org.openkinect.*;
import org.openkinect.processing.*;

import com.leapmotion.leap.Finger;
import com.leapmotion.leap.Hand;
import com.onformative.leap.LeapMotionP5;
/*

kinect webcam demo w/leap control

press 1 OR  1 FINGER OVER LEAP = MODE 1 = VGA CAM

press 2 OR  2 FINGERS OVER LEAP = MODE 2 = DEPTH CAM

press 3 OR  3 FINGERS OVER LEAP = MODE 3 = MATRIX CAM


*/

PImage img, imgMask;

// Kinect Library object
Kinect kinect;

//  point cloud
float a = 0;

// Scale up by 200
float factor = 666;
      
// Size of kinect image
int skip;
int w = 640;
int h = 480;
PVector v = new PVector();

// We'll use a lookup table so that we don't have to repeat the math over and over
float[] depthLookUp = new float[2048];
int[] depth = new int[307200];

//  point cloud

public int camMode = 1;  //  1, 2, 3
//  1  =  VGA cam          49
//  2  =  Depth cam        50
//  3  =  Point cloud????  51


//  LEAP FINGER DETECT

public int cX, cY;
public float offset = TWO_PI;


//  THIS SKETCH ONLY SUPPORTS ONE HAND
public PVector ftip1, ftip2, ftip3, ftip4, ftip5;

public Hand hand; 
public ArrayList<Finger> fingers;

LeapMotionP5 leap;

//  LEAP FINGER DETECT


////////////////////////////////////
void setup() { 
  size(640, 480, P3D);//800, 600,P3D);
  smooth();
 
  rectMode(CENTER);
  ellipseMode(CENTER);

  leap = new LeapMotionP5(this);
  
  cX = (int) width/2;
  cY = (int) height/2;
 
  kinect = new Kinect(this); 
  kinect.start(); 
  

  //  enable webcam action
  //  Color VGA Motion Camera 640x480 pixel resolution @30FPS
  kinect.enableRGB(true); 

      //  enable depth cam
      //  Depth Camera 640x480 pixel resolution 12@30FPS
      kinect.enableDepth(true);


  // Lookup table for all possible depth values (0 - 2047)
  for (int i = 0; i < depthLookUp.length; i++) {
    depthLookUp[i] = rawDepthToMeters(i);
  }

}


/*
Currently, the library makes data available to you in four ways: 
- RGB image from the kinect camera as a PImage. 
- Grayscale image from the IR camera as a PImage 
- Grayscale image with each pixel’s brightness mapped to depth (brighter = closer). 
- Raw depth data (11 bit numbers between 0 and 2048) as an int[] array
*/

///////////////////////////////////////////////
void draw(){
  
  try {
    //  get hand
    hand = leap.getHand(0);
    //  get detected fingers
    fingers = leap.getFingerList(hand);
  } catch( Exception exc )
  {
    println("NO HAND OR FINGERS? : " + exc );
  }

//  LISTEN FOR LEAP FINGERS
  
// DEBUG
//text( fingers.size(), 0, height-11);

//strokeWeight( (TWO_PI * fingers.size()) + HALF_PI );
//stroke( random(11,255), random(11,255), random(11,255) );

//  ALWAYS TRY GETTING THE FULL HAND
  try{ ftip1 = leap.getTip( fingers.get(0) ); }catch( Exception exc ){ ftip1 = null; }
  try{ ftip2 = leap.getTip( fingers.get(1) ); }catch( Exception exc ){ ftip2 = null; }
  try{ ftip3 = leap.getTip( fingers.get(2) ); }catch( Exception exc ){ ftip3 = null; }
//  try{ ftip4 = leap.getTip( fingers.get(3) ); }catch( Exception exc ){ ftip4 = null; }
//  try{ ftip5 = leap.getTip( fingers.get(4) ); }catch( Exception exc ){ ftip5 = null; }
  

    //  HOW MANY FINGERS AM I HOLDING UP?
    switch( fingers.size() )
    {
      
      case 1:
      {
      
        //  set camera mode
        camMode = 1;
      
      }
      break;
      
      case 2:
      {
        //  set camera mode
        camMode = 2;              
      }
      break;
      
      case 3:
      {
        //  set camera mode
        camMode = 3;

      }
      break;
      
      
      default :{
      }
      break;
    }

//  LISTEN FOR LEAP FINGERS

  switch( camMode ){
    /////////////////////////////////
    case 1 : {
println("MODE 1");
      
      
      image( kinect.getVideoImage(), 0, 0 );  //, width/2 - 320, height/2 - 240 );
//    background( kinect.getVideoImage() );
      
      
      
        strokeWeight(42);
        stroke(#EFEFEF);
        if(null != ftip1)  point( ftip1.x, ftip1.y );

  }
    break;
    //////////////////////////////////
    case 2 : {
println("MODE 2");            


      image( kinect.getDepthImage(), 0, 0);  //, width/2 - 320, height/2 - 240 );

//    background( kinect.getDepthImage() );

      
    }
    break;
    /////////////////////////////////
    case 3 : {
println("MODE 3");
      
      background(0);

      // Get the raw depth as array of integers
      depth = kinect.getRawDepth();
    
      // We're just going to calculate and draw every 4th pixel (equivalent of 160x120)
      int skip = 4;
    
      // Translate and rotate
      translate(width/2,height/2,-50);
      rotateY(a);
    int rawDepth, offset;
      for(int x=0; x<w; x+=skip) {
        for(int y=0; y<h; y+=skip) {
          offset = x+y*w;
    
          // Convert kinect data to world xyz coordinate
          rawDepth = depth[offset];
          v = depthToWorld(x,y,rawDepth);

    
          pushMatrix();
    
          translate(v.x*factor,v.y*factor,factor-v.z*factor);
          
          // Draw a point
          
          strokeWeight(1);
          stroke( random(255) ,random(255) ,random(255) );
          point(0,0);
          
          strokeWeight(.5);  
          stroke( 255 );
          point(0,0);
          
          strokeWeight(.25);
          stroke(random(42), 255, random(42));
          line( 0,0, random(TWO_PI), random(TWO_PI) );          
                    
          
// TODO: HYPE SWARM???          
          popMatrix();
        }
      }
    
    }
  
  
  
      //  TIPS
    strokeWeight(42);
    stroke(#EFEFEF);
    if(null != ftip1)  point( ftip1.x, ftip1.y );
    if(null != ftip2)  point( ftip2.x, ftip2.y );
    if(null != ftip3)  point( ftip3.x, ftip3.y );
//    if(null != ftip4)  point( ftip4.x, ftip4.y );
//    if(null != ftip5)  point( ftip5.x, ftip5.y );

  
  
  }

  
/*
THRESHOLD
Converts the image to black and white pixels depending if they are above or below the threshold defined by the level parameter. The parameter must be between 0.0 (black) and 1.0 (white). If no level is specified, 0.5 is used.

GRAY
Converts any colors in the image to grayscale equivalents. No parameter is used.

OPAQUE
Sets the alpha channel to entirely opaque. No parameter is used.

INVERT
Sets each pixel to its inverse value. No parameter is used.

POSTERIZE
Limits each channel of the image to the number of colors specified as the parameter. The parameter can be set to values between 2 and 255, but results are most noticeable in the lower ranges.

BLUR
Executes a Guassian blur with the level parameter specifying the extent of the blurring. If no parameter is used, the blur is equivalent to Guassian blur of radius 1. Larger values increase the blur.

ERODE
Reduces the light areas. No parameter is used.

DILATE
Increases the light areas. No parameter is used.
*/

  
//  STOP WHEN ESCAPE IS HIT
  if (keyPressed && keyCode == ESC) {

    save("kinectcam_"+hour()+minute()+second()+millis()+".png");
    draw();
    stop();
  }


}

void keyPressed(){

  switch( keyCode ){
  
      //  1  =  VGA cam          49
      case 49 : {
        camMode = 1;
      }
      break;
      
      //  2  =  Depth cam        50
      case 50 : {
        camMode = 2;
      }
      break;
      
      //  3  =  Point cloud????  51
      case 51 : {
        camMode = 3;
      }
      break;
  }

}



// These functions come from: http://graphics.stanford.edu/~mdfisher/Kinect.html
float rawDepthToMeters(int depthValue) {
  if (depthValue < 2047) {
    return (float)(1.0 / ((double)(depthValue) * -0.0030711016 + 3.3309495161));
  }
  return 0.0f;
}

PVector depthToWorld(int x, int y, int depthValue) {

  final double fx_d = 1.0 / 5.9421434211923247e+02;
  final double fy_d = 1.0 / 5.9104053696870778e+02;
  final double cx_d = 3.3930780975300314e+02;
  final double cy_d = 2.4273913761751615e+02;

  PVector result = new PVector();
  double depth =  depthLookUp[depthValue];//rawDepthToMeters(depthValue);
  result.x = (float)((x - cx_d) * depth * fx_d);
  result.y = (float)((y - cy_d) * depth * fy_d);
  result.z = (float)(depth);
  return result;
}

void stop() {
  kinect.quit();
  super.stop();
}

