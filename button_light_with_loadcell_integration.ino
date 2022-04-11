#include <LiquidCrystal.h>         
#include <HX711_ADC.h>
#include <EEPROM.h>
const int HX711_dout = 6; 
const int HX711_sck = 7; 
HX711_ADC LoadCell(HX711_dout, HX711_sck);
const int calVal_eepromAdress = 0;
long t;

LiquidCrystal lcd(13, 12, 11, 10, 9, 8);   
float voltage = 0;                          //the voltage measured from the TMP36
float degreesC = 0;                        
float degreesF = 0;  
const int button2Pin = 2;  // pushbutton 2 pin

int buttonCnt = 0; //488.
int buttonState, prevbuttonState =0;
int red_light_pin= 5;
int green_light_pin = 4;
int blue_light_pin = 3;

void setup()
{
  // Set up the pushbutton pins to be an input:
  Serial.begin(9600);// speed at which communication occurs 9600 between board and computer. 
  pinMode(button2Pin, INPUT);

  // Set up the LED pin to be an output:
  pinMode(red_light_pin, OUTPUT);
  pinMode(green_light_pin, OUTPUT);
  pinMode(blue_light_pin, OUTPUT);
  lcd.begin(16, 2);                       
  lcd.clear(); 
  
  LoadCell.begin();
  float calibrationValue; // calibration value (see example file "Calibration.ino")
  calibrationValue = 696.0; // uncomment this if you want to set the calibration value in the sketch
#if defined(ESP8266)|| defined(ESP32)
 // EEPROM.begin(512); // uncomment this if you use ESP8266/ESP32 and want to fetch the calibration value from eeprom
#endif
  EEPROM.get(calVal_eepromAdress, calibrationValue); // uncomment this if you want to fetch the calibration value from eeprom
  long stabilizingtime = 2000; // preciscion right after power-up can be improved by adding a few seconds of stabilizing time
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
 /*
 lcd.setCursor(14,0);
 lcd.print("grams");
 if(Serial.available())
 {
   char temp = Serial.read();
   if(temp == 't' || temp == 'T')
     scale.tare();  //Reset the scale to zero      
 }
 */
 static boolean newDataReady = 0;
  const int serialPrintInterval = 1000; 
  if(LoadCell.update()){
    newDataReady = true;
  }
  if(newDataReady){
    if (millis() > t + serialPrintInterval) {
          float i = LoadCell.getData();
          i = i/28.35;
          newDataReady = 0;
          t = millis();
          lcd.setCursor(0, 0);                      //set the cursor to the lower left position
          lcd.print("Weight: ");
          lcd.print(i);
          lcd.print(" oz");
          newDataReady = 0;
          t = millis();
        }
  }

//old code
/*
     if(millis() > t +PrintInterval)
      float i = LoadCell.getData();
    Serial.println(i, " ");
    lcd.clear();
    delay(300);
    lcd.setCursor(0, 0);                      //set the cursor to the lower left position
    lcd.print("Weight: ");
    lcd.print(i);
    */

    //comment
  voltage = analogRead(A0) * 0.004882813;   
  degreesC = (voltage - 0.5) * 100.0;      
  degreesF = degreesC * (9.0 / 5.0) + 32.0; 
 // delay(200);
  
  buttonState = digitalRead(button2Pin);
  if(buttonState != prevbuttonState){
    if(buttonState == HIGH){
      buttonCnt++;  // if button is pressed increase count
    }
    if(buttonCnt == 4){
      buttonCnt = 0; // we only have 3 states so we want to bring it back down to 0 after it hits 3 (remember index starts at 0)
    }
    delay(50); // this code executes fast, we want it to slow down a little bit so we make it wait 50 mili here
    Serial.print(buttonCnt);

   
  }
  prevbuttonState = buttonState; //We want to save button state so that we only iterate when button state differs. 

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

  if(buttonCnt == 1) // Chicken                                                 // then...
  {
    RGB_color(255, 0, 0); // Red
    low = 8.0;
    high = 8.3;
    /*
    lcd.clear();
    lcd.setCursor(0, 0);                      //set the cursor to the top left position
    lcd.print("Degrees C: ");                 //print a label for the data
    lcd.print(degreesC);  
    lcd.setCursor(0, 1);                      //set the cursor to the lower left position
    lcd.print("Red");
    */


  }
  if(buttonCnt == 0) //Bits
  {

  RGB_color(0, 0, 255); // Blue
  low = 8.0;
  high = 8.1;
  /*
  lcd.clear();
  lcd.setCursor(0, 0);                      //set the cursor to the lower left position
  lcd.print("Sensor off");
  lcd.setCursor(0, 1);                      //set the cursor to the lower left position
  lcd.print("Green");
  
  */

  }
   if(buttonCnt == 2) //Sweet Potatoes
  {
  
  RGB_color(0, 255, 0); // Green
  low = 8.0;
  high = 8.3;
  /*
  lcd.clear();
  lcd.setCursor(0, 0);                      //set the cursor to the lower left position
  lcd.print("Degrees C: ");                 //print a label for the data
  lcd.print(degreesC);
  lcd.setCursor(0, 1);                      //set the cursor to the lower left position
  lcd.print("Blue");
  */

  }
if(buttonCnt == 3) // PB                                                  // then...
  {
    RGB_color(255, 255, 0); // Yellow
    low = 8.4;
    high = 8.7;
    /*
    lcd.clear();
    lcd.setCursor(0, 0);                      //set the cursor to the lower left position
    lcd.print("Degrees F: ");                 //Print a label for the data
    lcd.print(degreesF); 
    lcd.setCursor(0, 1);                      //set the cursor to the lower left position
    lcd.print("Yellow");
    */

  }

   /*
   if(low <= weight && weight <= high){ //solid range
    
   }
   if(low > weight){ // add more
    
   }
    if(high < weight){ // take out
    
   }*/
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
