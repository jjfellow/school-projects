'''
    Justin Fellows and Ivan Chu
    11/26/23
    Project 2
    CSE 4360
'''
from ev3dev.ev3 import *

def wanderFunc():
    # initialize touch sensors
    touch_sensor_front = TouchSensor('in1')
    touch_sensor_right = TouchSensor('in4')
    leftMotor = Motor('outB')
    rightMotor = Motor('outA')
    foundWall = False
    
    # Begin by driving forward
    leftMotor.run_forever(speed_sp = 300)
    rightMotor.run_forever(speed_sp = 300)

    if touch_sensor_front.is_pressed or touch_sensor_right.is_pressed:
        foundWall = True
    return foundWall

