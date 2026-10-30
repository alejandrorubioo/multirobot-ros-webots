#!/usr/bin/env python
# license removed for brevity
import rospy
import random
from geometry_msgs.msg import Twist
from kobuki_msgs.msg import BumperEvent

#Definimos donde vamos a publicar
turt1 = rospy.Publisher('/tb4/mobile_base/commands/velocity', Twist, queue_size=10)
turt2 = rospy.Publisher('/tb2/mobile_base/commands/velocity', Twist, queue_size=10)

#Booleano para saber si el Bumper ha sido golpeado
tb1_presionado = False
tb2_presionado = False

#Booleano para para saber que lado ha sido golpeado
tb1_IZQ = False
tb1_CEN = False
tb1_DER = False
tb2_IZQ = False
tb2_CEN = False
tb2_DER = False

def linearMove(topic):
    twist = Twist()
    twist.linear.x = 0.3
    topic.publish(twist)

def stop(topic):
    twist = Twist()
    twist.linear.x = 0
    topic.publish(twist)

def marchaAtras(giro, topic):
    twist = Twist()
    twist.linear.x = -0.3
    twist.angular.z = giro
    topic.publish(twist)

def callback_bumper(data):
    global tb1_presionado
    global tb1_IZQ
    global tb1_CEN
    global tb1_DER

    #Para saber si ha sido presionado
    if data.state == BumperEvent.PRESSED:
        tb1_presionado = True  
        stop(turt1)

    if tb1_presionado == True:
        print("TB1 - Bumper presionado")

        #Para saber que lado ha sido presionado: IZQ,DER,CEN
        if data.bumper == 0: #IZQ
            print("  Golpe Izquierda")
            tb1_IZQ = True
        if data.bumper == 1: #CEN
            print("  Golpe Central")
            tb1_CEN = True
        if data.bumper == 2: #DER
            print("  Golpe Derecha")
            tb1_DER = True

def callback_bumper_2(data):
    global tb2_presionado
    global tb2_IZQ
    global tb2_CEN
    global tb2_DER

    #Para saber si ha sido presionado
    if data.state == BumperEvent.PRESSED:
        tb2_presionado = True  
        stop(turt2)

    if tb2_presionado == True:
        print("TB2 - Bumper presionado")

        #Para saber que lado ha sido presionado: IZQ,DER,CEN
        if data.bumper == 0: #IZQ
            print("  Golpe Izquierda")
            tb2_IZQ = True
        if data.bumper == 1: #CEN
            print("  Golpe Central")
            tb2_CEN = True
        if data.bumper == 2: #DER
            print("  Golpe Derecha")
            tb2_DER = True

def mover_turtlebots():
    global tb1_presionado
    global tb1_IZQ
    global tb1_CEN
    global tb1_DER
    global tb2_presionado
    global tb2_IZQ
    global tb2_CEN
    global tb2_DER

    #Nos suscribirmos al bumper
    rospy.Subscriber('tb4/mobile_base/events/bumper', BumperEvent, callback_bumper)
    rospy.Subscriber('tb2/mobile_base/events/bumper', BumperEvent, callback_bumper_2)

    #Creamos un nodo
    rospy.init_node('movimiento', anonymous=True)

    rate = rospy.Rate(15) # 10hz

    # Para mantener el codigo ejecutandose
    while not rospy.is_shutdown():
        if tb1_presionado == False:
            linearMove(turt1)
        else:

            if tb1_IZQ:
                for x in range(0,20): # 10it x 5hz = 10 x 1/5 = 2 segundos aprox
                    marchaAtras(-0.4, turt1)
                    rate.sleep()
                tb1_IZQ = False

            if tb1_CEN:
                #Elegimos aleatoriametne si giramos a la izquierda o a la derecha
                sentido_giro = random.randint(0, 1)
                if sentido_giro == 0:
                    giro = 0.4
                else:
                    giro = -0.4

                for x in range(0,20): # 10it x 5hz = 10 x 1/5 = 2 segundos aprox
                    marchaAtras(giro, turt1)
                    rate.sleep()
                tb1_CEN = False

            if tb1_DER:
                for x in range(0,20): # 10it x 5hz = 10 x 1/5 = 2 segundos aprox
                    marchaAtras(0.4, turt1)
                    rate.sleep()
                tb1_DER = False

            tb1_presionado = False

        if tb2_presionado == False:
            linearMove(turt2)
        else:
            if tb2_IZQ:
                for x in range(0,20): # 10it x 5hz = 10 x 1/5 = 2 segundos aprox
                    marchaAtras(-0.4, turt2)
                    rate.sleep()
                tb2_IZQ = False

            if tb2_CEN:
                #Elegimos aleatoriametne si giramos a la izquierda o a la derecha
                sentido_giro = random.randint(0, 1)
                if sentido_giro == 0:
                    giro = 0.4
                else:
                    giro = -0.4

                for x in range(0,20): # 10it x 5hz = 10 x 1/5 = 2 segundos aprox
                    marchaAtras(giro, turt2)
                    rate.sleep()
                tb2_CEN = False

            if tb2_DER:
                for x in range(0,20): # 10it x 5hz = 10 x 1/5 = 2 segundos aprox
                    marchaAtras(0.4, turt2)
                    rate.sleep()
                tb2_DER = False

            tb2_presionado = False


if __name__ == '__main__':
    try:
        mover_turtlebots()
    except rospy.ROSInterruptException:
        pass
