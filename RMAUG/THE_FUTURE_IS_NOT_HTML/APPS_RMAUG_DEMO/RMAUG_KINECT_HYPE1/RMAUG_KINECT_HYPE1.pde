import org.openkinect.*;
import org.openkinect.processing.*;

//  Kinect webcam
//  TAB = save frame
//  ESC = saves frame and stops sketch

// Kinect Library object
Kinect kinect;
float deg = 15; // Start at 15 degrees

//  HYPE STUFF
HDrawablePool pool;

int cellSize = 10;
PImage img;
HPixelColorist colors;
//  HYPE STUFF


void setup() { 

  size(640, 480);// ,P3D);
  
  smooth();
  
  H.init(this).background(#202020);
   
  //  Get the Kinect and start it up
  kinect = new Kinect(this);
  kinect.start();

  kinect.enableRGB(true);
  kinect.enableDepth(false);
  kinect.processDepthImage(false);

  kinect.tilt(deg);


}

void draw() {


  if( frameCount % 15 == 0 ) {
    
    img = kinect.getVideoImage();
  
    colors = new HPixelColorist(img).fillAndStroke();
  
      
    pool = new HDrawablePool( (width/cellSize)*(height/cellSize)  );
    pool.autoAddToStage()
      .add (
        new HRect()
        .rounding(random(1,11))
      )
      .layout (
        new HGridLayout()
        .startX(16)
        .startY(16)
        .spacing(cellSize+HALF_PI,cellSize+HALF_PI)
        .cols((width/cellSize))
  
      )
        .setOnCreate (
          new HCallback(){
            public void run(Object obj) {
              
              HDrawable d = (HDrawable) obj;
                  d.anchorAt(H.CENTER)
                   .size(cellSize)
              ;
  
                colors.applyColor(d);
            }
        }
        )
      .requestAll();


    H.drawStage();
  }

}



//  KEY HANDLER  //
//////////////////////////////
//  Key press handler
void keyPressed(){

  
  //  CAM MODE SWITCHER
  switch( keyCode ){
      case ESC: {
          saveFrame("STOP-###########.png");
          stop();     
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

