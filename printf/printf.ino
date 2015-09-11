char* ftos(float floatVal)
{
  String stringVal = "";     
  
  stringVal+=String(int(floatVal))+ "."+String(getDecimal(floatVal)); //combining both whole and decimal part in string with a full                                                                      //stop between them
  char * cstr = new char [stringVal.length()+1];
  strcpy (cstr, stringVal.c_str());
  
  return cstr;  
}
//function to extract decimal part of float
long getDecimal(float val)
{
  int intPart = int(val);
  long decPart = 1000*(val-intPart); //I am multiplying by 1000 assuming that the foat values will have a maximum of 3 decimal places
                                     //Change to match the number of decimal places you need
   if(decPart>0)return(decPart);           //return the decimal part of float number if it is available 
   else if(decPart<0)return((-1)*decPart); //if negative, multiply by -1
   else if(decPart=0)return(00);           //return 0 if decimal part of float number is not available
}

void pt(const char* format, ...)
{
    char dest[80];
    va_list argptr;
    va_start(argptr, format);
    vsprintf(dest, format, argptr);
    va_end(argptr);
    Serial.print(dest);
}
void setup() 
{
  Serial.begin(9600);
  Serial.println("fuck off");
  Serial.println("fuck off");
  Serial.println("fuck off");
  pt("the %s value is %s\n\n", "current", ftos(2.1));
  pt("OK!\n");
}
void loop() 
{
}
