// File:          Foraging.cpp
// Date:
// Description:   Busqueda de alimentos y vuelta a zona
// Author:        Alejandro Rubio
// Modifications:

// You may need to add webots include files such as
// <webots/DistanceSensor.hpp>, <webots/Motor.hpp>, etc.
// and/or to add some other includes
#include <stdio.h>
#include <ctime>

// You may need to add webots include files such as
// <webots/DistanceSensor.hpp>, <webots/Motor.hpp>, etc.
// and/or to add some other includes
#include <webots/Robot.hpp>
#include <webots/Motor.hpp>
#include <webots/DistanceSensor.hpp>


void getSensorValues();
void avoidObstacles();
void PickPlaceFood(int, int);
void chooseRandomDirection(float, float);
void stopRobot();


#define TIME_STEP 64
#define MAX_SPEED -10

// Velocidad máxima de avance (invertida, negativo hacia delante)
#define HIGH_SPEED -8
#define MID_SPEED  -6
#define LOW_SPEED  -2

// Definición de distancias para el control 
#define HIGH_DIST 250
#define MID_DIST  150 
#define LOW_DIST  100
#define SIDE_DIST 150

// Definicion de valores minimos y maximos de superficie de colores
const int DEST_LOW_LIMIT = 85;
const int DEST_HIGH_LIMIT = 95;
const int HOME_LOW_LIMIT = 415;
const int HOME_HIGH_LIMIT = 425;

// All the webots classes are defined in the "webots" namespace
using namespace webots;
using namespace std;

// create the Robot instance.
Robot *robot = new Robot();

enum States {START, AVOID_OBSTACLES, PICK_FOOD, FINISH};
int currentState = START;
int previousState = currentState;

// Motors setup 
Motor *front_left_motor = robot->getMotor("MOTOR_FRONT_LEFT");
Motor *front_right_motor = robot->getMotor("MOTOR_FRONT_RIGHT");
Motor *back_left_motor = robot->getMotor("MOTOR_BACK_LEFT");
Motor *back_right_motor = robot->getMotor("MOTOR_BACK_RIGHT");

// Distance sensors: 0-35cm in range [0, 350]
DistanceSensor *ds_front = robot->getDistanceSensor("front_ds");
DistanceSensor *ds_left = robot->getDistanceSensor("left_ds");
DistanceSensor *ds_right = robot->getDistanceSensor("right_ds");

//IR sensors: black(440) white(90)
DistanceSensor *IR_dch = robot->getDistanceSensor("IR_1");
DistanceSensor *IR_front = robot->getDistanceSensor("IR_2");
DistanceSensor *IR_izq = robot->getDistanceSensor("IR_3");

// Sensor values:
double ds_front_val, ds_right_val, ds_left_val;   // Distance
double IR_dch_val, IR_front_val, IR_izq_val;       // IR

// Timer variable
double currentTime=0, prevTime=0;

// Indice aleatorio para cambios de direcciones
float leftIndex=0, rightIndex=0;

// Main loop:
int main(int argc, char **argv) {

  // Semilla aleatoria 
  std::srand(std::time(nullptr));

  // Motors initialization
  front_left_motor->setPosition(INFINITY);
  front_right_motor->setPosition(INFINITY);
  back_left_motor->setPosition(INFINITY);
  back_right_motor->setPosition(INFINITY);
  // Velocity to 0
  front_left_motor->setVelocity(0);
  front_right_motor->setVelocity(0);
  back_left_motor->setVelocity(0);
  back_right_motor->setVelocity(0);

  // Distance sensors activation
  ds_front->enable(TIME_STEP);
  ds_left->enable(TIME_STEP);
  ds_right->enable(TIME_STEP);
  // IR sensors activation
  IR_dch->enable(TIME_STEP);
  IR_front->enable(TIME_STEP);
  IR_izq->enable(TIME_STEP);

  while (robot->step(TIME_STEP) != -1) {
    
    // Se obtiene valor de sensores
    getSensorValues();

    switch(currentState){

      case START:
      // 2 segundos de parada antes de comenzar
        if (robot->getTime() > 3){
          previousState = currentState;
          currentState = AVOID_OBSTACLES;

          // Se reinicia tiempo
          prevTime = 0;
        }
        break;

      case AVOID_OBSTACLES:
        // Cada 5 segundos se cambia ligeramente de direccion para escoger caminos aleatorios (durante 1 segundo)
        if (robot->getTime() - prevTime >= 5){
          if (leftIndex==0 and rightIndex==0){
            leftIndex = (std::rand() % 100); leftIndex *= 0.01;
            rightIndex = (std::rand() % 100); rightIndex *= 0.01;
          }

          // Se mueve el robot
          chooseRandomDirection(leftIndex, rightIndex);

          // Se reinicia contador al pasar de 6 segundos
          if (robot->getTime() - prevTime >= 6){
            prevTime = robot->getTime();
            leftIndex = 0; rightIndex=0;
          }
        }
        // Mientras tanto, se evitan obstaculos
        else{
          // Esquiva de obstaculos
          avoidObstacles();

          if (previousState == START){
            // Si se detecta color de comida:
            if ((IR_front_val > DEST_LOW_LIMIT) and (IR_front_val < DEST_HIGH_LIMIT)){
              previousState = currentState;
              currentState = PICK_FOOD;
              cout << "Detección de zona de comida" << endl;
              // Se activa temporizador para "recolectar" comida
              prevTime = robot->getTime();
            }
          }
          else if (previousState == PICK_FOOD){
            // Si se detecta color de inicio:
            if ((IR_front_val > HOME_LOW_LIMIT) and (IR_front_val < HOME_HIGH_LIMIT)){
              previousState = currentState;
              currentState = FINISH;
              cout << "Detección de casa" << endl;
              // Se activa temporizador para terminar tarea
              prevTime = robot->getTime();
            }
          }
        }
        break;

      case PICK_FOOD:
        // Va muy lento por el circulo durante 7 segundos, si detecta a alguien se queda quieto:
        PickPlaceFood(DEST_LOW_LIMIT, DEST_HIGH_LIMIT);
        if (robot->getTime()-prevTime > 7){
          previousState = currentState;
          currentState = AVOID_OBSTACLES;
        }
        // Vuelve a estado de obtstaculos
        break;
      
      case FINISH:
        // Va lento por el círculo durante 7 segundos
        if (robot->getTime()-prevTime < 7){
          PickPlaceFood(HOME_LOW_LIMIT, HOME_HIGH_LIMIT);
        } 
        // Tarea terminada.
        else {
          // El robot se queda quieto
          stopRobot();
        }
        break;

      default:
        break;
    }
  };

  delete robot;
  return 0;
}

/** SE OBTIENE VALOR DE SENSORES **/
void getSensorValues(){
    ds_front_val = ds_front->getValue();
    ds_right_val = ds_right->getValue();
    ds_left_val = ds_left->getValue();
    IR_dch_val = IR_dch->getValue();
    IR_front_val = IR_front->getValue();
    IR_izq_val = IR_izq->getValue();
}

/** ACTÚA EN FUNCION DE DETECCIONES LATERALES Y FRONTALES **/
void avoidObstacles(){

  bool left_detection = false;
  bool front_detection = false;
  bool right_detection = false;
  float left_vel = 0, right_vel = 0; 

  // Comprobación de sensores
  if(ds_front_val < HIGH_DIST){
    front_detection = true;
  }
  if(ds_left_val < SIDE_DIST){
    left_detection = true;
  }
  if(ds_right_val < SIDE_DIST){
    right_detection = true;
  }    

  // Distancia frontal
  if(front_detection){
      // Deteccion tambien en izquierda y derecha
      if(left_detection && right_detection){   
        // Retroceso lento
        left_vel = -MID_SPEED;
        right_vel = -MID_SPEED;       
      }
      // Solo deteccion por un lado
      else if(left_detection){      
        left_vel = HIGH_SPEED;
        right_vel = LOW_SPEED;
      }
      else if(right_detection){     
        left_vel = LOW_SPEED;
        right_vel = HIGH_SPEED;  
      }
      else{
        // Giro hacia izquierda
        if(ds_left_val > ds_right_val){ 
          left_vel = LOW_SPEED;
          right_vel = HIGH_SPEED;
        }
        // Giro hacia derecha
        else{
          left_vel = LOW_SPEED;
          right_vel = HIGH_SPEED;
        }
      }
      
      // Aun así, si el valor de deteccion es pequeño, el robot rota
      if(ds_front_val < LOW_DIST || ds_left_val < LOW_DIST || ds_right_val < LOW_DIST){        
        // Se retrocede con precaución 
        left_vel = MID_SPEED;
        right_vel = -MID_SPEED; 
      }
    }
    // Detecciones laterales
    else{
      if(left_detection && !right_detection){
        left_vel = HIGH_SPEED;
        right_vel = LOW_SPEED;
      }
      else if(!left_detection && right_detection){
        left_vel = LOW_SPEED;
        right_vel = HIGH_SPEED;
      }
      // El robot sigue recto, caso más prioritario
      else{
        left_vel = HIGH_SPEED; 
        right_vel = HIGH_SPEED;
      }
    }
    
    // La velocidad de los motores se asigna
    front_left_motor->setVelocity(left_vel);
    back_left_motor->setVelocity(left_vel);
    front_right_motor->setVelocity(right_vel);
    back_right_motor->setVelocity(right_vel);   
}

void PickPlaceFood(const int LOW_LIMIT, const int HIGH_LIMIT){

  bool left_detection = false;
  bool front_detection = false;
  bool right_detection = false;
  float left_vel = 0, right_vel = 0; 

  // Comprobación de sensores
  if(ds_front_val < LOW_DIST/10){
    front_detection = true;
  }
  if(ds_left_val < LOW_DIST/10){
    left_detection = true;
  }
  if(ds_right_val < LOW_DIST/10){
    right_detection = true;
  }  

  // Si hay deteccion o ha dejado de detectar el color de destino
  if ((front_detection or left_detection or right_detection) or 
      (IR_front_val < LOW_LIMIT) or (IR_front_val > HIGH_LIMIT)){
    left_vel = 0;
    right_vel = 0; 
  }
  // Va lento por la zona hasta que cambie de estado
  else {
    left_vel = LOW_SPEED;
    right_vel = LOW_SPEED; 
  }

  // Asignación de velocidades de los motores 
  front_left_motor->setVelocity(left_vel);
  back_left_motor->setVelocity(left_vel);
  front_right_motor->setVelocity(right_vel);
  back_right_motor->setVelocity(right_vel);   
}

void chooseRandomDirection(float left_index, float right_index){
  // Hace giros aleatorios
  if (leftIndex>rightIndex){
    front_left_motor->setVelocity(MAX_SPEED);
    back_left_motor->setVelocity(MAX_SPEED);
    front_right_motor->setVelocity(right_index*MAX_SPEED);
    back_right_motor->setVelocity(right_index*MAX_SPEED);  
  }
  else{
    front_left_motor->setVelocity(left_index*MAX_SPEED);
    back_left_motor->setVelocity(left_index*MAX_SPEED);
    front_right_motor->setVelocity(MAX_SPEED);
    back_right_motor->setVelocity(MAX_SPEED);   
  }
}

void stopRobot(){
    front_left_motor->setVelocity(0);
    back_left_motor->setVelocity(0);
    front_right_motor->setVelocity(0);
    back_right_motor->setVelocity(0);   
}

