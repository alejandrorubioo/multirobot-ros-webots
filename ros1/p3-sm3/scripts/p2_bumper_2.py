#!/usr/bin/env python
# -*- coding: utf-8 -*-

# ROS
import rospy
import sys
from geometry_msgs.msg import Twist, Vector3
from kobuki_msgs.msg import BumperEvent
from std_msgs.msg import String
import time

MAX_LINEAR = 0.5
MAX_ANGULAR = 1

class BumperMov:

    def __init__(self):

        # Se guarda argumento del robot
        self.arg_robot    =sys.argv[1]      # p.e. rosrun bumper.py robot1

        # Inicializar el nodo
        rospy.init_node("bumper_" + self.arg_robot)

        # Publisher
        self.pub_tb1 = rospy.Publisher('/' + self.arg_robot + '/mobile_base/commands/velocity', Twist, queue_size=10)

        # Subscriber
        self.bumber_subs = rospy.Subscriber('/' + self.arg_robot + '/mobile_base/events/bumper', BumperEvent, self.bumper_callback)
        
        # Publishing rate
        self.rate = rospy.Rate(20)

        # Velocidades
        self.linear_x   = MAX_LINEAR
        self.ang_z      = 0

        # Bumpers
        self.bumper_detected = False
        self.bumper_left     = False
        self.bumper_center   = False
        self.bumper_right    = False
     

    def start_movement(self):
        vel_msg = Twist()
        
        while not rospy.is_shutdown():

            # Si se detecta bumper
            if self.bumper_detected:
                self.moveBack()

            # Se define mensaje
            vel_msg.linear.x = self.linear_x
            vel_msg.angular.z = self.ang_z
   
            # Se publica velocidad
            self.pub_tb1.publish(vel_msg)

            # wait for 0.1 seconds (10 HZ) and publish again
            self.rate.sleep()
            #rospy.sleep()

            
    def moveBack(self):
        # Se publica velocidad en cada iteración
        # vel_msg = Twist(Vector3(self.linear_x,0,0),Vector3(0,0,self.ang_z))

        # Se actualiza el contador
        self.actual_time = time.time()
        self.t = self.actual_time - self.prev_time

        # Marcha atras
        if self.t < 2:
            self.linear_x = -MAX_LINEAR
        
        # Giro dependiendo de lado
        elif self.t <5:
            self.linear_x = MAX_LINEAR * -0.5
            if self.bumper_left:
                self.ang_z = 0.6
            elif self.bumper_right:
                self.ang_z = -0.6
            elif self.bumper_center:
                self.ang_z = 1.2

        # Sale del bucle
        else:
            self.linear_x = MAX_LINEAR
            self.ang_z = 0
            # Reset
            self.prev_time = self.actual_time
            self.bumper_detected = False
            self.bumper_left     = False
            self.bumper_center   = False
            self.bumper_right    = False


    def bumper_callback(self, data):
        if data.state == BumperEvent.PRESSED:
            self.bumper_detected = True
            if data.bumper == BumperEvent.LEFT:
                rospy.loginfo("Bumper LEFT")
                self.bumper_left = True
            elif data.bumper == BumperEvent.CENTER:
                rospy.loginfo("Bumper CENTER")
                self.bumper_center = True
            elif data.bumper == BumperEvent.RIGHT:
                rospy.loginfo("Bumper RIGHT")
                self.bumper_right = True


            # Reset variables de tiempo
            self.actual_time = time.time()
            self.prev_time = self.actual_time
            self.t = self.actual_time - self.prev_time


if __name__ == '__main__':
    try:
        x = BumperMov()
        x.start_movement()

    except rospy.ROSInterruptException:
        pass