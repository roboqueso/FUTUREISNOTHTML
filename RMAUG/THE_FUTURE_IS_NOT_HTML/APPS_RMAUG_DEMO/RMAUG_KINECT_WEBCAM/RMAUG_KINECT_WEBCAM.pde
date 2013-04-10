import org.openkinect.*;
import org.openkinect.processing.*;
/*
Simple KINECT webcam demo.
Switch camera modes by pressing 1, 2, or 3 on your keyboard
*/


// Kinect Library object
Kinect kinect;
PImage imgVga, imgDepth;

//  Key  Mode          P5 keyCode
//  1    VGA cam       49
//  2    Depth cam     50
//  3    Crazytown     51
public int camMode = 1;

float deg = 15; // Start at 15 degrees

void setup() { 

  size(640, 480 ,P3D);
  smooth();
 
  //  Get the Kinect and start it up
  kinect = new Kinect(this);
  kinect.start();
  
  //  we don't NEED all of this, it's just fun for the demo
  kinect.enableDepth(true);
  kinect.enableRGB(true);
  kinect.enableIR(false);
  kinect.tilt(deg);

}



void draw(){

  //  which mode is the camera in?
  switch( camMode )
  {
    //  RGB 
    case 1 : {
      
      
      image( kinect.getVideoImage(), 0, 0 );
      

  }
    break;


    //  DEPTH
    case 2 : {

      image( kinect.getDepthImage(), 0, 0 );
      
    }
    break;
    
    //  IR CAM
    case 3:{

      
      kinect.enableIR(true);
      
      
      image( kinect.getDepthImage(), 0, 0 );
      
    }
  
  }


}



//////////////////////////////
//  Key press handler
void keyPressed(){

  //  STOP WHEN ESCAPE IS HIT
  if ( ESC == keyCode) {

    save("kinectcam_"+hour()+minute()+second()+millis()+".png");

    stop();
  }

  
  
  //  CAM MODE SWITCHER
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
      
      //  3  =  mask mode        51
      case 51 : {
        camMode = 3;
      }
      break;
     
      case UP : {

          deg++;
          
          tilt(deg);
      }
     break; 
     
     case DOWN: {
          deg--;
          
          tilt(deg);
     }
     break;
  }





}

//  KINECT TILT CAMERA HANDLER
void tilt( float deg ){
  
  if( null != kinect ) {
    
    deg = constrain(deg,0,30);
    kinect.tilt(deg);

  }
}

void stop() {
  kinect.quit();
  super.stop();
}

