/*

  Put all MOUSE centric code here

*/



//  mouseMoved  ///////////////////////////////////////////////////////////////
void mouseMoved() {


//TODO: add keyPressed event listener to capture spacebar keyUP?
//  UPDATE: PEN UP IF SPACEBAR IS DOWN
if( penDown )
{

  //  smooth ?
  if(bSmooth){
    smooth();
  } else {
    noSmooth();
  }
  

  //  set default color
  stroke(random(255));  //  default random B&W


  //  fill the shapes?
  if(bFill){
    //  fill with color or B&W?
    if(bColor)
      fill(random(255),random(255),random(255));
    else
      fill(frameCount%2==0?0:255);
  } else {
    noFill();
  }

  //  handle stroke size
  strokeWeight( brushSz );
  
  //  draw with appropriate "brush"
  switch(brush){

    case 'e':
    {
      //  always strokes B&W
      ellipse( mouseX, mouseY, brushSz*PI, brushSz*PI );
      //  set UI
      // lbBrush.setValue(0);
    }
    break;

    case 'l':
    {

      if(bColor)
        stroke(random(255),random(255),random(255));
      else
        stroke(frameCount%2==0?0:255);

      line( mouseX, mouseY, pmouseX, pmouseY );
      //  set UI
      // lbBrush.setValue(1);
    }
    break;

    case 'p':
    {

      if(bColor)
        stroke(random(255),random(255),random(255));
      else
        stroke(frameCount%2==0?0:255);

      point( mouseX, mouseY);
      //  set UI
      // lbBrush.setValue(2);
    }
    break;

    case 'r':{
      //  always strokes B&W
      rect( mouseX, mouseY, brushSz*4, brushSz*4 );
      //  set UI
      // lbBrush.setValue(3);
    }
    break;
  }
  //  brush


} else {
  // println("SPACEBAR -- PEN UP");
}  


}
