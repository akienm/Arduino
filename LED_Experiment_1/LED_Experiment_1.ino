
// Init the Pins used for PWM
const int redPin   = 9;
const int greenPin = 10;
const int bluePin  = 11;

// commands
const int  C = int('C');  // Check ready
const int  S = int('S');  // Red+
const int  R = int('R');  // Red+
const int  r = int('r');  // Red-
const int  G = int('G');  // Green+
const int  g = int('g');  // Green-
const int  B = int('B');  // Blue+
const int  b = int('b');  // Blue-

// define global working variables
int serinp;       // input from serial port
int currentColorValueRed = 255;
int currentColorValueGreen = 255;
int currentColorValueBlue = 255;

void setup()
{  
  Serial.begin(9600);           // set up Serial library at 9600 bps
  Serial.flush();

  pinMode(redPin, OUTPUT);
  pinMode(greenPin, OUTPUT);
  pinMode(bluePin, OUTPUT);
}


// wait for command from host, then do:
/*  commands are single characters
 const int  C = int('C');  // Check ready
 */

void loop()  {
  int rval;
  serinp = 0;
  analogWrite(redPin,   currentColorValueRed);
  analogWrite(greenPin, currentColorValueGreen); 
  analogWrite(bluePin,  currentColorValueBlue);  
  while (Serial.available() > 0) {
    analogWrite(redPin,   currentColorValueRed);
    analogWrite(greenPin, currentColorValueGreen); 
    analogWrite(bluePin,  currentColorValueBlue);  
    serinp = Serial.read();   
    analogWrite(redPin,   currentColorValueRed);
    analogWrite(greenPin, currentColorValueGreen); 
    analogWrite(bluePin,  currentColorValueBlue);  
    switch (serinp) {
    case C:
      {
        Serial.print("Ready!\n");
        break; 
      }
    case S:
      {
        currentColorValueRed = 255;
        currentColorValueGreen = 255;
        currentColorValueBlue = 255;      }      
        Serial.print("RESET\n");
        break; 
    case R: 
      {
        currentColorValueRed++;
        if(currentColorValueRed > 255)
          currentColorValueRed = 0;
        Serial.print("Red+ OK: ");
        Serial.println(currentColorValueRed,DEC);
        break; 
      }
    case r: 
      {
        currentColorValueRed--;
        if(currentColorValueRed < 0)
          currentColorValueRed = 255;
        Serial.print("Red- OK: ");
        Serial.println(currentColorValueRed,DEC);
        break; 
      }
    case G: 
      {
        currentColorValueGreen++;
        if(currentColorValueGreen > 255)
          currentColorValueGreen = 0;
        Serial.print("Green+ OK: ");
        Serial.println(currentColorValueGreen,DEC);
        break; 
      }
    case g: 
      {
        currentColorValueGreen--;
        if(currentColorValueGreen < 0)
          currentColorValueGreen = 255;
        Serial.print("Green- OK: ");
        Serial.println(currentColorValueGreen,DEC);
        break; 
      }
    case B: 
      {
        currentColorValueBlue++;
        if(currentColorValueBlue > 255)
          currentColorValueBlue = 0;
        Serial.print("Blue+ OK: ");
        Serial.println(currentColorValueBlue,DEC);
        break; 
      }
    case b: 
      {
        currentColorValueBlue--;
        if(currentColorValueBlue < 0)
          currentColorValueBlue = 255;
        Serial.print("Blue- OK: ");
        Serial.println(currentColorValueBlue,DEC);
        break; 
      }
    default: 
      {
        Serial.println("unrecognized command\n");
      }
    }  // end switch  

    analogWrite(redPin,   currentColorValueRed);
    analogWrite(greenPin, currentColorValueGreen); 
    analogWrite(bluePin,  currentColorValueBlue);  
  }  // end while    

} 




