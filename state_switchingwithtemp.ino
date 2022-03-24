/*
SparkFun Inventor's Kit 
Example sketch 05

PUSH BUTTONS

  Use pushbuttons for digital input

  Previously we've used the analog pins for input, now we'll use
  the digital pins for input as well. Because digital pins only
  know about HIGH and LOW signals, they're perfect for interfacing
  to pushbuttons and switches that also only have "on" and "off"
  states.

  We'll connect one side of the pushbutton to GND, and the other
  side to a digital pin. When we press down on the pushbutton,
  the pin will be connected to GND, and therefore will be read
  as "LOW" by the Arduino.

  But wait - what happens when you're not pushing the button?
  In this state, the pin is disconnected from everything, which 
  we call "floating". What will the pin read as then, HIGH or LOW?
  It's hard to say, because there's no solid connection to either
  5 Volts or GND. The pin could read as either one.

  To deal with this issue, we'll connect a small (10K, or 10,000 Ohm)
  resistance between the pin and 5 Volts. This "pullup" resistor
  will ensure that when you're NOT pushing the button, the pin will
  still have a weak connection to 5 Volts, and therefore read as
  HIGH.

  (Advanced: when you get used to pullup resistors and know when
  they're required, you can activate internal pullup resistors
  on the ATmega processor in the Arduino. See
  http://arduino.cc/en/Tutorial/DigitalPins for information.)

Hardware connections:

  Pushbuttons:

    Pushbuttons have two contacts that are connected if you're
    pushing the button, and disconnected if you're not.

    The pushbuttons we're using have four pins, but two pairs
    of these are connected together. The easiest way to hook up
    the pushbutton is to connect two wires to any opposite corners.

    Connect any pin on pushbutton 1 to ground (GND).
    Connect the opposite diagonal pin of the pushbutton to
    digital pin 2.

    Connect any pin on pushbutton 2 to ground (GND).
    Connect the opposite diagonal pin of the pushbutton to
    digital pin 3.

    Also connect 10K resistors (brown/black/red) between
    digital pins 2 and 3 and 5 Volts. These are called "pullup"
    resistors. They ensure that the input pin will be either
    5V (unpushed) or GND (pushed), and not somewhere in between.
    (Remember that unlike analog inputs, digital inputs are only
    HIGH or LOW.)

  LED:

    Most Arduinos, including the Uno, already have an LED
    and resistor connected to pin 13, so you don't need any
    additional circuitry.

    But if you'd like to connect a second LED to pin 13,

    Connect the positive side of your LED to Arduino digital pin 13
    Connect the negative side of your LED to a 330 Ohm resistor
    Connect the other side of the resistor to ground

This sketch was written by SparkFun Electronics,
with lots of help from the Arduino community.
This code is completely free for any use.
Visit http://learn.sparkfun.com/products/2 for SIK information.
Visit http://www.arduino.cc to learn about the Arduino.

Version 2.0 6/2012 MDG
*/


// First we'll set up constants for the pin numbers.
// This will make it easier to follow the code below.
#include <LiquidCrystal.h> 
const int button2Pin = 3;  // pushbutton 2 pin
const int ledPin1 =  4; // LED pin
const int ledPin2 =  5;
int buttonCnt = 0;
int buttonState, prevbuttonState =0;

LiquidCrystal lcd(13, 12, 11, 10, 9, 8);    // tell the RedBoard what pins are connected to the display

float voltage = 0;                          //the voltage measured from the TMP36
float degreesC = 0;                         //the temperature in Celsius, calculated from the voltage
float degreesF = 0; 

void setup()
{
  // Set up the pushbutton pins to be an input:
  Serial.begin(9600);// speed at which communication occurs 9600 between board and computer. 
  pinMode(button2Pin, INPUT);

  // Set up the LED pin to be an output:
  pinMode(ledPin1, OUTPUT);  
  pinMode(ledPin2, OUTPUT);

  lcd.begin(16, 2); 
  lcd.clear();  
    
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
  
  buttonState = digitalRead(button2Pin);
  if(buttonState != prevbuttonState){
    if(buttonState == HIGH){
      buttonCnt++;  // if button is pressed increase count
    }
    if(buttonCnt == 3){
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

  voltage = analogRead(A0) * 0.004882813;   //convert the analog reading, which varies from 0 to 1023, back to a voltage value from 0-5 volts
  degreesC = (voltage - 0.5) * 100.0;       //convert the voltage to a temperature in degrees Celsius
  degreesF = degreesC * (9.0 / 5.0) + 32.0; //convert the voltage to a temperature in degrees Fahrenheit

  lcd.clear();    

  if(buttonCnt == 1) // pushing button 1 AND button 2                                                     // then...
  {
    digitalWrite(ledPin1, HIGH); //turn the LED on
    digitalWrite(ledPin2, LOW);// turn the LED on
    lcd.setCursor(0, 0);                      //set the cursor to the top left position
    lcd.print("Degrees C: ");                 //print a label for the data
    lcd.print(degreesC);  
    lcd.setCursor(0, 1);                      //set the cursor to the lower left position
    lcd.print("Setting: 1");
  }
  if(buttonCnt == 0)
  {
    digitalWrite(ledPin1, LOW); //turn the LED on
    digitalWrite(ledPin2, LOW);// turn the LED off
    lcd.setCursor(0, 0);                      //set the cursor to the lower left position
    lcd.print("Sensor off");ca
    lcd.setCursor(0, 1);                      //set the cursor to the lower left position
    lcd.print("Setting: 0");
  }
   if(buttonCnt == 2)
  {
    digitalWrite(ledPin2, HIGH); //turn the LED on
    digitalWrite(ledPin1, LOW);// turn the LED off
    
    lcd.setCursor(0, 0);                      //set the cursor to the lower left position
    lcd.print("Degrees F: ");                 //Print a label for the data
    lcd.print(degreesF); 
    lcd.setCursor(0, 1);                      //set the cursor to the lower left position
    lcd.print("Setting: 2");
  }
                     //print the degrees Fahrenheit

  delay(300); 
  // As you can see, logic operators can be combined to make
  // complex decisions!

  // Don't forget that we use = when we're assigning a value,
  // and use == when we're testing a value for equivalence.
}
