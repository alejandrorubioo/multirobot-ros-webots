// File:          Flocking.cpp
// Date:
// Description: Movimiento coordinado
// Author:      Alejandro Rubio
// Modifications:

#include <stdio.h>

// Librerías requeridas para los sensores y actuadores utilizados
#include <webots/Robot.hpp>
#include <webots/Motor.hpp>
#include <webots/DistanceSensor.hpp>

#define TIME_STEP 64

// Contador de  cambio de direccion
#define COUNT_GIRO 50

// Distancias
#define DIST_LEJANO 230
#define DIST_MEDIO  150
#define DIST_CERCANO  20
#define DIST_LADO 200

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
bool stop_robot = false;

// Velocidad auxiliar en motor
float izq_speed, der_speed; 
int count_dir = 0;          // Flag de direccion

int main(int argc, char **argv) {
  
  // Motors initialization
  front_left_motor->setPosition(INFINITY);  front_left_motor->setVelocity(0.0);
  front_right_motor->setPosition(INFINITY); front_right_motor->setVelocity(0.0);
  back_left_motor->setPosition(INFINITY);   back_left_motor->setVelocity(0.0);
  back_right_motor->setPosition(INFINITY);  back_right_motor->setVelocity(0.0);
  
  // Sensor inizialization
  ds_left->enable(TIME_STEP);
  ds_front->enable(TIME_STEP);
  ds_right->enable(TIME_STEP);

  izq_speed = VEL_MEDIA;
  der_speed = VEL_MEDIA; 

  // Bucle principal:
  while (robot->step(TIME_STEP) != -1) {
      
    // Se obtiene valor de sensores
    getSensorValues();
    
    // Comienzan a false
    front_detect = false;
    izq_detect = false;
    der_detect = false;

    // Se avanza girando si aun hay distancia
    if(ds_valor_front > DIST_LEJANO){
      // Varia cada X iteraciones 
      if (count_dir < COUNT_GIRO){     
        izq_speed = VEL_MEDIA;
        der_speed = VEL_BAJA;
      } else{       
        izq_speed = VEL_BAJA;
        der_speed = VEL_MEDIA;
      
        // Reset
        if(count_dir >= 2*COUNT_GIRO){
          count_dir = 0; 
        }      
      }
      count_dir ++; 
    }

    // Distancia media
    else if(ds_valor_front > DIST_MEDIO){
      izq_speed = VEL_ALTA;
      der_speed = VEL_ALTA; 
    }
    // Distancia baja
    else if(ds_valor_front > DIST_CERCANO){
      izq_speed = VEL_BAJA;
      der_speed = VEL_BAJA; 
    }
    else{
      izq_speed = 0;
      der_speed = 0;
    }
    
    // Deteccion izquierda
    if(ds_valor_izq < DIST_LADO and ds_valor_izq > DIST_CERCANO){
      // Se avanza hacia la izquierda
      izq_speed = VEL_BAJA;
      der_speed = VEL_ALTA;    
    }
    // Deteccion izquierda MUY proxima
    else if(ds_valor_izq < DIST_CERCANO){
      izq_speed = VEL_MEDIA;
      der_speed = 0;
    }     

    // Deteccion derecha
    if(ds_valor_der < DIST_LADO and ds_valor_der > DIST_CERCANO){
      // Se avanza hacia la derecha
      izq_speed = VEL_ALTA;
      der_speed = VEL_BAJA;
    }
    // Deteccion derecha MUY proxima
    else if(ds_valor_der < DIST_CERCANO){
      izq_speed = 0;
      der_speed = VEL_MEDIA; 
    }     

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
