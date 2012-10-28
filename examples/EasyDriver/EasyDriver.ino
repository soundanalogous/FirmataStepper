/*
 FirmataStepper is a simple non-blocking stepper motor library
 for 2 and 4 wire bipolar and unipolar stepper motor drive circuits
 as well as EasyDriver (http://schmalzhaus.com/EasyDriver/) and 
 other step + direction drive circuits.

 created 28 October 2012
 by Jeff Hoefs 

 wiring diagram and tutorial for DRIVER (EasyDriver) stepper interface:
 http://bildr.org/2011/06/easydriver/
*/

#include "FirmataStepper.h"

#define DIR_PIN 2       // for EasyDriver
#define STEP_PIN 3      // for EasyDriver
#define BUTTON 12
#define MICRO_STEPS_PER_REV 200 * 8 // easy driver has 8 microsteps per step

bool toggle = true;
bool buttonState;
bool lastButtonState = false;
bool lastPinState = false;
long lastDebounceTime = 0;
long debounceDelay = 20;

// EasyDriver (or other step + direction) interface
FirmataStepper stepper(FirmataStepper::DRIVER, MICRO_STEPS_PER_REV, DIR_PIN, STEP_PIN);   
 

void onPressed() {
  if (toggle) {
    // use acceleration and deceleration (10.00 rad/sec^2)
    // params: num steps, speed (0.01*rad/sec), accel (0.01*rad/sec^2), decel (0.01*rad/sec^2)
    // 2000 = 20 rad/sec, 2050 = 20.50 rad/sec
    stepper.setStepsToMove(16000, 2000, 1000, 1000);
  } else {
    // no acceleration or deceleration
    stepper.setStepsToMove(-16000, 2000);
  }

  toggle = !toggle;
}

// check if the button is pressed (and debounce button)
// assuming button wired with pull down resistor (10k resistor to ground)
bool isButtonPressed() {
  bool pinVal = digitalRead(BUTTON);
  bool isPressed = false;

  if (pinVal != lastPinState) {
    lastDebounceTime = millis();
  }

  if ((millis() - lastDebounceTime) > debounceDelay) {
    buttonState = pinVal;
    if (buttonState && buttonState != lastButtonState) isPressed = true;
    lastButtonState = buttonState;
  }

  lastPinState = pinVal;

  return isPressed;
}

void setup() {
  
  Serial.begin(57600);
  
  pinMode(DIR_PIN, OUTPUT);
  pinMode(STEP_PIN, OUTPUT);
  pinMode(BUTTON, INPUT);

}

void loop() {

  if (isButtonPressed()) {
    onPressed();
  }
    
  // update the step position
  bool done = stepper.update();

  // check if the step sequence is complete
  if (done == true) {
    Serial.print("stepper done");
  }

}
