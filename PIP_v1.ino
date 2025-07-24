    // Libraries
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>


    // Used ports
#define S_TEMP A1

#define LED_C3 D10
#define LED_C2 D9
#define LED_C1 D8

#define LED_N  D7

#define LED_H1 D5
#define LED_H2 D4
#define LED_H3 D3


    // Debug option
//#define DEBUG_MODE

    // Display init.
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
#define LOGO_HEIGHT   16
#define LOGO_WIDTH    16
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);


    // Global var.
float temp    = 0;
//float voltage;
int tick      = 0;    // samples taken
//int interval  = 1000; // the delay value
int count     = 0;
float avg     = 0;



void setup()
{

  pinMode(LED_C1, OUTPUT);
  pinMode(LED_C2, OUTPUT);
  pinMode(LED_C3, OUTPUT);
  pinMode(LED_N,  OUTPUT);
  pinMode(LED_H1, OUTPUT);
  pinMode(LED_H2, OUTPUT);
  pinMode(LED_H3, OUTPUT);




  analogReadResolution(14);

  Serial.begin(9600);

    // Display setup.
      // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS))
  {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }

    // Display checks
  delay(1000);
  display.display();
  delay(1000);
  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);

  }



void readTemp(int value) // Calculate temp as a samples average (to compensate for null/inaccurate values)
{
  #ifdef DEBUG_MODE
    Serial.print("Average: ");
    Serial.println(avg);
  #endif

  value = value * 5 * 100.0 / 16384.0;

  temp += value;
  tick +=1;c
  delay(10);

  if(avg == 0)
  {
    avg = value;
    count = 30;
  }

  avg = avg * count;
  count = count + 1;
  avg = (avg + value) / count;

}


void displayTemp()
{

  temp = temp / tick;

  #ifdef DEBUG_MODE
  //Serial.println(voltage);
  Serial.println(avg);
  Serial.println();
  #endif

      // Temperature
  display.setCursor(0, 0);
  display.setTextSize(1);
  display.print("Temp.: ");

  //display.setCursor(30,20);
  display.setTextSize(2);
  display.print(temp);
  display.setTextSize(1);
  display.write(9);
  display.setTextSize(2);
  display.print("C");


      // Heart rate
  display.setCursor(0,30);
  display.setTextSize(1);
  display.print("BPM:");
  display.print(" mort.");

        // Heart rate
  display.setCursor(0,40);
  display.setTextSize(1);
  display.print("Blood oxygen lvl.:");
  display.setCursor(20,50);
  display.print("tot mort%");

  display.display();

  led_switch();

  Serial.println(temp);
}


void led_switch()
{

  digitalWrite(LED_C3, LOW);
  digitalWrite(LED_C2, LOW);
  digitalWrite(LED_C1, LOW);
  digitalWrite(LED_N, LOW);
  digitalWrite(LED_H1, LOW);
  digitalWrite(LED_H2, LOW);
  digitalWrite(LED_H3, LOW);

  if(temp-avg <= -0.5)
  {
    digitalWrite(LED_C1, HIGH);
    if(temp-avg <= -1.0)
    {
      digitalWrite(LED_C2, HIGH);
      if(temp-avg <= -1.5)
      {
        digitalWrite(LED_C3, HIGH);
      }
    }
  }
  else if(temp-avg >= 0.5)
    {
      digitalWrite(LED_H1, HIGH);
      if(temp-avg >= 1.0)
      {
        digitalWrite(LED_H2, HIGH);
        if(temp-avg >= 1.5)
        {
          digitalWrite(LED_H3, HIGH);
        }
      }
    }
  else
  {
    digitalWrite(LED_N, HIGH);
  }
}


void loop() 
{

      // Take current temp reading
  readTemp(analogRead(S_TEMP));


      // Show temp when enough samples have been taken
  if(tick >= 100)
  {
    display.clearDisplay();
    displayTemp();
    tick = 0;
    temp = 0;
  }
  
}


