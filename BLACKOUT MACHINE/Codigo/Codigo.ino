
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
#include <LiquidCrystal_I2C.h>

#define ledPin 13
#define avoidPin D0
#define LCDPin1 D1
#define LCDPin2 D2
//DEFINIR
#define RedIngPumpPin D4
#define BlueIngPumpPin D5

#define YellowIngPumpPin D3

const int stepsPerRevolution = 2048;  // change this to fit the number of steps per revolution
//const int stepsPerRevolution = 5000;  // change this to fit the number of steps per revolution
// for your motor


// initialize the stepper library on pins 8 through 11:
Stepper myStepper(stepsPerRevolution, D5, D6, D7, D8);
LiquidCrystal_I2C lcd(0x27,16,2);

int stepCount = 0;  // number of steps the motor has taken
int motorSpeed = 10;

//int LED = 13; // Use the onboard Uno LED
//int isObstaclePin = 2;  // This is our input pin
//int isObstacle = HIGH;  // HIGH MEANS NO OBSTACLE

int const numberOfIngredients = 3;
int numberOfPositions = numberOfIngredients + 1; //Resting/Ready position
int positions[4]; 
int const RED_ING = 1;
int const BLUE_ING = 2;
int const YELLOW_ING = 3;
int currentStep = 0;

void setup() {
  
  myStepper.setSpeed(motorSpeed);
  loadPositions();
  Wire.begin(LCDPin2, LCDPin1);
  Serial.begin(9600);
}

void ledSetup(){
  lcd.init();
  lcd.clear();
  lcd.begin(16, 2);
  lcd.backlight();
  lcd.print("HELLO WORLD");
}

void loop() {
  Serial.print(positions[0]);
  Serial.print(positions[1]);
  Serial.print(positions[2]);
  goToIngredient(YELLOW_ING);
  //shortExtraction();
  //delay(600000);
  /*
  int avoidVal = digitalRead(avoidPin);
  if(avoidVal == LOW)
  {
    Serial.println("DETECTA");
    delay(3000);
    //digitalWrite(ledPin, HIGH);

  }
  else
  {
    Serial.println("NO DETECTA");
    //digitalWrite(ledPin, LOW);
    myStepper.step(stepsPerRevolution / 300);
  }*/
}

void shortExtraction(){
  analogWrite(RedIngPumpPin, 400);
  delay(5000);
  analogWrite(RedIngPumpPin, 0);
}

void executeDrink(){
  goToIngredient(YELLOW_ING);
  delay(1000);
  goToIngredient(YELLOW_ING);
  delay(1000);
  goToIngredient(RED_ING);
  delay(1000);
}
  
  void loadPositions(){
    int stepCount = 0;
    int stepSize = stepsPerRevolution / numberOfPositions;
    
    for (int i = 0; i < numberOfPositions; i++){
      positions[i] = stepCount;
      stepCount += stepSize;
    }
  }

void goToIngredient(int ingredient){
  int desiredStep = positions[ingredient];  
  int stepsToDo = desiredStep - currentStep;
  moveSteps(stepsToDo);
}

void moveSteps(int steps){
  myStepper.step(steps);
  currentStep = currentStep + steps;
  Serial.print(currentStep);
}

void shortExtraction(int ingredient, int seconds){
  int pumpPin = getPumpPinForIngredient(ingredient);
  analogWrite(pumpPin, 400);
  delay(seconds * 1000);
  analogWrite(pumpPin, 0);
}

int getPumpPinForIngredient(int ingredient){
    switch (ingredient) {
      case RED_ING:
        return RedIngPumpPin;
        break;
      case BLUE_ING:
        return BlueIngPumpPin;
        break;
      case YELLOW_ING:
        return YellowIngPumpPin;
        break;
      default:
        break;
    }
}








