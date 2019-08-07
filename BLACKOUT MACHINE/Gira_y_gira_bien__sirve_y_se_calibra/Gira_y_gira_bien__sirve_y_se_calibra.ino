// ConstantSpeed.pde
// -*- mode: C++ -*-
//
// Shows how to run AccelStepper in the simplest,
// fixed speed mode with no accelerations
/// \author  Mike McCauley (mikem@airspayce.com)
// Copyright (C) 2009 Mike McCauley
// $Id: ConstantSpeed.pde,v 1.1 2011/01/05 01:51:01 mikem Exp mikem $

#include <AccelStepper.h>


#define avoidPin D0
// Defaults to AccelStepper::FULL4WIRE (4 pins) on 2, 3, 4, 5
AccelStepper stepper(AccelStepper::FULL4WIRE, D5, D6, D7, D8);


void setup()
{  
   stepper.setMaxSpeed(1000);
   stepper.setSpeed(512);
   stepper.setAcceleration(128); 
   Serial.begin(9600);
}

  //bajar vel y cacomodar con snesore 4 veces menos
bool serving = true;
bool reubicado = false;

void loop()
{  
  if(serving){
    reubicado = false;
    receta1();
  } else if (!reubicado) {
    
    Serial.println("REUBICANDO");
    buscarPosicionInicial();
    
    int avoidVal = digitalRead(avoidPin);
    if(avoidVal == LOW){
      
      Serial.println("Detectado");
      
      stepper.setCurrentPosition(0);
      reubicado = true;
    }
  }
   
}

void receta1(){
  if (stepper.currentPosition()==0) {
   stepper.moveTo(1024);
  }

  if (stepper.currentPosition()==1024) {
   //stepper.moveTo(0);
   
   //Parar
   serving = false;
  }
  
   stepper.run();
}

void buscarPosicionInicial(){
  stepper.setSpeed(512);  
  stepper.runSpeed();
}



