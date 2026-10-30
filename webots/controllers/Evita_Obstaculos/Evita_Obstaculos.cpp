// File:          Evita_Obstaculos.cpp
// Date:
// Description:   Evitación de Obstáculos
// Author:        Alejandro Rubio
// Modifications:

#include <stdio.h>

// Librerías requeridas para los sensores y actuadores utilizados
#include <webots/Robot.hpp>
#include <webots/Motor.hpp>
#include <webots/DistanceSensor.hpp>

#define TIME_STEP 64

// Distancias
#define DIST_LEJANO 260
#define DIST_MEDIO  145 
#define DIST_CERCANO  90
#define DIST_LADO 145

// Velocidades 
#define MAX_SPEED -10
#define VEL_MEDIA  -5
#define VEL_ALTA -9
#define VEL_BAJA  -3

// All the webots classes are defined in the "webots" namespace
using namespace webots;
using namespace std;

// ----- PROTOTIPOS ------
void getSensorValues();
void setSpeed(float, float);


// Se instancia el robot
Robot *robot = new Robot();

// Motores
Motor *front_left_motor = robot->getMotor("MOTOR_FRONT_LEFT");
Motor *front_right_motor = robot->getMotor("MOTOR_FRONT_RIGHT");
Motor *back_left_motor = robot->getMotor("MOTOR_BACK_LEFT");
Motor *back_right_motor = robot->getMotor("MOTOR_BACK_RIGHT");

// Distance sensors
DistanceSensor *ds_left = robot->getDistanceSensor("left_ds");
DistanceSensor *ds_front = robot->getDistanceSensor("front_ds");
DistanceSensor *ds_right = robot->getDistanceSensor("right_ds");

// Valor de sensores
double ds_valor_izq, ds_valor_front, ds_valor_der;

// Variables de deteccion
bool izq_detect = false;
bool front_detect = false;
bool der_detect = false;

// Velocidad auxiliar en cada motor
float izq_speed = 0, der_speed = 0; 

int main(int argc, char **argv) {
  // Motors initialization
  front_left_motor->setPosition(INFINITY);  front_left_motor->setVelocity(0.0);
  front_right_motor->setPosition(INFINITY); front_right_motor->setVelocity(0.0);
  back_left_motor->setPosition(INFINITY);   back_left_motor->setVelocity(0.0);
  back_right_motor->setPosition(INFINITY);  back_right_motor->setVelocity(0.0);
  
  // Sesnros inizialization
  ds_left->enable(TIME_STEP);
  ds_front->enable(TIME_STEP);
  ds_right->enable(TIME_STEP);

  // Bucle principal:
  while (robot->step(TIME_STEP) != -1) {
      
    // Se obtiene valor de sensores
    getSensorValues();
    
    // Comienzan a false
    front_detect = false;
    izq_detect = false;
    der_detect = false;

    // Comprobación de sensores y activación de variables de control
    if(ds_valor_front < DIST_LEJANO){
      front_detect = true;
    }
    if(ds_valor_izq < DIST_LADO){
      izq_detect = true;
    }
    if(ds_valor_der < DIST_LADO){
      der_detect = true;
    }

    // No hay deteccion
    if (not front_detect and not izq_detect and not der_detect){
      izq_speed = MAX_SPEED; 
      der_speed = MAX_SPEED; 
    }

    // deteccion frontal
    if (front_detect){
      // deteccion izq y der
      if(izq_detect and der_detect){   
        izq_speed = -VEL_MEDIA;
        der_speed = -VEL_MEDIA;       
      }
      // deteccion izq
      else if(izq_detect){      
        izq_speed = VEL_ALTA;
        der_speed = VEL_BAJA;
      }
      // deteccion der
      else if(der_detect){     
        izq_speed = VEL_BAJA;
        der_speed = VEL_ALTA;  
      }
      // solo front
      else{
        // Se alterna entre lado más amplio
        if(ds_valor_izq > ds_valor_der){ 
          izq_speed = VEL_BAJA;
          der_speed = VEL_ALTA;
        } else{
          izq_speed = VEL_BAJA;
          der_speed = VEL_ALTA;
        }
      }
      
      // Si hay alguna deteccion cercana no se mueve, rota.
      if(ds_valor_front < DIST_CERCANO || ds_valor_izq < DIST_CERCANO || ds_valor_der < DIST_CERCANO){        
        izq_speed = VEL_MEDIA;
        der_speed = -VEL_MEDIA; 
      }
    }
    // Detecciones laterales
    else{
      // Deteccion izq
      if(izq_detect and not der_detect){
        izq_speed = VEL_ALTA;
        der_speed = VEL_BAJA;
      }
      // Deteccion der
      else if(not izq_detect and der_detect){
        izq_speed = VEL_BAJA;
        der_speed = VEL_ALTA;
      }
      // Sigue recto
      else{
        izq_speed = VEL_ALTA; 
        der_speed = VEL_ALTA;
      }
    }
    
    // Seteo de velocidades al terminar
    setSpeed(izq_speed, der_speed);
    
  };

  // Enter here exit cleanup code.

  delete robot;
  return 0;
}

void getSensorValues(){
    ds_valor_front = ds_front->getValue();
    ds_valor_der = ds_right->getValue();
    ds_valor_izq = ds_left->getValue();
}

void setSpeed(float izq, float der){
  front_left_motor->setVelocity(izq);
  back_left_motor->setVelocity(izq);
  front_right_motor->setVelocity(der);
  back_right_motor->setVelocity(der);   
}