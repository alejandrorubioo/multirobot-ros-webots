#!/usr/bin/env python
# -- coding: utf-8 --

# ROS
import rospy
from geometry_msgs.msg import Twist, Vector3
from std_msgs.msg import String

def ej1():

    # Topics
    pub_tb1 = rospy.Publisher('/robot1/mobile_base/commands/velocity', Twist, queue_size=10)
    pub_tb2 = rospy.Publisher('/robot2/mobile_base/commands/velocity', Twist, queue_size=10)
    pub_tb3 = rospy.Publisher('/robot3/mobile_base/commands/velocity', Twist, queue_size=10)
    pub_tb4 = rospy.Publisher('/robot4/mobile_base/commands/velocity', Twist, queue_size=10)

    # Nodo
    rospy.init_node("ej1_task")

    #Velocidades
    x_value=0.4
    wz_value=0.7
    
    while not rospy.is_shutdown():
    
        #Messages deffinition
        tb1= Twist(Vector3(x_value,0,0),Vector3(0,0,wz_value))
        tb2=Twist(Vector3(x_value,0,0),Vector3(0,0,wz_value))
        tb3=Twist(Vector3(x_value,0,0),Vector3(0,0,wz_value))
        tb4=Twist(Vector3(x_value,0,0),Vector3(0,0,wz_value))

        #The message is published
        pub_tb1.publish(tb1)
        pub_tb2.publish(tb2)
        pub_tb3.publish(tb3)
        pub_tb4.publish(tb4)

        # Comenzaran a dar circulos a velocdad angular 0.7

if __name__ == "__main__":
    try:
       ej1()
    except rospy.ROSInterruptException:
        pass