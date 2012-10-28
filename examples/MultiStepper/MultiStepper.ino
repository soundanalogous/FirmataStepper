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

#define MOTOR1_PIN_1 2
#define MOTOR1_PIN_2 3
#define MOTOR1_PIN_3 4
#define MOTOR1_PIN_4 5
#define MOTOR2_PIN_1 6
#define MOTOR2_PIN_2 7
#define MOTOR2_PIN_3 8
#define MOTOR2_PIN_4 9 
#define BUTTON 12
#define STEPS_PER_REV 200 // number of steps to rotate 360 degrees
#define NUM_STEPPERS 2

bool toggle = true;
bool buttonState;
bool lastButtonState = false;
bool lastPinState = false;
long lastDebounceTime = 0;
long debounceDelay = 20;

FirmataStepper *stepper[NUM_STEPPERS];


void onPressed() {
  if (toggle) {
    // use acceleration and deceleration (10.00 rad/sec^2)
    // params: num steps, speed (0.01*rad/sec), accel (0.01*rad/sec^2), decel (0.01*rad/sec^2)
    // 1900 = 19 rad/sec, 2050 = 20.50 rad/sec
    stepper[0]->setStepsToMove(2000, 1900, 1000, 1000);
    // no acceleration or deceleration
    stepper[1]->setStepsToMove(2000, 1900);
  } else {
    // no acceleration or deceleration
    stepper[0]->setStepsToMove(-2000, 1900);
    // use acceleration and deceleration (20.00 rad/sec^2)
    stepper[1]->setStepsToMove(-2000, 1900, 2000, 2000);
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
  
  pinMode(MOTOR1_PIN_1, OUTPUT);
  pinMode(MOTOR1_PIN_2, OUTPUT);
  pinMode(MOTOR1_PIN_3, OUTPUT);
  pinMode(MOTOR1_PIN_4, OUTPUT);

  pinMode(MOTOR2_PIN_1, OUTPUT);
  pinMode(MOTOR2_PIN_2, OUTPUT);
  pinMode(MOTOR2_PIN_3, OUTPUT);
  pinMode(MOTOR2_PIN_4, OUTPUT);

  pinMode(BUTTON, INPUT);

  // two wire interface
  stepper[0] = new FirmataStepper(FirmataStepper::TWO_WIRE, STEPS_PER_REV, MOTOR1_PIN_1, MOTOR1_PIN_2);
  stepper[1] = new FirmataStepper(FirmataStepper::TWO_WIRE, STEPS_PER_REV, MOTOR2_PIN_1, MOTOR2_PIN_2);  

  // four wire interface
  //stepper[0] = new FirmataStepper(FirmataStepper::FOUR_WIRE, STEPS_PER_REV, MOTOR1_PIN_1, MOTOR1_PIN_2, MOTOR1_PIN_3, MOTOR1_PIN_4);
  //stepper[1] = new FirmataStepper(FirmataStepper::FOUR_WIRE, STEPS_PER_REV, MOTOR2_PIN_1, MOTOR2_PIN_2, MOTOR2_PIN_3, MOTOR2_PIN_4); 
  
}

void loop() {

  if (isButtonPressed()) {
    onPressed();
  }
    
  // if one or more stepper motors are used, update their position
  if (NUM_STEPPERS > 0) {
    for (int i=0; i<NUM_STEPPERS; i++) {
      // update the step position
      bool done = stepper[i]->update();

      // check if the step sequence is complete
      if (done == true) {
        Serial.print("stepper ");
        Serial.print(i);
        Serial.println(" done");
      }
    }
  }  
}
