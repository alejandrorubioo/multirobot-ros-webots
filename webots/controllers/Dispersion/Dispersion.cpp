// File:          Dispersion.cpp
// Date:
// Description:   Dispersion de grupo
// Author:        Alejandro Rubio
// Modifications:

#include <stdio.h>

// Librerías requeridas para los sensores y actuadores utilizados
#include <webots/Robot.hpp>
#include <webots/Motor.hpp>
#include <webots/DistanceSensor.hpp>

#define TIME_STEP 64

// Distancias
#define DIST_LEJANO 250
#define DIST_MEDIO  145 
#define DIST_CERCANO  20
#define DIST_LADO 145

// Velocidades 
#define MAX_SPEEED -10
#define VEL_MEDIA  -5
#define VEL_ALTA -9
#define VEL_BAJA  -3

// All the webots classes are defined in the "webots" namespace
using namespace webots;
using namespace std;

// PROTOTIPOS
void getSensorValues();
void setSpeed(float, float);


// Se instancia el robot - variable global
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
bool izq_detect;
bool front_detect;
bool der_detect;
bool stop_robot = false;

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

    // Comprobación de distancia cercana
    if(ds_valor_front < DIST_LEJANO){
      front_detect = true;
    }
    if(ds_valor_izq < DIST_LEJANO){
      izq_detect = true;
    }
    if(ds_valor_der < DIST_LEJANO){
      der_detect = true;
    }
    // Se parará el robot si no hay deteccion
    if (not front_detect and
        not izq_detect and
        not der_detect){
      stop_robot = true;
    }
    else stop_robot = false;

    // No hay condicion de parada
    if (not stop_robot){
      // deteccion frontal o laterales
      if (front_detect or (der_detect and izq_detect)){
        izq_speed = -VEL_MEDIA;
        der_speed = -VEL_MEDIA;
      }
      else if (der_detect){
        izq_speed = 0;
        der_speed = -VEL_MEDIA;
      }
      else if (izq_detect){
        izq_speed = -VEL_MEDIA;
        der_speed = 0;
      }
      else {
        izq_speed = 0;
        der_speed = 0;
      }
      
      // Seteo de velocidades al terminar
      setSpeed(izq_speed, der_speed);
    }
    else {
      setSpeed(0, 0);
    }
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
