// Random.pde
// -*- mode: C++ -*-
//
// Make a single stepper perform random changes in speed, position and acceleration
//
// Copyright (C) 2009 Mike McCauley
// $Id: Random.pde,v 1.1 2011/01/05 01:51:01 mikem Exp mikem $

#include <AccelStepper.h>


const int stepsPerRevolution = 2048; 
int const numberOfIngredients = 3;
int numberOfPositions = numberOfIngredients + 1; //Resting/Ready position
int positions[4]; 
int const RED_ING = 1;
int const BLUE_ING = 2;
int const YELLOW_ING = 3;
int currentStep = 0;



// Define a stepper and the pins it will use
AccelStepper stepper(AccelStepper::FULL4WIRE, D5, D6, D7, D8);
void setup()
{  
  Serial.begin(9600);
  loadPositions();
  stepper.setCurrentPosition(0);
  
      stepper.setSpeed(320);
  Serial.print(stepper.currentPosition());
}

void loop()
{
  
    if (stepper.distanceToGo() == 0)
    {
    goToIngredient(YELLOW_ING);
     }
      stepper.runSpeed();
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
  moveToStep(desiredStep);
}


void moveToStep(int step){
  
      // Random change to speed, position and acceleration
      // Make sure we dont get 0 speed or accelerations
      delay(1000);
      stepper.moveTo(step);
        Serial.print(stepper.currentPosition());
        Serial.print("");
      stepper.setSpeed(2048);
}


