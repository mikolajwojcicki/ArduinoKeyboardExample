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
#include "Keyboard.h"

#define LowPin1 1
#define LowPin2 0
#define HighPin 21
#define HighPin1 20
#define SafetyPin 9
const byte row = 2;
const byte col = 2;
const byte rowP[row] = {HighPin,HighPin1};
const byte colP[col] = {LowPin1,LowPin2};
const byte keys[row][col] = {
  {'A','B'},
  {'C','D'}
};
bool pressed[row][col];

void setup() {
  // initialize control over the keyboard:
  Serial.begin(9600);
  Keyboard.begin();
  pinMode(SafetyPin, INPUT);
  for(byte i=0;i<row;i++) {
    for(byte j=0;j<col;j++) {
      pressed[i][j]=0;
    }
  }
  for(byte i=0; i<row; i++) {
    pinMode(rowP[i],OUTPUT);
    digitalWrite(rowP[i], LOW);
  };
  for(byte i=0; i<col; i++) {
    pinMode(colP[i],INPUT);
  };
}

void loop() {
  if(digitalRead(SafetyPin)==LOW) {
    for(byte i=0;i<row;i++) {
      digitalWrite(rowP[i], HIGH);
      for(byte j=0;j<col;j++) {
        char out = keys[i][j];
        if(digitalRead(colP[j])==HIGH) {
          Serial.println(out);
          Keyboard.press(out);
          pressed[i][j]=1;
          delay(150);
        }
        else if(pressed[i][j]==1) {
          Keyboard.release(out);
          pressed[i][j]=0;
        }
      }
      delay(1);
      digitalWrite(rowP[i],LOW);
    };
  }
  else {
    Keyboard.releaseAll();
  }
}