#include <DHT.h>
#include<Servo.h>

#define TYPE DHT11
#define HSPIN 4

DHT HS(HSPIN, TYPE);

char val;         // Stores serial port data value

int rot = 0;
int timeW = 0;

Servo s;

void setup()
{
  s.attach(3);
  s.write(0);
  Serial.begin(9600);       // Begins serial communication

  HS.begin();
}
 
void loop() {
  if( timeW = 720 ) // Waters every 720 minutes (12 hours)
  {
    s.write(60);
    delay(2000); // Time it'll spend watering the plant
    s.write(0);
    timeW = 0;
  }
  if( Serial.available() )       // Checks for Bluetooth conectivity
  {
    if(!isnan(HS.readHumidity())) // Checks if value is compatible/error occured
    {
    Serial.print("Humidity: ");
    Serial.print(HS.readHumidity());
    Serial.println("-----------------------------------------------------");
    delay(60000); // Stops for a minute in order to not send a stupid amount of unnescessary information to the phone, and also to allow for better time management (So I can stick to 16 bit integers)
    }
    else
    {
    Serial.println("Error reading sensor!");
    }
    timeW++;
  }
  }
  


