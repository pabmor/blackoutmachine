#define ledPin 13
#define avoidPin 2

#include <Stepper.h>

const int stepsPerRevolution = 2048;
Stepper myStepper(stepsPerRevolution, 8, 9, 10, 11);

void setup()
{
  pinMode(ledPin, OUTPUT);
  pinMode(avoidPin, INPUT);
}

bool stop = false;

void loop()
{
  int motorSpeed = 5;
  myStepper.setSpeed(motorSpeed);
  
  int avoidVal = digitalRead(avoidPin);
  if(avoidVal == LOW)
  {
    digitalWrite(ledPin, HIGH);
    stop = true;
  }
  else
  {
    digitalWrite(ledPin, LOW);
    
    myStepper.step(stepsPerRevolution / 200);
  }
}
