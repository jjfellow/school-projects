#!/usr/bin/env python3

'''
    Justin Fellows and Ivan Chu
    11/26/23
    Project 2
    CSE 4360
'''

from ev3dev.ev3 import *
import time

def extinguishFunc(bgLight = 0):
    # Initialize fan motor
    fan_motor = MediumMotor('outC')

    # Initialize light sensor
    light_sensor = ColorSensor('in3')

    # Init logfile
    log = open("extLog.txt", 'w')
    log.close()

    # Read the initial state of the light sensor
    light_value = light_sensor.value()
    log = open("extLog.txt", 'a')
    log.write("init light value: " + str(light_value) + "\n")

    while light_value > bgLight: # bgLight is the measured background light of the room
        # Turn on the fan motor at full speed
        fan_motor.run_forever(speed_sp=-200)
        log.write("Motor is on\n")
        # Read the updated state of the light sensor
        light_value = light_sensor.value()
        log.write("Updated light value: " + str(light_value) + "\n")

    # Turn off the fan motor
    fan_motor.stop(stop_action='brake')
    log.write("Stopping motor\n\n\n")
    log.close()

if __name__ == "__main__":
    extinguishFunc()