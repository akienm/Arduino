
const int  C = int('C');  // Check ready

// define global working variables
int     serinp;       // input from serial port

void setup()
{  
  Serial.begin(9600);           // set up Serial library at 9600 bps
  Serial.flush();
}


// wait for command from host, then do:
/*  commands are single characters
 const int  C = int('C');  // Check ready
*/

void loop()  {
  int rval;
  serinp = 0;
  while (Serial.available() > 0) {
    serinp = Serial.read();   
    switch (serinp) {
      case C:
        {
          rval = 0;
          Serial.print("ready check, result = ");
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



