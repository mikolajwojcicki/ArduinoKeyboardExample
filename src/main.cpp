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

//#define JOYSTICK

#if !defined(JOYSTICK)
#include "Keyboard.h"
#define KEYBOARD
#else
#include "Joystick.h"
#endif

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
#else
const uint8_t keys[row][col] = {
  {0,1},
  {2,3}
};
#endif
bool pressed[row][col];

#if defined(KEYBOARD)
void setup() {
  // initialize control over the keyboard:
  Serial.begin(9600);
  Keyboard.begin();
  for(byte i=0;i<row;i++) {
    for(byte j=0;j<col;j++) {
      pressed[i][j]=0;
    }
  }
  for(byte i=0; i<row; i++) {
    pinMode(rowP[i],INPUT);
  };
  for(byte i=0; i<col; i++) {
    pinMode(colP[i],INPUT);
  };
}

void loop() {
    for(byte i=0;i<row;i++) {
      pinMode(rowP[i],OUTPUT);
      digitalWrite(rowP[i], HIGH);
      for(byte j=0;j<col;j++) {
        char out = keys[i][j];
        if(digitalRead(colP[j])==HIGH) {
          Serial.println(out);
          Keyboard.press(out);
          pressed[i][j]=1;
          delay(75);
        }
        else if(pressed[i][j]==1) {
          Keyboard.release(out);
          pressed[i][j]=0;
        }
      }
      delay(1);
      digitalWrite(rowP[i],LOW);
      pinMode(rowP[i],INPUT);
    };
}
#else
Joystick_ controller(JOYSTICK_DEFAULT_REPORT_ID, JOYSTICK_TYPE_GAMEPAD,4U,0U,false,false,false,false,false,false,false,false,false,false,false);
void setup() {
  Serial.begin(9600);
  pinMode(SafetyPin, INPUT);
  for(byte i=0;i<row;i++) {
    for(byte j=0;j<col;j++) {
      pressed[i][j]=0;
    }
  }
  for(byte i=0; i<row; i++) {
    pinMode(rowP[i],INPUT);
  };
  for(byte i=0; i<col; i++) {
    pinMode(colP[i],INPUT);
  };
  controller.begin();
}

void loop() {
  while(digitalRead(SafetyPin)==LOW) {
    for(byte i=0;i<row;i++) {
      pinMode(rowP[i],OUTPUT);
      digitalWrite(rowP[i], HIGH);
      for(byte j=0;j<col;j++) {
        char out = keys[i][j];
        if(digitalRead(colP[j])==HIGH) {
          Serial.println(out);
          controller.pressButton(out);
          pressed[i][j]=1;
          delay(150);
        }
        else if(pressed[i][j]==1) {
          controller.releaseButton(out);
          pressed[i][j]=0;
        }
      }
      delay(1);
      digitalWrite(rowP[i],LOW);
      pinMode(rowP[i],INPUT);
    };
  }
}
#endif