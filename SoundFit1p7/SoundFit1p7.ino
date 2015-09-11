//
//  Arduino / Adafruit shield function code
// will be resident in eeprom in Arduino and execute operations based on host commands
//
//  Version 0.2 2/9/13
//  rev 0.3 added 2/28/13
//  added light control 3/9/13 rev 0.4
//  fixed switch hysterisis 3/14/13 v0.5
//  added motor home to reset, and 12V sense to ready 3/18/13  v0.6
//  v0.7 10 deg elev steps amm
//  dcr

#include <AFMotor.h>  // stepper motor library

// motor port #1 is elevation motor, with 48 steps per rev, and 1/16 gearing = 16*48 = 768 steps per shaft rotation
// motor port #2 is rotation motor with 200 steps per rev

AF_Stepper motor1(48, 1);
AF_Stepper motor2(200, 2);



//  DEFINE commands
const int  S = int('S');  // reSet
const int  C = int('C');  // Check ready
const int  L = int('L');  // rotate Left
const int  R = int('R');  // rotate Right
const int  U = int('U');  // elevate Up
const int  D = int('D');  // elevate Down
const int  z = int('z');  // check rotation zero position
const int  Z = int('Z');  // check elevation zero position
const int  h = int('h');  // home to rotation zero position
const int  H = int('H');  // home to elevation zero position
const int  N = int('N');  // light oN
const int  F = int('F');  // light ofF

// define useful constants
const int   Rlim = 200;    // rotation step count limit (one shaft rotation)
const int   Elim = 192;    // elevation step count limit (1/4 shaft rotation)
const int   rstep = 8;     // number of revolution motor small steps per large step command  (change later)
const int   estep = 11;    // number of elevation motor small steps per large step command   (change later)
const int   limval = 500;  // a/d converter limit value to decide if limit switch encountered  (==============CHANGE LATER================)
const int   elevpin = A1;   // a/d converter channel pin for elevation sense
const int   rotatepin = A2; // a/d converter channel pin for rotation sense
const int   lightpin = A0;  // light control analog output
const int   motorpwr = A3;  // motor power (12V) sense
const int   pwrlevel = 500; // 12V active level


const int   left = FORWARD;
const int   right = BACKWARD;
const int   up = BACKWARD; // FORWARD;
const int   down = FORWARD; // BACKWARD;


// define global working variables
int     serinp;       // input from serial port
int     rotatecount;  // count of rotation steps from zero position
int     elevcount;    // count of elevation steps from zero position


void setup()
{  
  int rval;

  motor1.setSpeed(50);  // elevation
  motor2.setSpeed(30);  // rotation

  Serial.begin(9600);           // set up Serial library at 9600 bps
  Serial.flush();
  
  pinMode(A1,INPUT);   // elevation sense
  pinMode(A2,INPUT);   // rotation sense
  pinMode(A3,INPUT);   // 12V sense
  pinMode(A0,OUTPUT);  // light control

  rval = LightOff();

  rotatecount = 0;
  elevcount = 0;
}


// define routines to use


//
//=======================================================================================
int  Reset()  {
  int rval;

  rval = HomeElev();
  rval = HomeRotate(); 
  delay(100);
  motor1.release();
  motor2.release();
  elevcount = 0;
  rotatecount = 0;
  
  rval = LightOff();
  return 0;
}

//---------------------------------------------------------------------------------------

//
//=======================================================================================
//  CheckRotateZero - tests if rotation zero limit reached
//      returns: 0 = no limit sensed
//               1 = limit sensed
int CheckRotateZero() {
  int aval;
  // read the analog value on the rotation sense input pin
  aval = 0;
  aval = analogRead(rotatepin);
  if(aval > limval){
    return 0;
  } 
  else {
    return 1;
  }  
}

//---------------------------------------------------------------------------------------

//
//=======================================================================================
//  CheckElevZero - tests if rotation zero limit reached
//      returns: 0 = no limit sensed
//               1 = limit sensed
int CheckElevZero() {
  int aval;
  // read the analog value on the elevation sense input pin
  aval = 0;
  aval = analogRead(elevpin);
  if(aval > limval){
    return 0;
  } 
  else {
    return 1;
  }  
}


//---------------------------------------------------------------------------------------

//
//=======================================================================================
//  CheckReady - tests to see if motors, etc. ok
//      returns: 0 = no error, 1 = no 12V power

//---------------------------------------------------------------------------------------
int CheckReady() {
  int aval;
  // check value of 12V sensor
  aval = analogRead(motorpwr);
  if(aval < pwrlevel) {
    return 1;
  } else {
    return 0;
  }
}

//
//=======================================================================================
//  RotateLeft - turns left specified number of large steps, 
//      returns: 0 = command executed ok
//               1 = limit sensed
//               2 = overcount (rotation limit exceeded)
//               3 = other (not used presently)
int RotateLeft(int steps)  {
  int smstps;  // small step counter
  int lrgstps; // large step counter
  int lrgtst;
  int smtst;
  int rval;

  rval = 0;

  if(rotatecount >= 0) {  // see if we can move from here
    lrgstps = 0;
    lrgtst = 0;
    do { // for large steps
      smstps = 0;
      do {  // for small steps
        motor2.step(1,left,DOUBLE);
        smstps += 1;
        rotatecount -= 1;  //  left rotation decrements counter
        if(smstps > Rlim) rval = 2;
        if(rotatecount > Rlim) rval = 2;
        if(rotatecount <= 0) rval = 2;
        if(CheckRotateZero() > 0) rval = 1;  
      } while ((rval == 0)&&(smstps < rstep)); // one set of small steps
      lrgstps +=1;
      if(lrgstps >= steps) lrgtst = 1;      
    } while ((rval == 0)&&(lrgtst < 1));  // for the number of large steps

  } 
  // motor2.release();
  return rval;  
}

//---------------------------------------------------------------------------------------

//
//=======================================================================================
//  RotateRight - turns right specified number of steps, 
//      returns: 0 = command executed ok
//               1 = limit sensed
//               2 = overcount (rotation limit exceeded)
//               3 = other (not used presently)
int RotateRight(int steps)  {
  int smstps;  // small step counter
  int lrgstps; // large step counter
  int lrgtst;
  int smtst;
  int rval;

  rval = 0;

  if(rotatecount <= Rlim) {  // see if we can move from here
    lrgstps = 0;
    lrgtst = 0;
    do { // for large steps
      smstps = 0;
      do {  // for small steps
        motor2.step(1,right,DOUBLE);
//        delay(10);   // try slowing down small steps for smoother transit
        
        smstps += 1;
        rotatecount += 1;  //  right rotation increments counter
        if(smstps > Rlim) rval = 2;
        if(rotatecount >= Rlim) rval = 2;
        if(rotatecount < 0) rval = 2;
//        if(CheckRotateZero() > 0) rval = 1;  
      } while ((rval == 0)&&(smstps < rstep)); // one set of small steps
      lrgstps +=1;
      if(lrgstps >= steps) lrgtst = 1;      
    } while ((rval == 0)&&(lrgtst < 1));  // for the number of large steps

  } 
  // motor2.release();
  return rval;  
}


//---------------------------------------------------------------------------------------

//
//=======================================================================================
//  Up - moves elevation arm up specified number of steps, 
//      returns: 0 = command executed ok
//               1 = limit sensed
//               2 = overcount (rotation limit exceeded)
//               3 = other (not used presently)
int Up(int steps)  {
  int smstps;  // small step counter
  int lrgstps; // large step counter
  int lrgtst;
  int smtst;
  int rval;

  rval = 0;

  if(elevcount >=0) {  // see if we can move from here
    lrgstps = 0;
    lrgtst = 0;
    do { // for large steps
      smstps = 0;
      do {  // for small steps
        motor1.step(1,up,DOUBLE);
        smstps += 1;
        elevcount -= 1;  //  up decrements counter
        if(smstps > Elim) rval = 2;
        if(elevcount > Elim) rval = 2;
        if(elevcount < 0) rval = 2;
        if(CheckElevZero() > 0) rval = 1;  
      } while ((rval == 0)&&(smstps < estep)); // one set of small steps
      lrgstps +=1;
      if(lrgstps >= steps) lrgtst = 1;      
    } while ((rval == 0)&&(lrgtst < 1));  // for the number of large steps

  } 
  // motor1.release();
  return rval;  
}


//---------------------------------------------------------------------------------------

//
//=======================================================================================
//  Down - moves elevation arm down specified number of steps, 
//      returns: 0 = command executed ok
//               1 = limit sensed
//               2 = overcount (rotation limit exceeded)
//               3 = other (not used presently)
int Down(int steps)  {
  int smstps;  // small step counter
  int lrgstps; // large step counter
  int lrgtst;
  int smtst;
  int rval;

  rval = 0;

  if(elevcount <= Elim) {  // see if we can move from here
    lrgstps = 0;
    lrgtst = 0;
    do { // for large steps
      smstps = 0;
      do {  // for small steps
        motor1.step(1,down,DOUBLE);
        smstps += 1;
        elevcount += 1;  //  down increments counter
        if(smstps > Elim) rval = 2;
        if(elevcount > Elim) rval = 2;
        if(elevcount < 0) rval = 2;
//        if(CheckElevZero() > 0) rval = 1;  
      } while ((rval == 0)&&(smstps < estep)); // one set of small steps
      lrgstps +=1;
      if(lrgstps >= steps) lrgtst = 1;      
    } while ((rval == 0)&&(lrgtst < 1));  // for the number of large steps

  } 
  // motor1.release();
  return rval;  
}


//---------------------------------------------------------------------------------------

//
//=======================================================================================
//  HomeRotate - rotates to home position
//      returns: 0 = command executed ok
//               1 = limit sensed
//               2 = overcount (rotation limit exceeded)
//               3 = other (not used presently)
int HomeRotate() {
  int test;
  int rval;
  rval = 0;
  test = 0;
  do {
    if(CheckRotateZero() > 0) {
      rval = 1;  
    } else {    
      motor2.step(1,left,DOUBLE);
      test += 1;
      if(test > Rlim) rval = 2;
    }  
  } while (rval == 0);
  if(rval == 1) {
    rval = 0;
    rotatecount = 0;
  }  
  // motor2.release();
  return rval;  
}  

//---------------------------------------------------------------------------------------

//
//=======================================================================================
//  HomeElev - moves elevation arm to home position
//      returns: 0 = command executed ok
//               1 = limit sensed
//               2 = overcount (rotation limit exceeded)
//               3 = other (not used presently)
int HomeElev() {
  int test;
  int rval;
  rval = 0;
  test = 0;
  do {
    if(CheckElevZero() > 0) {
      rval = 5;  
//      Serial.print("elev home zero check, result = ");
//      Serial.println(rval,DEC);
    } else {  
      motor1.step(1,up,DOUBLE);
      test += 1;  
      if(test > Elim) rval = 2;
 //     {
 //       Serial.print("elev home count check, result = ");
 //       Serial.println(test,DEC);
 //       rval = 2;
 //     }  
    }  
  } while (rval == 0);
  if(rval == 1) { 
    rval = 0;
    elevcount = 0;
  } 
  // motor1.release();
  return rval;  
}  

//---------------------------------------------------------------------------------------

//
//=======================================================================================
//  LightOn - turns the interior illumination on
//      returns: 0 = no error

//---------------------------------------------------------------------------------------
int LightOn() {
  analogWrite(lightpin,255);
  return 0;
}

//
//=======================================================================================
//  LightOff - turns the interior illumination off
//      returns: 0 = no error

//---------------------------------------------------------------------------------------
int LightOff() {
  analogWrite(lightpin,0);
  return 0;
}

//
//=======================================================================================
//

//---------------------------------------------------------------------------------------

//
//=======================================================================================
//

//---------------------------------------------------------------------------------------



// wait for command from host, then do:
/*  commands are single characters
 const char  S = "S";  // reSet
 const char  C = "C";  // Check ready
 const char  L = "L";  // rotate Left
 const char  R = "R";  // rotate Right
 const char  U = "U";  // elevate Up
 const char  D = "D";  // elevate Down
 const char  z = "z";  // check rotation zero position
 const char  Z = "Z";  // check elevation zero position
 const char  h = "h";  // home to rotation zero position
 const char  H = "H";  // home to elevation zero position
 const char  N = "N";  // light oN
 const char  F = "F";  // light ofF
*/

void loop()  {
  int rval;
  serinp = 0;
  while (Serial.available() > 0) {
    serinp = Serial.read();   
    switch (serinp) {
      case S:
        {
          rval = Reset();
          rval = 0;
          Serial.print("reset, result = ");
          Serial.println(rval,DEC);
          break; 
        }
      case C:
        {
          rval = CheckReady();
          Serial.print("ready check, result = ");
          Serial.println(rval,DEC);
          break; 
        }
      case L:
        {
          rval = RotateLeft(1);
          Serial.print("rotate left, result = ");
          Serial.println(rval,DEC);
          break; 
        }
      case R:
        {
          rval = RotateRight(1);
          Serial.print("rotate right, result = ");
          Serial.println(rval,DEC);
          break; 
        }
      case U:
        {
          rval = Up(1);
          Serial.print("up, result = ");
          Serial.println(rval,DEC);
          break; 
        }
      case D:
        {
          rval = Down(1);
          Serial.print("down, result = ");
          Serial.println(rval,DEC);
          break; 
        }
      case z:
        {
          rval = CheckRotateZero();
          Serial.print("check rotate zero, result = ");
          Serial.println(rval,DEC);
          break; 
        }
      case Z:
        {
          rval = CheckElevZero();
          Serial.print("check elevation zero, result = ");
          Serial.println(rval,DEC);
          break; 
        }
      case h:
        {
          rval = HomeRotate();
          Serial.print("home rotate, result = ");
          Serial.println(rval,DEC);
          rotatecount = 0;
          break; 
        }
      case H:
        {
          rval = HomeElev();
          Serial.print("home elevation, result = ");
          Serial.println(rval,DEC);
          elevcount = 0;
          break; 
        }
      case N:
        {
          rval = LightOn();
          Serial.print("light ON, result = ");
          Serial.println(rval,DEC);
          break; 
        }
      case F:
        {
          rval = LightOff();
          Serial.print("light off, result = ");
          Serial.println(rval,DEC);
          break; 
        }
      default: 
        {
          Serial.println("unrecognized command");
        }
      }  // end switch  

  }  // end while    

} 



