#!/usr/bin/env python2.7
# -*- coding: utf-8 -*-

import rospy 
import std_msgs
import time
from geometry_msgs.msg import Twist
from kobuki_msgs.msg import BumperEvent


class bumperEvent():

    # Topics a traves de los que se recibe o publica informacion
    _tb4_velocity_topic = '/tb4/mobile_base/commands/velocity'
    _tb2_velocity_topic = '/tb2/mobile_base/commands/velocity'
    _tb4_bumper_topic = '/tb4/mobile_base/events/bumper'
    _bumper_state = False

    def __init__(self):

        # Partes publisher en diferentes topics
        self.tb4_pub = rospy.Publisher(self._tb4_velocity_topic, Twist, queue_size=10)
        self.tb2_pub = rospy.Publisher(self._tb2_velocity_topic, Twist, queue_size=10)

        self.tb4_bumper_sub = rospy.Subscriber(self._tb4_bumper_topic, BumperEvent, self._bumper_callback) 	# Bumper Message

    def _publish_vel(self, lin, ang):
        """	
        Publica la velocidad recibida en todos los robots

        Args:
            :arg  lin:      Velocidad de avance
            :arg  ang:      Velocidad de giro
        """

        # Se configura el tipo de mensaje a publicar
        self._speed=Twist()

        # Asignacion de velocidades en la variable a publicar
        self._speed.linear.x = lin; self._speed.linear.y = 0; self._speed.linear.z = 0
        self._speed.angular.x = 0; self._speed.angular.y = 0; self._speed.angular.z = 0
        
        # Si se ha activado el bumper se para y gira el robot
        if self._bumper_state: 
            self._speed.linear.x = -0.3
            self._speed.angular.z = -0.5
            self._bumper_state = False

            for i in range(0,10000):
                self.tb4_pub.publish(self._speed)
                self.tb2_pub.publish(self._speed)
            
        # Publicacion de los valores de velocidad
        rospy.loginfo("Avance: {}; Giro: {}".format(self._speed.linear.x, self._speed.angular.z))
        self.tb4_pub.publish(self._speed);      self.tb2_pub.publish(self._speed)
        

    # Callback de la suscripción al topic del bumper	
    def _bumper_callback(self, data_bump):

        #rospy.loginfo(data_bump.state)
        if data_bump.state == BumperEvent.PRESSED: 
            self._bumper_state = True

    def run(self):
        """	
        Mueve los diferentes Turtlebots coordinados al mismo tiempo con mismos movimientos

        """

        # Se inicializa el nodo 
        rospy.init_node("Move_TB", anonymous=True)

        # Velocidades iniciales
        lin_vel = 0.5
        ang_vel = 0

        # Frecuencia de actualizacion
        r = rospy.Rate(10)

        while True:    
            lin_vel = 0.1
            ang_vel = 0

            # Se publica en todos los robots la misma velocidad
            self._publish_vel(lin_vel, ang_vel)

            # Se libera carga para no saturar la simulacion ni los robots
            r.sleep()

        rospy.spin()

if __name__ == '__main__': 
    
    TB = bumperEvent()
    TB.run()