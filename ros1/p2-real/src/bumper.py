#!/usr/bin/env python
# license removed for brevity
import rospy
import random
from geometry_msgs.msg import Twist
from kobuki_msgs.msg import BumperEvent

#Booleano para saber si el Bumper ha sido golpeado
presionado = False

#Booleano para para saber que lado ha sido golpeado
IZQ = False
CEN = False
DER = False

def linearMove():
    turtlebot = rospy.Publisher('/tb4/mobile_base/commands/velocity', Twist, queue_size=10)
    twist = Twist()
    twist.linear.x = 0.2
    turtlebot.publish(twist)

def stop():
    turtlebot = rospy.Publisher('/tb4/mobile_base/commands/velocity', Twist, queue_size=10)
    twist = Twist()
    twist.linear.x = 0
    turtlebot.publish(twist)

def marchaAtras(giro):
    turtlebot = rospy.Publisher('/tb4/mobile_base/commands/velocity', Twist, queue_size=10)
    twist = Twist()
    twist.linear.x = -0.2
    twist.angular.z = giro
    turtlebot.publish(twist)

def callback_bumper(data):
    global presionado
    global IZQ
    global CEN
    global DER
    print("Dentro callbaxck")
    #Para saber si ha sido presionado
    if data.state == BumperEvent.PRESSED:
        presionado = True  

    if presionado == True:

        #Para saber que lado ha sido presionado: IZQ,DER,CEN
        if data.bumper == 0: #IZQ
            print("Golpe Izquierda")
            IZQ = True
        if data.bumper == 1: #CEN
            print("Golpe Central")
            CEN = True
        if data.bumper == 2: #DER
            print("Golpe Derecha")
            DER = True

def mover_turtlebots():
    global presionado
    global IZQ
    global CEN
    global DER

    #Nos suscribirmos al bumper
    turt1 = rospy.Subscriber('tb4/mobile_base/events/bumper', BumperEvent, callback_bumper)

    #Creamos un nodo
    rospy.init_node('movimiento', anonymous=True)

    rate = rospy.Rate(15) # 10hz

    # Para mantener el codigo ejecutandose
    while not rospy.is_shutdown():
        if presionado == False:
            linearMove()
        else:
            stop()

            if IZQ:
                for x in range(0,20): # 10it x 5hz = 10 x 1/5 = 2 segundos aprox
                    marchaAtras(-0.4)
                    rate.sleep()
                IZQ = False
            if CEN:
                #Elegimos aleatoriametne si giramos a la izquierda o a la derecha
                sentido_giro = random.randint(0, 1)
                if sentido_giro == 0:
                    giro = 0.4
                else:
                    giro = -0.4

                for x in range(0,20): # 10it x 5hz = 10 x 1/5 = 2 segundos aprox
                    marchaAtras(giro)
                    rate.sleep()
                CEN = False
            if DER:
                for x in range(0,20): # 10it x 5hz = 10 x 1/5 = 2 segundos aprox
                    marchaAtras(0.4)
                    rate.sleep()
                DER = False

            presionado = False

if __name__ == '__main__':
    try:
        mover_turtlebots()
    except rospy.ROSInterruptException:
        pass
