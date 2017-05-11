#include "Tlc5940.h"

void setup()
{
  Tlc.init(4095); // initialise TLC5940 and set all channels 
}



void loop()
{
  for(int i=0; i<9;i++)
  {
     Tlc.set(i, 200);    
     Tlc.update();
     delay(500); 
     Tlc.set(i, 4095);    
     Tlc.update();
     delay(500); 
  }
}
