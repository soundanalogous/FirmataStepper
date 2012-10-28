/*
 FirmataStepper is a simple non-blocking stepper motor library
 for 2 and 4 wire bipolar and unipolar stepper motor drive circuits
 as well as EasyDriver (http://schmalzhaus.com/EasyDriver/) and 
 other step + direction drive circuits.

 created 28 October 2012
 by Jeff Hoefs 

 circuit for TWO_WIRE and FOUR_WIRE bipolar stepper interfaces: 
 http://arduino.cc/en/Reference/StepperBipolarCircuit

 circuit for TWO_WIRE and FOUR_WIRE unipolar stepper interfaces: 
 http://arduino.cc/en/Reference/StepperUnipolarCircuit
*/

#include "FirmataStepper.h"

#define MOTOR_PIN_1 2
#define MOTOR_PIN_2 3
#define MOTOR_PIN_3 4
#define MOTOR_PIN_4 5
#define BUTTON 12
#define STEPS_PER_REV 200 // number of steps to rotate 360 degrees

bool toggle = true;
bool buttonState;
bool lastButtonState = false;
bool lastPinState = false;
long lastDebounceTime = 0;
long debounceDelay = 20;

// two wire interface
FirmataStepper stepper(FirmataStepper::TWO_WIRE, STEPS_PER_REV, MOTOR_PIN_1, MOTOR_PIN_2);

// four wire interface
//FirmataStepper stepper(FirmataStepper::FOUR_WIRE, STEPS_PER_REV, MOTOR_PIN_1, MOTOR_PIN_2, MOTOR_PIN_3, MOTOR_PIN_4);  


void onPressed() {
  if (toggle) {
    // use acceleration and deceleration (10.00 rad/sec^2)
    // params: num steps, speed (0.01*rad/sec), accel (0.01*rad/sec^2), decel (0.01*rad/sec^2)
    // 1900= 19 rad/sec, 2050 = 20.50 rad/sec
    stepper.setStepsToMove(2000, 1900, 1000, 1000);
  } else {
    // no acceleration or deceleration
    stepper.setStepsToMove(-2000, 1900);
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
  
  pinMode(MOTOR_PIN_1, OUTPUT);
  pinMode(MOTOR_PIN_2, OUTPUT);
  pinMode(MOTOR_PIN_3, OUTPUT);
  pinMode(MOTOR_PIN_4, OUTPUT);  
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
