#!/usr/bin/env python3
'''
    Justin Fellows and Ivan Chu
    11/26/23
    Project 2
    CSE 4360
'''

from ev3dev.ev3 import *
import time

def fireFindFunc(bgLight = 0):
    # Initialize light sensor
    light_sensor = ColorSensor('in3')
    light_sensor.MODE_COL_AMBIENT
    leftMotor = Motor('outB')
    rightMotor = Motor('outA')
    foundFire = False

    # Turn in a complete circle, looking for the candle
    startTime = time.process_time()
    curTime = startTime
    leftMotor.run_forever(speed_sp = -300)
    rightMotor.run_forever(speed_sp = 300)

    while curTime < (startTime + 2.6):
        if light_sensor.value() > (bgLight + 6):
            foundFire = True
            break
        curTime = time.process_time()
    leftMotor.stop(stop_action='brake')
    rightMotor.stop(stop_action='brake')
    return foundFire

if __name__ == "__main__":
    fireFindFunc(bgLight = 1)