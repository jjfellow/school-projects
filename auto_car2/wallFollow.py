#!/usr/bin/env python3
'''
    Justin Fellows and Ivan Chu
    11/26/23
    Project 2
    CSE 4360
'''

import time
from ev3dev.ev3 import *
def wallFollowFunc(bgLight = 1):
    # initialize touch sensors and motors
    touch_sensor_left = TouchSensor('in4')
    touch_sensor_front = TouchSensor('in1')
    lightSensor = ColorSensor('in3')
    leftMotor = LargeMotor('outB')
    rightMotor = LargeMotor('outA')
    foundGoal = False

    if lightSensor.value() > (bgLight + 6):
        foundGoal = True
        leftMotor.stop(stop_action='brake')
        rightMotor.stop(stop_action='brake')
        return foundGoal

    if touch_sensor_left.is_pressed and touch_sensor_front.is_pressed:
        #Car is in a corner, turn right
        leftMotor.run_forever(speed_sp = -100)
        rightMotor.run_forever(speed_sp = -100)
        time.sleep(0.2)
        startTime = time.process_time()
        leftMotor.run_forever(speed_sp = 300)
        rightMotor.run_forever(speed_sp = -300)
        while time.process_time() < (startTime + 0.97) and not touch_sensor_left.is_pressed:
            pass
    elif touch_sensor_left.is_pressed and not touch_sensor_front.is_pressed:
        #Car is driving along the wall, drive forward
        leftMotor.run_forever(speed_sp = 299)
        rightMotor.run_forever(speed_sp = 300)
    elif touch_sensor_front.is_pressed and not touch_sensor_left.is_pressed:
        #Car is at a wall, turn right to follow it
        leftMotor.run_forever(speed_sp = -100)
        rightMotor.run_forever(speed_sp = -100)
        time.sleep(0.2)
        startTime = time.process_time()
        leftMotor.run_forever(speed_sp = 300)
        rightMotor.run_forever(speed_sp = -300)
        while time.process_time() < (startTime + 0.97) and not touch_sensor_left.is_pressed:
            pass
    else:
        #Car is not on a wall, spiral left to find one
        leftMotor.run_forever(speed_sp = 150)
        rightMotor.run_forever(speed_sp = 300)
    return foundGoal

# Testing function, only runs when run as the main program
if __name__ == "__main__":
    wallFollowFunc(bgLight = 5)