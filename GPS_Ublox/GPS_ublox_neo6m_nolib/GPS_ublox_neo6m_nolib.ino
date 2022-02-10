#include <SoftwareSerial.h>

SoftwareSerial gps(4,3);

char dato=' ';

void setup()
{
 // Serial Comm for monitor
 Serial.begin(115200);     
// Comm with GPS       
 gps.begin(9600); 
}


void loop()
{
  if(gps.available())
  {
    dato=gps.read();
    Serial.print(dato);
  }
}
