#include <DHT.h>
#include<Servo.h>

#define TYPE DHT11
#define HSPIN 4

DHT HS(HSPIN, TYPE);

char val;         // Stores serial port data value

const int buzzer = 9;

int rot = 0; //Servo rotation
const unsigned long max = 4251767295;
unsigned long milNow = 0; //Current time
unsigned long pastMilW = 0; //Stores last time plant was watered
unsigned long pastMilI = 0; //Stores last time humidity information was sent
unsigned long intervalRollover = 0; //Stores interval between max value for unsigned int and last time the plant was watered, compensating for rollover
unsigned long interval = 43200000; //Interval between watering periods

bool manual = false;
bool rolledover = false;

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
  
  if(pastMilW >= max && milNow < pastMilW) // Checks if millis() has rolled over and sets value for compensation
  {
    rolledover = true;
    intervalRollover = max - pastMilW;
  }
  if(rolledover)
  {
    milNow += intervalRollover; 
  }
  if(milNow - pastMilW >= interval && !manual) // Waters every 720 minutes (12 hours)
  {
    water();
  }
  if(Serial.available())       // Checks for Bluetooth conectivity
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
    if(Serial.read() == "time")
    {
      Serial.println("Input new interval in milliseconds:");
      if(atol(Serial.read()) > 0 && !isnan(atol(Serial.read())) && atol(Serial.read()) < max)
      {
        interval = atol(Serial.read());
      }
      else
      {
        Serial.println("Please input a valid number!"); 
      }
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
