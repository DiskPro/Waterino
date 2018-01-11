#include <DHT.h>
#include<Servo.h>

#define TYPE DHT11
#define HSPIN 4

DHT HS(HSPIN, TYPE);

char val;         // Stores serial port data value

const int buzzer = 9;

int rot = 0; //Servo rotation
long milNow = 0; //Current time
long pastMilW = 0; //Stores last time plant was watered
long pastMilI = 0; //Stores last time humidity information was sent

bool manual = false;

Servo s;

void setup()
{
  s.attach(3);
  s.write(0);
  Serial.begin(9600);       // Begins serial communication

  HS.begin();
}

void water()
{
  s.write(60);
    delay(2000); // Time it'll spend watering the plant
    s.write(0);
    if(Serial.available())
    {
    Serial.println("Plant watered!");
    }
    else
    {
    tone(buzzer, 1500);
    delay(1000);
    noTone(buzzer);
    }
    pastMilW = milNow;
}
 
void loop() {
  milNow = millis();
  
  if(milNow - pastMilW >= 43200000 && !manual) // Waters every 720 minutes (12 hours)
  {
    water();
  }
  if( Serial.available() )       // Checks for Bluetooth conectivity
  {
    if(Serial.read() == "manual")
    {
      if(manual)
      {
       manual = false;
       Serial.println("Switched to automatic mode!");
      }
      else
      {
       manual = true;
       Serial.println("Switched to manual mode!");
      }
      
    }
    if(Serial.read() == "water" && manual)
    {
      water();
    }
    if(!isnan(HS.readHumidity())) // Checks if value is compatible/error occured
    {
      if(milNow - pastMilI >= 60000)
      {
    Serial.print("Humidity: ");
    Serial.print(HS.readHumidity());
    Serial.println("-----------------------------------------------------");
      pastMilI = milNow;
      }
    }
    else
    {
    Serial.println("Error reading sensor!");
    }
  }
  }
