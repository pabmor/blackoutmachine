
/*
 Stepper Motor Control - speed control

 This program drives a unipolar or bipolar stepper motor.
 The motor is attached to digital pins 8 - 11 of the Arduino.
 A potentiometer is connected to analog input 0.

 The motor will rotate in a clockwise direction. The higher the potentiometer value,
 the faster the motor speed. Because setSpeed() sets the delay between steps,
 you may notice the motor is less responsive to changes in the sensor value at
 low speeds.

 Created 30 Nov. 2009
 Modified 28 Oct 2010
 by Tom Igoe

 */

#include <Stepper.h>
#include <Wire.h>
//#include <LiquidCrystal_I2C.h>

#define ledPin 13
#define avoidPin 2
const int stepsPerRevolution = 2048;  // change this to fit the number of steps per revolution
//const int stepsPerRevolution = 5000;  // change this to fit the number of steps per revolution
// for your motor


// initialize the stepper library on pins 8 through 11:
Stepper myStepper(stepsPerRevolution, D5, D6, D7, D8);
//LiquidCrystal_I2C lcd(0x27,16,2); //

int stepCount = 0;  // number of steps the motor has taken
int motorSpeed = 10;

int LED = 13; // Use the onboard Uno LED
int isObstaclePin = 2;  // This is our input pin
int isObstacle = HIGH;  // HIGH MEANS NO OBSTACLE

int const numberOfIngredients = 4;
int positions[numberOfIngredients]; 
int ING_ROJO = 0;
int ING_AZUL = 1;
int ING_VERDE = 2;
int ING_AMARILLO = 3;
int currentStep = 0;

void setup() {
  myStepper.setSpeed(motorSpeed);
 /* lcd.init();
  lcd.clear();
  lcd.begin(16, 2);
  lcd.backlight();*/
  loadPositions();
}

void loop() {
  myStepper.step(stepsPerRevolution / 200);
 /* lcd.print(" BLACKOUT-MACHINE ");
  lcd.print(" ------ ");
  int avoidVal = digitalRead(avoidPin);
  if(avoidVal == LOW)
  {
    digitalWrite(ledPin, HIGH);
    delay(2000);
  }
  else
  {
    digitalWrite(ledPin, LOW);
    myStepper.step(stepsPerRevolution / 200);
  }*/
}

void executeDrink(){
  goToIngredient(ING_AMARILLO);
  delay(1000);
  goToIngredient(ING_VERDE);
   delay(1000);
  goToIngredient(ING_ROJO);
   delay(1000);
  goToIngredient(ING_AMARILLO);
}

void loadPositions(){
  int stepCount = 0;
  int stepSize = stepsPerRevolution / numberOfIngredients;
  
  for (int i = 0; i < numberOfIngredients; i++){
    positions[i] = stepCount;
    stepCount += stepSize;
  }
}

/*
//TODO USE
int[] getAvailablePositions(int numberOfIngredients){
  int stepCount = 0;
  int stepSize = stepsPerRevolution / numberOfIngredients;
  int positions[numberOfIngredients];
  
  for (int i = 0; i < numberOfIngredients; i++){
    positions[0] = stepCount;
    stepCount += stepSize;
  }
  return positions;
}*/

//TODO USE
void setIngredients(){
  ING_ROJO = 0;
  ING_AZUL = 1;
  ING_VERDE = 2;
  ING_AMARILLO = 3;
}

void goToIngredient(int ingredient){
  int desiredStep = positions[ingredient];  
  int stepsToDo = desiredStep - currentStep;
  moveSteps(stepsToDo);
}

void moveSteps(int steps){
  myStepper.step(steps);
  currentStep = currentStep + steps;
}



