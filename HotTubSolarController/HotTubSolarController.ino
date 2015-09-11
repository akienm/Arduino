// convert C  x  9/5 + 32 = F
#include <OneWire.h>
#include <DallasTemperature.h>
#include <EEPROM.h>

#define AKIENDEBUG false

void(* softReset) (void) = 0;//declare reset function at address 0

// set points - these become eeprom data eventually
#define COLLECTOR_TEMP_OVERAGE 1
#define FAN_ON_TEMP 80.0
#define TUB_TARGET_TEMP 104
#define TEMP_CHECK_INTERVAL 10 // seconds
#define FAN_TIMER 120 // seconds
#define PUMP_OFF_TIMER 600 // seconds 
#define PUMP_ON_TIMER 120 // seconds 

const int a = int('a');  const int A = int('A'); const int b = int('b'); const int B = int('B'); const int c = int('c'); const int C = int('C'); const int d = int('d'); const int D = int('D'); const int e = int('e'); const int E = int('E'); const int f = int('f'); const int F = int('F'); const int g = int('g');  const int G = int('G');  const int h = int('h');  const int H = int('H');  const int i = int('i');  const int I = int('I');  const int j = int('j');  const int J = int('J');  const int k = int('k');  const int K = int('K');  const int l = int('l');  const int L = int('L');  const int m = int('m');  const int M = int('M');  const int n = int('n');  const int N = int('N');  const int o = int('o');  const int O = int('O');  const int p = int('p'); const int P = int('P'); const int q = int('q');  const int Q = int('Q');  const int r = int('r');  const int R = int('R');  const int s = int('s');  const int S = int('S');  const int t = int('t');  const int T = int('T');  const int u = int('u');  const int U = int('U');  const int v = int('v');  const int V = int('V');  const int w = int('w');  const int W = int('W');  const int x = int('x');  const int X = int('X');  const int y = int('y');  const int Y = int('Y');  const int z = int('z');  const int Z = int('Z'); 

// Defines pins by function
#define pin_fan           2
#define pin_pump          3
#define pin_temp_internal 4
#define pin_temp_tub      5
#define pin_temp_output   6
#define pin_temp_coll1    7
#define pin_temp_coll2    8

// Setup a oneWire instance to communicate with any OneWire devices (not just Maxim/Dallas temperature ICs)
OneWire oneWire_internal(pin_temp_internal);
OneWire oneWire_tub(pin_temp_tub);
OneWire oneWire_output(pin_temp_output);
OneWire oneWire_coll1(pin_temp_coll1);
OneWire oneWire_coll2(pin_temp_coll2);

// Pass our oneWire reference to Dallas Temperature. 
DallasTemperature temp_internal(&oneWire_internal);
DallasTemperature temp_tub(&oneWire_tub);
DallasTemperature temp_output(&oneWire_output);
DallasTemperature temp_coll1(&oneWire_coll1);
DallasTemperature temp_coll2(&oneWire_coll2);

//==========================================================================
// Flash/EEPROM storage setup
// The Arduino has Flash (32K Program Memory), RAM (2K volatile), and
// EEPROM (1K bytes). We use this to make strings storable in program 
// memory only instead of both in program memory and RAM.  This makes 
// the F() macro work properly.
// Workaround for http://gcc.gnu.org/bugzilla/show_bug.cgi?id=34734
#ifdef PROGMEM
#undef PROGMEM
#define PROGMEM __attribute__((section(".progmem.data")))
#endif
#define __ASSERT_USE_STDERR
#include <assert.h>
// Uninitialized EEPROM
#define UNINITIALIZED_EEPROM 10
// CRC Error
#define CRC_ERROR 11

long timer = 0;  // max 2147483647 seconds or 68 years

class Timers 
{
  private:
    int timerStatus;
    long currentTimeout;
    long tDEFAULT_VALUE;

  public: 
  Timers(long newDefaultValue)     
  {
    tDEFAULT_VALUE = newDefaultValue;
    Reset();
  }
  void Start()     
  {
    currentTimeout = timer + tDEFAULT_VALUE;
  }

  bool    IsRunning() { return currentTimeout > timer; }
  bool    IsDone()    { return currentTimeout <= timer; }
  void    Reset()     { currentTimeout = 0; }

  long    Remaining() 
  { 
    long result = currentTimeout - timer;
    if(result < 0)
    {
      result = 0;
    }
    return result;
  }
};

int serinp;  // serial input
float tempread_internal;
float tempread_tub;
float tempread_output;
float tempread_coll1;
float tempread_coll2;
bool fanStatus = false;
bool fanOnRequest = false;
bool fanOffRequest = false;
bool pumpStatus = false;
bool pumpOnRequest = false;
bool PumpOffRequest = false;

Timers tempCheckTimer = Timers(TEMP_CHECK_INTERVAL);
Timers fanTimer       = Timers(FAN_TIMER);
Timers PumpOffTimer   = Timers(PUMP_OFF_TIMER);
Timers pumpOnTimer    = Timers(PUMP_ON_TIMER);
//void print(String incoming) {
//  Serial.print(incoming);
//}
//
//void println(String incoming) {
//  Serial.println(incoming);
//}

void TimerTick()
{
  delay(1000);
  timer++;
  if(AKIENDEBUG) Serial.print(F("TimerTick:"));
  if(AKIENDEBUG) Serial.println(timer);

}

void MessageFanOn()
{
  Serial.println(F("COMMAND: Fan ON"));
}

void RequestFanOn()
{
  fanOnRequest = true;
}

void ForceFanOn() 
{
  fanOffRequest = false;
  fanOnRequest = false;
  fanStatus = true;
  digitalWrite(pin_fan, fanStatus);
  //fanTimer.Start();
}

void MessageFanOff()
{
  Serial.println(F("COMMAND: Fan OFF"));
}

void RequestFanOff() 
{
  fanOffRequest = true;
  if(!fanTimer.IsRunning())
    fanTimer.Start();
}

void ForceFanOff()
{
  fanOnRequest = false;
  fanOffRequest = false;
  fanStatus = false;
  digitalWrite(pin_fan, fanStatus);
  fanTimer.Reset();
}

void UpdateFan() 
{
  if(tempread_internal > 0)
  {
    if(fanStatus == true)
    {
      if((fanOffRequest == false) && (tempread_internal < FAN_ON_TEMP))
        MessageFanOff();
        RequestFanOff();

      if(fanOffRequest && fanTimer.IsDone())
        ForceFanOff();
    }
    else   // if(fanStatus == false)
    {
      if(fanTimer.IsDone())
      {
        if(fanOnRequest)
          ForceFanOn();
  
        if(tempread_internal > FAN_ON_TEMP)
          ForceFanOn();
      }
    }
  }
}

void MessagePumpOn()
{
  Serial.println(F("COMMAND: Pump ON"));
}

void RequestPumpOn()
{
  pumpOnRequest = true;
}

void ForcePumpOn() 
{
  pumpStatus = true;
  pumpOnRequest = false;
  PumpOffRequest = false;
  digitalWrite(pin_pump, pumpStatus);
  pumpOnTimer.Start();
  PumpOffTimer.Reset();
}

void MessagePumpOff()
{
  Serial.println(F("COMMAND: Pump Off"));
}

void RequestPumpOff()
{
  PumpOffRequest = true;
}

void ForcePumpOff()
{
  pumpStatus = false;
  pumpOnRequest = false;
  PumpOffRequest = false;
  digitalWrite(pin_pump, pumpStatus);
  PumpOffTimer.Start();
  pumpOnTimer.Reset();
}

float ComputeCollectorValue()
{
  return (tempread_coll1 + tempread_coll2) / 2;
}

void UpdatePump() 
{
  
  if(tempread_output > 0 && tempread_tub > 0 && tempread_coll1 > 0 && tempread_coll2 > 0)
  {
    if(pumpStatus) // conditions we might turn it off
    {
      if((tempread_output < tempread_tub) && pumpOnTimer.IsDone())
        ForcePumpOff();
      if(tempread_tub >= TUB_TARGET_TEMP)
        ForcePumpOff();
      if(tempread_tub > ComputeCollectorValue())
        ForcePumpOff();
    }
    else // conditions we might want to turn it on
    {
      if((tempread_tub < TUB_TARGET_TEMP) 
        && (ComputeCollectorValue() > tempread_tub)
        && (ComputeCollectorValue() + COLLECTOR_TEMP_OVERAGE > tempread_tub) 
        && PumpOffTimer.IsDone())
          ForcePumpOn();
    }
  }
}

void RequestUpdateTemps()
{
  if(AKIENDEBUG) Serial.print(F("RequestUpdateTemps: tempCheckTimer="));
  if(AKIENDEBUG) Serial.println(tempCheckTimer.Remaining());

  if(tempCheckTimer.IsDone()) 
  {
    if(AKIENDEBUG) Serial.println(F("TimerTick:UpdateTemps"));

    UpdateTemps();
  }
}

void ForceUpdateTemps()
{
  if(AKIENDEBUG) Serial.println(F("ForceUpdateTemps"));
  UpdateTemps();
}

void UpdateTemps() 
{
  if(AKIENDEBUG) Serial.println(F("UpdateTemps"));

  Serial.println("");
  Serial.println(F("<"));
  Serial.println(F("----------------------------------------"));

  // call sensor_pin_d2.requestTemperatures() to issue a global temperature 
  // request to all devices on the bus
  Serial.print(F("Requesting temperatures... "));
  FetchATemp(temp_internal, &tempread_internal);
  FetchATemp(temp_tub, &tempread_tub);
  FetchATemp(temp_output, &tempread_output);
  FetchATemp(temp_coll1, &tempread_coll1);
  FetchATemp(temp_coll2, &tempread_coll2);
  
  Serial.println(F("DONE"));
  UpdateFan();
  UpdatePump();
  ReportStatus();
  Serial.println(F(">"));
  tempCheckTimer.Start();
}

void FetchATemp(DallasTemperature tempObject, float *tempContainerVariable)
{
  if(AKIENDEBUG) Serial.println(F("FetchATemp"));
  int count;
  int numloops = 10;
  for (count=1; count<numloops; count++) 
  {
    tempObject.requestTemperatures(); // Send the command to get temperatures
    *tempContainerVariable = tempObject.getTempFByIndex(0);
    if(*tempContainerVariable > 0)
      break;
    TimerTick();
  }
}


void ReportStatus() 
{
  Serial.print(F(" Temperature inside controller (device 1) is:......"));
  Serial.print(tempread_internal);
  Serial.println(F("F"));
  
  Serial.print(F(" Temperature for tub (device 2) is:................"));
  Serial.print(tempread_tub);
  Serial.println(F("F"));
  
  Serial.print(F(" Temperature for collector output (device 3) is:..."));
  Serial.print(tempread_output);
  Serial.println(F("F"));
  
  Serial.print(F(" Temperature for collector sensor 1 (device 4) is:."));
  Serial.print(tempread_coll1);
  Serial.println(F("F"));
  
  Serial.print(F(" Temperature for collector sensor 2 (device 5) is:."));
  Serial.print(tempread_coll2);
  Serial.println(F("F"));
  
  Serial.print(F(" Average collector temp is........................."));
  Serial.print(ComputeCollectorValue());
  Serial.println(F("F"));

  Serial.println(F("Reporting"));
  Serial.print(F("fanStatus:      ")); Serial.println(fanStatus);
  Serial.print(F("FanTimer:       ")); Serial.println(fanTimer.Remaining());
  Serial.print(F("pumpStatus:     ")); Serial.println(pumpStatus);
  Serial.print(F("pumpOnTimer:    ")); Serial.println(pumpOnTimer.Remaining());
  Serial.print(F("PumpOffTimer:   ")); Serial.println(PumpOffTimer.Remaining());
}


void setup()
{
  // start serial port
  Serial.begin(9600);
  Serial.flush();
  Serial.println("");
  Serial.println("");
  Serial.println("");
  Serial.println(F("Akien's Hot Tub Controller"));
  Serial.println("");
  //  delay(1000);

  // Start up the library
  temp_internal.begin(); // IC Default 9 bit. If you have troubles consider upping it 12. Ups the delay giving the IC more time to process the temperature measurement
  temp_tub.begin(); 
  temp_output.begin(); 
  temp_coll1.begin(); 
  temp_coll1.begin(); 
  
  pinMode(pin_fan,OUTPUT);  // controls fan
  pinMode(pin_pump,OUTPUT); // controls pump
  
  ForceFanOff();
  digitalWrite(pin_pump, false);
  tempCheckTimer.Start();

  Serial.println(F("Initialization Complete"));
  Serial.println("");

}

void loop() 
{
  Serial.print(".");
  serinp = 0;
  while (Serial.available() > 0) 
  {
    serinp = Serial.read();
    switch (serinp) 
    {
      case c: 
      {
        Serial.println(F("c Ready!"));
        break; 
      }
      case F: // fan on
      {
        MessageFanOn();
        ForceFanOn();
        ForceUpdateTemps();
        break; 
      }  
      case f: // fan off
      {
        MessageFanOff();
        ForceFanOff();
        ForceUpdateTemps();
        break; 
      }  
      case P: // pump on
      {
        MessagePumpOn();
        ForcePumpOn();
        ForceUpdateTemps();
        break; 
      }  
      case p: 
      {
        MessagePumpOff();
        ForcePumpOff();
        ForceUpdateTemps();
        break; 
      }  
      case R:
      {
        softReset();
      }
      case T:
      {
        if(AKIENDEBUG) Serial.println(F("Command T: Force Update temps"));

        ForceUpdateTemps();
        break;
      }
      default: 
      {
        Serial.print(F("unrecognized command:"));
        Serial.print(serinp);
        Serial.print(" '");
        Serial.print(char(serinp));
        Serial.println("");
        Serial.println("");
      }
    }
    
  }
  
  TimerTick();
  RequestUpdateTemps();
}

