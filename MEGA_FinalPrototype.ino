#include <LiquidCrystal.h>         
#include <HX711_ADC.h>
#include <EEPROM.h>
#include <Adafruit_GFX.h>
#include <MCUFRIEND_kbv.h>
#include "SPI.h"  // using sdcard for display bitmap image
#include "SD.h"

const int HX711_dout = 53; //UNO 6
const int HX711_sck = 52; // UNO 7
HX711_ADC LoadCell(HX711_dout, HX711_sck);
#define  BLACK   0x0000
#define BLUE    0x001F
#define RED     0xF800
#define GREEN   0x07E0
#define CYAN    0x07FF
#define MAGENTA 0xF81F
#define YELLOW  0xFFE0
#define WHITE   0xFFFF

MCUFRIEND_kbv tft;

const int calVal_eepromAdress = 0;
long t;
long stabilizingtime;
/*
LiquidCrystal lcd(13, 12, 11, 10, 9, 8);   
float voltage = 0;                          //the voltage measured from the TMP36
float degreesC = 0;                        
float degreesF = 0;  */
//LiquidCrystal lcd(23, 25, 27, 29, 31, 33);   
float calibrationValue;
const int button2Pin = 43;  // UNO 2
const int button1 = 45; //UNO 1
int change = -5, lastchange = 0;

int buttonCnt = -1; //488.
int buttonState, buttonState1,prevbuttonState1 = 1, prevbuttonState =0;
int red_light_pin= 37; // UNO 5 
int green_light_pin = 39; // UNO 4
int blue_light_pin = 41; // UNO 3 41
int red_light_pin_2= 25; // UNO 5 
int green_light_pin_2 = 27; // UNO 4
int blue_light_pin_2 = 29; // UNO 3 41
float tempweight;
float previ;
boolean setprevi = false;

void setup()
{
  // Set up the pushbutton pins to be an input:
  Serial.begin(9600);// speed at which communication occurs 9600 between board and computer. 
  pinMode(button2Pin, INPUT);
  pinMode(button1, INPUT);

  // Set up the LED pin to be an output:
  pinMode(red_light_pin, OUTPUT);
  pinMode(green_light_pin, OUTPUT);
  pinMode(blue_light_pin, OUTPUT);
//  lcd.begin(16, 2);                       
 // lcd.clear(); 
  
  LoadCell.begin(); // calibration value (see example file "Calibration.ino")
  calibrationValue = -480.40; // uncomment this if you want to set the calibration value in the sketch //696.0
//#if defined(ESP8266)|| defined(ESP32)
//  EEPROM.begin(512); // uncomment this if you use ESP8266/ESP32 and want to fetch the calibration value from eeprom
//#endif
  EEPROM.get(calVal_eepromAdress, calibrationValue); // uncomment this if you want to fetch the calibration value from eeprom
  stabilizingtime = 2000; // preciscion right after power-up can be improved by adding a few seconds of stabilizing time was 2000
  boolean _tare = true; //set this to false if you don't want tare to be performed in the next step
  LoadCell.start(stabilizingtime, _tare);
  if (LoadCell.getTareTimeoutFlag())
  {
    Serial.println("Timeout, check MCU>HX711 wiring and pin designations");
    while (1);
  }
  else
  {
    LoadCell.setCalFactor(calibrationValue); // set calibration value (float)
    Serial.println("Startup is complete");
  }
  //tft.setFont();
  uint16_t ID=tft.readID();
  tft.begin(ID);
  tft.setCursor(0,0);
  tft.fillScreen(BLACK);
}


void loop()
{// variables to hold the pushbutton states
  
  // Since a pushbutton has only two states (pushed or not pushed),
  // we've run them into digital inputs. To read an input, we'll
  // use the digitalRead() function. This function takes one
  // parameter, the pin number, and returns either HIGH (5V)
  // or LOW (GND).

  // Here we'll read the current pushbutton states into
  // two variables:
   //displays the weight in 4 decimal places only for calibration
  float i;
  static boolean newDataReady = 0;
  const int serialPrintInterval = 500; // was 1000
  if(LoadCell.update()){
    newDataReady = true;
  }
  if(newDataReady){
    if (millis() > t + serialPrintInterval) {
          tempweight = LoadCell.getData();
          i = (tempweight)/28.35;
          Serial.print(tempweight);
          Serial.print(" ");
          newDataReady = 0;
          t = millis();
        }
  }


  
  buttonState = digitalRead(button2Pin);
  if(buttonState != prevbuttonState){
    if(buttonState == HIGH){
      buttonCnt++;  // if button is pressed increase count
    }
    if(buttonCnt == 4){
      buttonCnt = 0; // we only have 3 states so we want to bring it back down to 0 after it hits 3 (remember index starts at 0)
    }

   
  }
  prevbuttonState = buttonState; 
  
  buttonState1 = digitalRead(button1);
  Serial.print(buttonState1);
  Serial.print(" ");
  if(buttonState1 == 0 && buttonState1 != prevbuttonState1){
    Serial.print("Taring... ");
    LoadCell.tareNoDelay();
  }
  prevbuttonState1 = buttonState1;//We want to save button state so that we only iterate when button state differs. 

  // Remember that if the button is being pressed, it will be
  // connected to GND. If the button is not being pressed,
  // the pullup resistor will connect it to 5 Volts.

  // So the state will be LOW when it is being pressed,
  // and HIGH when it is not being pressed.

  // Now we'll use those states to control the LED.
  // Here's what we want to do:

  // "If either button is being pressed, light up the LED"
  // "But, if BOTH buttons are being pressed, DON'T light up the LED"

  // Let's translate that into computer code. The Arduino gives you
  // special logic functions to deal with true/false logic:

  // A == B means "EQUIVALENT". This is true if both sides are the same.
  // A && B means "AND". This is true if both sides are true.
  // A || B means "OR". This is true if either side is true.
  // !A means "NOT". This makes anything after it the opposite (true or false).

  // We can use these operators to translate the above sentences
  // into logic statements (Remember that LOW means the button is
  // being pressed)

  // "If either button is being pressed, light up the LED"
  // becomes:
  // if ((button1State == LOW) || (button2State == LOW)) // light the LED

  // "If BOTH buttons are being pressed, DON'T light up the LED"
  // becomes:
  // if ((button1State == LOW) && (button2State == LOW)) // don't light the LED

  // Now let's use the above functions to combine them into one statement:

  float low, high;
  /*
  tft.setCursor(0,0);
  tft.setTextColor(BLACK);
  if(!setprevi){
    previ = i;
    setprevi = true;
  }
  tft.print(previ);
  tft.setTextColor(WHITE);
  tft.print(i);
  previ = i;
  */
  
  if(buttonCnt == 1) // Chicken                                                 // then...
  {
    //Light 3 off
    red_light_pin= 44; // UNO 5 
    green_light_pin = 42; // UNO 4
    blue_light_pin = 40;
    RGB_color(0, 0, 0);
    //Light 1 on
    red_light_pin= 37; // UNO 5 
    green_light_pin = 39; // UNO 4
    blue_light_pin = 41;
    RGB_color(255, 0, 0); // Red
    low = 8.0;
    high = 8.3;
  }
  if(buttonCnt == 0) //Bits
  {
    //Light 3 off
    red_light_pin= 50; // UNO 5 
    green_light_pin = 48; // UNO 4
    blue_light_pin = 46; 
    RGB_color(0, 0, 0); 
    red_light_pin= 44; // UNO 5 
    green_light_pin = 42; // UNO 4
    blue_light_pin = 40;
    RGB_color(0, 0, 255); // 
    low = 8.0;
    high = 8.1;

  }
   if(buttonCnt == 2) //Sweet Potatoes
  {
    //Light 1 off
    red_light_pin= 37; // UNO 5 
    green_light_pin = 39; // UNO 4
    blue_light_pin = 41;
    RGB_color(0, 0, 0);
    //Light 2 on
    red_light_pin= 25; // UNO 5 
    green_light_pin = 27; // UNO 4
    blue_light_pin = 29; 
    RGB_color(0, 255, 0); // Green
    low = 8.0;
    high = 8.3;
  

  }
if(buttonCnt == 3) // PB                                                  // then...
  {
    //Light 2 off
    red_light_pin= 25; // UNO 5 
    green_light_pin = 27; // UNO 4
    blue_light_pin = 29; 
    RGB_color(0, 0, 0);
    //Light 3 on
    red_light_pin= 50; // UNO 5 
    green_light_pin = 48; // UNO 4
    blue_light_pin = 46; 
    RGB_color(255,255,0); // Yellow
    low = 8.4;
    high = 8.7;

  }

   
   if(low <= i && i <= high && change != 0){ //solid range
          /*
          lcd.setCursor(0, 0);                      //set the cursor to the lower left position
          lcd.print("Weight: ");
          lcd.print(i);
          lcd.print(" oz ");
          lcd.setCursor(0, 1);
          lcd.print(":)");
          */
          //Make rest blank
          tft.fillRect(125,83,70,315,BLACK);
          tft.fillRect(125,83,70,315,BLACK);
          tft.fillRect(30,210,270,70,BLACK);
          
          //Draw Smiley
          tft.drawRoundRect(170, 115, 100, 100, 50, YELLOW);
          tft.drawRoundRect(170, 265, 100, 100, 50, YELLOW);

          //tft.drawRoundRect(0, 0, 140, 140, 70, GREEN);
          tft.fillRoundRect(80, 155, 70, 170, 70, YELLOW);
          tft.fillRoundRect(86, 155, 70, 170, 70, BLACK);
          change = 0;
   }
   if(low > i && change != 1){ // add more
    /*
          lcd.setCursor(0, 0);                      //set the cursor to the lower left position
          lcd.print("Weight: ");
          lcd.print(i);
          lcd.print(" oz ");
          lcd.setCursor(0, 1);
          lcd.print("+ ");*/
          //Make rest blank
          tft.fillRect(125,83,70,315,BLACK);
          tft.drawRoundRect(170, 115, 100, 100, 50, BLACK);
          tft.drawRoundRect(170, 265, 100, 100, 50, BLACK);
          tft.fillRoundRect(80, 155, 70, 170, 70, BLACK);
          tft.fillRoundRect(86, 155, 70, 170, 70, BLACK);
          //Draw Plus
          tft.fillRect(125,83,70,315,YELLOW);
          tft.fillRect(30,210,270,70,YELLOW);
          change = 1;
   }
    if(high < i && change != 2){ // take out
      /*
          lcd.setCursor(0, 0);                      //set the cursor to the lower left position
          lcd.print("Weight: ");
          lcd.print(i);
          lcd.print(" oz");
          lcd.setCursor(0, 1);
          lcd.print("- ");*/
          //Make rest blank
          tft.fillRect(125,83,70,315,BLACK);
          tft.fillRect(30,210,270,70,BLACK);
          tft.drawRoundRect(170, 115, 100, 100, 50, BLACK);
          tft.drawRoundRect(170, 265, 100, 100, 50, BLACK);
          tft.fillRoundRect(80, 155, 70, 170, 70, BLACK);
          tft.fillRoundRect(86, 155, 70, 170, 70, BLACK);

          //Draw Minus
          tft.fillRect(125,83,70,315,YELLOW);
          change = 2;
   }
  // As you can see, logic operators can be combined to make
  // complex decisions!

  // Don't forget that we use = when we're assigning a value,
  // and use == when we're testing a value for equivalence.
}
void RGB_color(int red_light_value, int green_light_value, int blue_light_value)
 {
  analogWrite(red_light_pin, red_light_value);
  analogWrite(green_light_pin, green_light_value);
  analogWrite(blue_light_pin, blue_light_value);
}
