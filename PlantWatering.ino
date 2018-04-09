#include <DHT.h>
#include<Servo.h>

char val;         // Stores serial port data value

const int buzzer = 9;

int rot = 0; //Servo rotation
int humidityThreshold = 700;

const unsigned long max = 4251767295;
unsigned long milNow = 0; //Current time
unsigned long pastMilW = 0; //Stores last time plant was watered
unsigned long pastMilI = 0; //Stores last time humidity information was sent
unsigned long intervalRollover = 0; //Stores interval between max value for unsigned int and last time the plant was watered, compensating for rollover
unsigned long interval = 43200000; //Interval between watering periods

bool manual = false;
bool rolledover = false;
bool humidityModeOn = false;

Servo s;

void setup()
{
  s.attach(3);
  s.write(0);
  Serial.begin(9600);       // Begins serial communication
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
      delay(100);
      noTone(buzzer);
    }
    pastMilW = milNow;
}
 
void loop() 
  {
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
  if(humidityModeOn && !manual && analogRead(A0) < humidityThreshold && analogRead(A0) > 100)
  {
    water();
  }
  if(milNow - pastMilW >= interval && !manual) // Waters every 720 minutes (12 hours)
  {
    water();
  }
  if(Serial.available())       // Checks for Bluetooth conectivity
  {
    if(Serial.read() == "humidity") 
    {
      if(humidityModeOn)
      {
        manual = false;
        humidityModeOn = true;
        Serial.println("Switched to automatic humidity-based watering mode!");
      }
      else
      {
        humidityModeOn = false;
        Serial.println("Switched to regular automatic!");
      }
    }
    if(Serial.read() == "manual")
    {
      if(manual)
      {
       manual = false;
       Serial.println("Switched to automatic!");
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
    if(Serial.readString() == "config")
    {
      Serial.println("What do you want to change? (|H|umidity/|T|ime/|C|ancel)");
      switch(Serial.read())
      {
      case 'H':
      {
        Serial.println("Input new humidity minimum for watering (between 100 and 1023)");
        if(Serial.readString().toInt() > 100 && !isnan(Serial.readString().toInt()) && Serial.readString().toInt() < 1024)
        {
          humidityThreshold = Serial.readString().toInt();
          Serial.println("Value changed successfully!");
        }
        else
        {
          Serial.println("Please input a valid number!");
        }
      }
      case 'T':
      {
        Serial.println("Input new interval in milliseconds:");
        if(Serial.readString().toInt() > 0 && !isnan(Serial.readString().toInt()) && Serial.readString().toInt() < max)
        {
          interval = atol(Serial.readString().toInt());
        }
        else
        {
          Serial.println("Please input a valid number!"); 
        }
      }
      case 'C':
      {
        Serial.println("Config cancelled");
      }
      }
    }
    if(!isnan(analogRead(A0))) // Checks if value is compatible/error occured
    {
      if(milNow - pastMilI >= 60000)
      {
        Serial.print("Soil Humidity: ");
        Serial.print(analogRead(A0));
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
