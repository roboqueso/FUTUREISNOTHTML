/*

  Put all KEYBOARD   centric code here

*/



//  keyPressed  ///////////////////////////////////////////////////////////////
void keyPressed() {


  switch(key){
    //  set strokeSize
    case '1':
    case '2':
    case '3':
    case '4':
    case '5':
    case '6':
    case '7':
    case '8':
    case '9':
    {
      brushSz = key-48; //  NOTE: I'm not sure about the number key offset thingy yet, but this gives you the correct number
    }
    break;


    //  set brush type
    case 'e':
    case 'l':
    case 'p':
    case 'r':
    {
      brush = key;
    }
    break;

    //  drawing options
    case 'c':{
      bColor = !bColor;
    }
    break;

    case 'f':{
      bFill = !bFill;
    }
    break;


    case 's':{
      bSmooth = !bSmooth;
    }
    break;

    case 'h':{
      showHelp();
    }
    break;

    //  WRITE image to disk
    case 'w':{
        fileName = "fmd"+ month() +day() + year() + hour() + minute() + millis()+".png";
        save( fileName );
        //  BEEP IS GOOD!
        java.awt.Toolkit.getDefaultToolkit().beep();
        // println(fileName + " saved!");
    }
    break;

    case ' ':{
      penDown = false;
    }
    break;


    //  erase drawing
    case DELETE:
    {
      setBackground();
    }
    break;

    default :
      println("# press : '" + key + "' : " + keyCode  );
    break;
  }
}


//  keyReleased ///////////////////////////////////////////////////////////////
void keyReleased()
{
  if( key == ' '){
    penDown = true;
  }
}

