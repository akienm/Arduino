

const int  c = int('c'); // check ready
const int  d = int('d'); // down
const int  l = int('l'); // launch demo
const int  m = int('m'); // max on
const int  o = int('o'); // off
const int  r = int('r'); // reset
const int  u = int('u'); // up
const int  l1 = int('1'); // 1800K
const int  l3 = int('2'); // 2700K
const int  l5 = int('5'); // 5000K

// HAND TUNED FOR BETTER FADE FROM 0 TO NONZERO ON GREEN AND BLUE
int kLoForTable = 420;
int kHiForTable = 5000;
int stepSize = 18;
int indexTotal = 254;
int rTable[] = {255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255};
int gTable[] = {0, 0, 0, 0, 0, 0, 1, 2, 3, 5, 7, 9, 11, 14, 17, 20, 23, 26, 29, 31, 34, 37, 39, 42, 44, 46, 49, 51, 53, 55, 58, 60, 62, 64, 66, 68, 70, 72, 73, 75, 77, 79, 81, 82, 84, 86, 87, 89, 90, 92, 94, 95, 97, 98, 99, 101, 102, 104, 105, 106, 108, 109, 110, 112, 113, 114, 115, 117, 118, 119, 120, 121, 123, 124, 125, 126, 127, 128, 129, 130, 131, 132, 133, 134, 135, 136, 137, 138, 139, 140, 141, 142, 143, 144, 145, 146, 147, 148, 149, 149, 150, 151, 152, 153, 154, 154, 155, 156, 157, 158, 158, 159, 160, 161, 161, 162, 163, 164, 164, 165, 166, 167, 167, 168, 169, 169, 170, 171, 171, 172, 173, 173, 174, 175, 175, 176, 177, 177, 178, 178, 179, 180, 180, 181, 182, 182, 183, 183, 184, 184, 185, 186, 186, 187, 187, 188, 188, 189, 189, 190, 190, 191, 192, 192, 193, 193, 194, 194, 195, 195, 196, 196, 197, 197, 198, 198, 198, 199, 199, 200, 200, 201, 201, 202, 202, 203, 203, 204, 204, 204, 205, 205, 206, 206, 207, 207, 207, 208, 208, 209, 209, 209, 210, 210, 211, 211, 211, 212, 212, 213, 213, 213, 214, 214, 215, 215, 215, 216, 216, 216, 217, 217, 218, 218, 218, 219, 219, 219, 220, 220, 220, 221, 221, 221, 222, 222, 222, 223, 223, 223, 224, 224, 224, 225, 225, 225, 226, 226, 226, 227, 227, 227, 228, 228};
int bTable[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 2, 3, 5, 7, 10, 13, 16, 19, 22, 25, 27, 29, 31, 33, 34, 36, 38, 40, 42, 43, 45, 47, 48, 50, 52, 53, 55, 57, 58, 60, 61, 63, 65, 66, 68, 69, 71, 72, 74, 75, 76, 78, 79, 81, 82, 84, 85, 86, 88, 89, 90, 92, 93, 94, 96, 97, 98, 99, 101, 102, 103, 104, 106, 107, 108, 109, 110, 112, 113, 114, 115, 116, 117, 119, 120, 121, 122, 123, 124, 125, 126, 127, 128, 129, 131, 132, 133, 134, 135, 136, 137, 138, 139, 140, 141, 142, 143, 144, 145, 146, 147, 148, 149, 149, 150, 151, 152, 153, 154, 155, 156, 157, 158, 159, 160, 160, 161, 162, 163, 164, 165, 166, 167, 167, 168, 169, 170, 171, 172, 172, 173, 174, 175, 176, 176, 177, 178, 179, 180, 180, 181, 182, 183, 183, 184, 185, 186, 186, 187, 188, 189, 189, 190, 191, 192, 192, 193, 194, 195, 195, 196, 197, 197, 198, 199, 199, 200, 201, 202, 202, 203, 204, 204, 205, 206};

int redPin = 9;
int greenPin = 10;
int bluePin = 11;

int serinp;  // serial input
int index;   // index into the various data
int calcedIndex; // calced index into sub table
int currentTemp; // current index in Kelvin
int rValue;  // red LED value
int gValue;  // green LED value
int bValue;  // blue LED value
int redWidth = 255;  // width of first data set (red only segment)

int demoflag;
int demodirection;

int CalculateIndexFromKelvin(int value)
{
  int oldValue = 0;

  if(value > kLoForTable)
  {
    int i = -1;
    int newValue = 0;
  
    while(i < indexTotal && newValue < value)
    {
      oldValue = i;
      i = i + 1;
      newValue = stepSize * i + kLoForTable;
    }
    
    oldValue = oldValue+redWidth;
  }
 
  return oldValue;
}

void CalculateLEDValues() 
{
  if(index <= redWidth) 
  {
    rValue = index;
    gValue = 0;
    bValue = 0;
  }
  else
  {
    calcedIndex = index - redWidth;
    rValue = rTable[calcedIndex];
    gValue = gTable[calcedIndex];
    bValue = bTable[calcedIndex];
  }
  
  if(index <= redWidth) 
  {
    Serial.println(index,DEC);
  }
  else
  {
    Serial.println(kLoForTable + ((index - redWidth) * stepSize),DEC);
  }
    
  analogWrite(redPin, rValue);
  analogWrite(greenPin, gValue);
  analogWrite(bluePin, bValue);
}

void up()
{
  if(index < (indexTotal + redWidth - 1)) 
  {
    Serial.print("UP!\n");
    index++; 
    CalculateLEDValues();
  }
  else
  {
    Serial.print("UP! Er, no. Already at max\n");
    if(demodirection == 1) demodirection = 0;
    if(demoflag == 1)
    {
      analogWrite(bluePin, 255);
      analogWrite(redPin, 0);
      analogWrite(greenPin, 0);
      delay(50);
    }
  }
}

void down()
{
  if(index > 0) 
  {
    Serial.print("DOWN!\n");
    index--;
    CalculateLEDValues();
  }
  else
  {
    Serial.print("DOWN! Er, no. Already at min\n");
    if(demodirection == 0) demodirection = 1;
  }
}

void reset()
{
  index = 0;
  currentTemp = kLoForTable;
  demoflag = 0;
  CalculateLEDValues();
  Serial.println("Reset complete\n");
}

void demo()
{
  demoflag = 1;  
  demodirection = 1;
}

void demoaction()
{
  if(demoflag==1)
  {
    if(demodirection == 1) 
    {
      up();
    }
    else
    {
      down();
    }

    if(index > redWidth)
    {
      delay(50);
    }
  }
}

void maxon()
{
   analogWrite(bluePin, 255);
   analogWrite(redPin, 255);
   analogWrite(greenPin, 255);
}

void setup()
{
  Serial.begin(9600);           // set up Serial library at 9600 bps
  Serial.flush();
  
  pinMode(redPin, OUTPUT);
  pinMode(greenPin, OUTPUT);
  pinMode(bluePin, OUTPUT);

  reset(); 
  demo(); 
}

void loop() 
{

  serinp = 0;
  while (Serial.available() > 0) 
  {
    serinp = Serial.read();  
    switch (serinp) 
    {
      case c: 
      {
        demoflag = 0;
        Serial.print("Ready!\n");
        break; 
      }
      case u: 
      {
        demoflag = 0;
        up();
        break; 
      }
      case d: 
      {
        demoflag = 0;
        down();
        break;
      } 
      case o:
      {
        demoflag = 0;
        reset();
        break;
      }
      case r:
      {
        demoflag = 0;
        reset();
        break;
      }
      case l:
      {
        demoflag = 0;
        demo();
        break;
      }
      case m:
      {
        demoflag = 0;
        maxon();
        break;
      }
      case l1:
      {
        demoflag = 0;
        index = CalculateIndexFromKelvin(1800);
        CalculateLEDValues();
        break;
      }
      case l3:
      {
        demoflag = 0;
        index = CalculateIndexFromKelvin(2800);
        CalculateLEDValues();
        break;
      }
      case l5:
      {
        demoflag = 0;
        index = CalculateIndexFromKelvin(5000);
        CalculateLEDValues();
        break;
      }
      default: 
      {
        Serial.println("unrecognized command\n");
      }
    }   
  }
    demoaction();
    delay(1);
}



