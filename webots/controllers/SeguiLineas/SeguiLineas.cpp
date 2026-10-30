// File:          SeguiLineas.cpp
// Date:
// Description:   Seguimiento de línea
// Author:        Alejandro Rubio
// Modifications:

#include <stdio.h>
#include <ctime>

// Librerías requeridas para los sensores y actuadores utilizados
#include <webots/Robot.hpp>
#include <webots/Motor.hpp>
#include <webots/DistanceSensor.hpp>

void avoidObstacles();
void seguiLineas();
void chooseRandomDirection(float, float);

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
#define VEL_ALTA -6
#define VEL_BAJA  -3

// Umbral por debajo del que está la línea blanca en el infrarrojos
#define BLACK_LINE 300

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
DistanceSensor *IR_right = robot->getDistanceSensor("IR_1");
DistanceSensor *IR_front = robot->getDistanceSensor("IR_2");
DistanceSensor *IR_left = robot->getDistanceSensor("IR_3");

// Valor de sensores
double ds_valor_izq, ds_valor_front, ds_valor_der;
double ir_valor_izq, ir_valor_front, ir_valor_der;

// Variables de linea
bool found_line = false; 

// Velocidad auxiliar en motor
float izq_speed, der_speed; 

// Timer variable
double currentTime=0, prevTime=0;

// Indice aleatorio para cambios de direcciones
float leftIndex=0, rightIndex=0;

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
  IR_right->enable(TIME_STEP);
  IR_front->enable(TIME_STEP);
  IR_left->enable(TIME_STEP);

  izq_speed = VEL_MEDIA;
  der_speed = VEL_MEDIA; 

  // Bucle principal:
  while (robot->step(TIME_STEP) != -1) {
      
    // Se obtiene valor de sensores
    getSensorValues();
    
    // Si está buscando la línea se esquivan obstáculos
    if(!found_line){
      // Cada 5 segundos se cambia ligeramente de direccion para escoger caminos aleatorios (durante 1 segundo)
        if (robot->getTime() - prevTime >= 3){
          if (leftIndex==0 and rightIndex==0){
            leftIndex = (std::rand() % 100); leftIndex *= 0.01;
            rightIndex = (std::rand() % 100); rightIndex *= 0.01;
          }
          // Se mueve el robot
          chooseRandomDirection(leftIndex, rightIndex);

          // Se reinicia contador al pasar de 6 segundos
          if (robot->getTime() - prevTime >= 4){
            prevTime = robot->getTime();
            leftIndex = 0; rightIndex=0;
          }
        }
        // Mientras tanto, se evitan obstaculos
        else{
          // Esquiva de obstaculos
          avoidObstacles();
        }

        if(ir_valor_izq > BLACK_LINE or ir_valor_front > BLACK_LINE or ir_valor_der > BLACK_LINE){
        found_line = true; 
      }
    }
    // Comineza a seguir liena
    else if (found_line){
      seguiLineas();
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
    ir_valor_izq = IR_left->getValue();
    ir_valor_front = IR_front->getValue();
    ir_valor_der = IR_right->getValue();
}

void avoidObstacles(){

  bool left_detection = false;
  bool front_detection = false;
  bool right_detection = false;
  float izq_speed = 0, der_speed = 0; 

  // Comprobación de sensores
  if(ds_valor_front < DIST_LEJANO){
    front_detection = true;
  }
  if(ds_valor_izq < DIST_LADO){
    left_detection = true;
  }
  if(ds_valor_der < DIST_LADO){
    right_detection = true;
  }    

  // Distancia frontal
  if(front_detection){
      // Deteccion tambien en izquierda y derecha
      if(left_detection and right_detection){   
        // Retroceso lento
        izq_speed = -VEL_MEDIA;
        der_speed = -VEL_MEDIA;       
      }
      // Solo deteccion por un lado
      else if(left_detection){      
        izq_speed = VEL_ALTA;
        der_speed = VEL_BAJA;
      }
      else if(right_detection){     
        izq_speed = VEL_BAJA;
        der_speed = VEL_ALTA;  
      }
      else{
        // Giro hacia izquierda
        if(ds_valor_izq > ds_valor_der){ 
          izq_speed = VEL_BAJA;
          der_speed = VEL_ALTA;
        }
        // Giro hacia derecha
        else{
          izq_speed = VEL_BAJA;
          der_speed = VEL_ALTA;
        }
      }
      
      // Aun así, si el valor de deteccion es pequeño, el robot rota
      if(ds_valor_front < DIST_CERCANO || ds_valor_izq < DIST_CERCANO || ds_valor_der < DIST_CERCANO){        
        // Se retrocede con precaución 
        izq_speed = VEL_MEDIA;
        der_speed = -VEL_MEDIA; 
      }
    }
    // Detecciones laterales
    else{
      if(left_detection && !right_detection){
        izq_speed = VEL_ALTA;
        der_speed = VEL_BAJA;
      }
      else if(!left_detection && right_detection){
        izq_speed = VEL_BAJA;
        der_speed = VEL_ALTA;
      }
      // El robot sigue recto, caso más prioritario
      else{
        izq_speed = VEL_ALTA; 
        der_speed = VEL_ALTA;
      }
    }
    
    // La velocidad de los motores se asigna
    front_left_motor->setVelocity(izq_speed);
    back_left_motor->setVelocity(izq_speed);
    front_right_motor->setVelocity(der_speed);
    back_right_motor->setVelocity(der_speed);   
}

void chooseRandomDirection(float left_index, float right_index){
  // Hace giros aleatorios
  if (leftIndex>rightIndex){
    front_left_motor->setVelocity(VEL_ALTA);
    back_left_motor->setVelocity(VEL_ALTA);
    front_right_motor->setVelocity(right_index*VEL_ALTA);
    back_right_motor->setVelocity(right_index*VEL_ALTA);  
  }
  else{
    front_left_motor->setVelocity(left_index*VEL_ALTA);
    back_left_motor->setVelocity(left_index*VEL_ALTA);
    front_right_motor->setVelocity(VEL_ALTA);
    back_right_motor->setVelocity(VEL_ALTA);   
  }
}

void seguiLineas(){
  // ambos lados
  if(ir_valor_izq > BLACK_LINE && ir_valor_der > BLACK_LINE){
    izq_speed = VEL_MEDIA;
    der_speed = VEL_MEDIA;
  }
  // deteccion izquierda
  else if(ir_valor_izq > BLACK_LINE){
    izq_speed = -VEL_BAJA;
    der_speed = VEL_ALTA;
  }
  // deteccion derecha
  else if(ir_valor_der > BLACK_LINE){
    izq_speed = VEL_ALTA;
    der_speed = -VEL_BAJA;
  }
  // Se ha perdido linea
  else {
      found_line = false;
  } 

  setSpeed(izq_speed, der_speed);
}

void setSpeed(float izq, float der){
  front_left_motor->setVelocity(izq);
  back_left_motor->setVelocity(izq);
  front_right_motor->setVelocity(der);
  back_right_motor->setVelocity(der);   
}
