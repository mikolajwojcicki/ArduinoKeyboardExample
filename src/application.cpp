/*
  Keyboard test

  For the Arduino Leonardo, Micro or Due

  Reads a byte from the serial port, sends a keystroke back.
  The sent keystroke is one higher than what's received, e.g. if you send a,
  you get b, send A you get B, and so forth.

  The circuit:
  - none

  created 21 Oct 2011
  modified 27 Mar 2012
  by Tom Igoe

  This example code is in the public domain.

  https://www.arduino.cc/en/Tutorial/BuiltInExamples/KeyboardSerial
*/

#include "Arduino.h"

#define JOYSTICK

#if !defined(JOYSTICK)
#include "Keyboard.h"
#define KEYBOARD
#else
#include "Joystick.h"
#endif
//defining which pins are goint to be used for matrix LowPins for colums, which state is checked, and HighPins for rows which become HIGH
#define LowPin1 1
#define LowPin2 0
#define LowPin3 2
#define HighPin1 19
#define HighPin2 20
#define HighPin3 21
const byte row = 3;
const byte col = 3;
const byte rowP[row] = {HighPin1,HighPin2,HighPin3};
const byte colP[col] = {LowPin1,LowPin2,LowPin3};
#if !defined(JOYSTICK)
const byte keys[row][col] = {
  {'1','2','3'},
  {'4','5','6'},
  {'7','8','9'}
};
bool pressed[row][col];
#else
const uint8_t keys[row][col] = {
  {0,1,2},
  {3,4,5},
  {6,7,8}
};
int pressed_test=0b000000000; //an array same as in KEYBOARD but represented as a binary number, index 0 being on the far right
#endif

#if defined(KEYBOARD)
void setup() {
  // initialize control over the keyboard:
  //Serial.begin(9600);
  Serial.end();
  for(byte i=0;i<row;i++) { //clearing the table holding button's states. Not only clears it but also defines it in a way
    for(byte j=0;j<col;j++) {
      pressed[i][j]=0;
    }
  }
  for(byte i=0; i<row; i++) { //setting rows as INPUTS to avoid any short circuits
    pinMode(rowP[i],INPUT);
  };
  for(byte i=0; i<col; i++) { //setting colums as INPUTS for later usage
    pinMode(colP[i],INPUT);
  };
  Keyboard.begin(); //establishing HID connection to the host computer
}

void loop() {
    //Serial.println("--------------------------------------");
    for(byte i=0;i<row;i++) {
      pinMode(rowP[i],OUTPUT); //changing state of row pins to output in order to later check them
      digitalWrite(rowP[i], HIGH);
      for(byte j=0;j<col;j++) { //sweeping through every colum pin and checking if it's state changed to HIGH
        char out = keys[i][j];
        //Serial.print(pressed[i][j]);
        if(digitalRead(colP[j])==HIGH) { //if the state is HIGH that means that this button in [i][j] position is pressed
          Keyboard.press(out);
          pressed[i][j]=1;
          delay(10);
        }
        else if(pressed[i][j]==1) { //if the checked button is not pressed and it's state was previously set to 1 meaning it was pressed the button is released and it's state is reset
          Keyboard.release(out);
          pressed[i][j]=0;
        }
      };
      delay(1);
      digitalWrite(rowP[i],LOW); //bringing the row pin to LOW at the end of the loop and changing it to input again to avoid shorts
      pinMode(rowP[i],INPUT);
      //Serial.println("");
    };
    //Serial.println("--------------------------------------");
}
#else
int pressed_test_keystrokes=0b00; //binary representation of an array holding information about currently pressed keystrokes
const unsigned int standardButtons = 9U; // number of physical buttons in the matrix 
const unsigned int keystrokes = 2U; // number of programmed keystrokes
unsigned int buttons = keystrokes+standardButtons;
bool can_press_keys=true;
Joystick_ controller(JOYSTICK_DEFAULT_REPORT_ID, JOYSTICK_TYPE_GAMEPAD,buttons,0U,false,false,false,false,false,false,false,false,false,false,false);

int setbit(int bit, byte position) { //setbit function performs bitwise OR opreation and stores it's result in the defined position by shifting 1UL which is just a one byte to the left position amount and compraing it to the bit's bit int that position
  return bit |= 1UL<<position;
}

int clearbit(int bit, byte position) { //clearbit function performs bitwise AND operation and stores it's result in the defined position by first shifting the only bit in 1UL position times to the left, then inverts the 1UL and compraes the two values
  return bit &= ~(1UL<<position);
}

bool testbit(int bit, byte position) {
	return bit & 1UL<<position;
}

void releaseKeystrokes() {
  for(byte i=standardButtons;i<buttons;i++) {
    controller.releaseButton(i);
  }
}

void releaseKeys() {
  for(byte i=0;i<standardButtons;i++) {
    controller.releaseButton(i);
  }
}

void keystroke(int binary_array) { //this function has predefined patterns for keystrokes which are used in switch
  switch (binary_array)
  {
  case 0b000010001:
    releaseKeys();
    controller.pressButton(9);
    pressed_test_keystrokes=setbit(pressed_test_keystrokes,0);
    break;
  case 0b000010010:
    releaseKeys();
    controller.pressButton(10);
    pressed_test_keystrokes=setbit(pressed_test_keystrokes,0);
    break;
  default:
    break;
  }
}

void setup() {
  // initialize control over the keyboard:
  //Serial.begin(9600);
  Serial.end();
  for(byte i=0; i<row; i++) {
    pinMode(rowP[i],INPUT);
  };
  for(byte i=0; i<col; i++) {
    pinMode(colP[i],INPUT);
  };
  controller.begin();
}

void loop() {
    //Serial.println("--------------------------------------");
    for(byte i=0;i<row;i++) {
      pinMode(rowP[i],OUTPUT);
      digitalWrite(rowP[i], HIGH);
      for(byte j=0;j<col;j++) {
        uint8_t out = keys[i][j];
        //Serial.print(pressed[i][j]);
        if(digitalRead(colP[j])==HIGH) {
          pressed_test=setbit(pressed_test,out);
          if(out!=4&&can_press_keys==true) { //checks if pressed key is not 4th and can_press_keys is true
            controller.pressButton(out);
          }
          else { //if not it means that 4th key is pressed. the keys are locked and keystroke function is run
            can_press_keys=false;
            keystroke(pressed_test);
          }
          delay(10);
        }
        else if(testbit(pressed_test,out)==1) {
          controller.releaseButton(out);
          pressed_test=clearbit(pressed_test,out);
        }
        else if(pressed_test==0UL&&pressed_test_keystrokes!=0UL) { //checks if there are keystorokes with pressed status and no keys are pressed. if they are not, keystrokes are released and states reset to 0
            releaseKeystrokes();
            pressed_test_keystrokes=0b00;
        }
        else if(pressed_test==0UL&&pressed_test_keystrokes==0UL&&can_press_keys==false) { //if there are not keystrokes and no keys pressed and can_press_keys is still false it is being set to true
          can_press_keys=true;
        }
      };
      delay(1);
      digitalWrite(rowP[i],LOW);
      pinMode(rowP[i],INPUT);
      //Serial.println("");
    };
    //Serial.println("--------------------------------------");
}
#endif