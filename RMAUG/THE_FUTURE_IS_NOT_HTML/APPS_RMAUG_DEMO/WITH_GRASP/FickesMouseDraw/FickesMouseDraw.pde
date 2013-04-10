//import controlP5.*;
//  ---------------------------------------------------------------------------
//  
//  FickesMouseDraw by ericfickes.com
//  
//  This is a simple drawing tool powered by the mouseDraw and keyPressed events.
//  You will draw whenever you move your mouse.  No magic exists in the main draw() function
//  You can control the brush type, brush size, fill, and smooth by pressing the appropriate
//  first letter on your keyboard.
//  Usage :
//  brush   : e = ellipse, l = line, p = point, r = rect
//  brushSz : keys 1 - 9
//  fill    : f to toggle ( fill only works with ellipse and rect )
//  smooth  : s to toggle
//  erase screen : fn + delete
//  stroke color : c to toggle B&W or random colors ( * ellipse and rect get fill color, line and point get stroke color )
//  pen down : toggle with spacebar. This allows you to move the cursor without drawing
//  redraw help : h redraws the top help box
//  WRITE PNG : save your sketch as a PNG by pushing w
//  
//  ---------------------------------------------------------------------------
char brush = 'l';         //  current brush
int brushSz = 1;          //  strokeSize
Boolean bFill = false;    // to fill or not to fill
Boolean bSmooth = false;  // should we be smooth?
Boolean bColor = false;   // B&W or random color?
color bgColor = #000000;  // background color gets used in setup and in keyPressed
Boolean penDown = true;    //  is the pen up or are you drawing? ( toggle with spacebar )
String fileName = "";

//  UI
//ControlP5 cp5;
//ListBox lbBrush;


//  SETUP   ///////////////////////////////////////////////////////////////////
void setup() {
  // setup core sketch settings items
  size( 900, 900 );  //  P2D, P3D, OPENGL, PDF

  ellipseMode(CENTER);
  // rectMode(CENTER);
  fill(#EF0000);
  
  setBackground();
  
//  cp5 = new ControlP5(this);



}



//  DRAW  /////////////////////////////////////////////////////////////////////
void draw()
{
  //  Typically we'd be drawing inside of this function since it's the main 
  //  looper.  However, for this example we're running completely off the
  //  built in mouseMoved() and keyPressed() events
}

//  showHelp  /////////////////////////////////////////////////////////////////
void showHelp() {
  noStroke();
  fill(0);
  rect(0, 0, width, 21);
  fill(#aFaFaF);
  text("HELP: brush: e,l,p,r | strokeSz: 1-9 | color: c | fill: f | smooth: s | help: h | pen up: SPACE | Save: w | erase:[fn]+DELETE ", 0, 12);
}



//  setBackground /////////////////////////////////////////////////////////////
void setBackground(){

  background(bgColor);

  showHelp();    

  fill(#EF0000);
  text("ericfickes.com", 0, height);
}

