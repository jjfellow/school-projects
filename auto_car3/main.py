#!/usr/bin/env python3
'''
    Justin Fellows and Ivan Chu
    12/14/23
    Project 3
    CSE 4360
'''
from ev3dev.ev3 import *
from time import sleep

# Connect sensors and motors
left_motor = LargeMotor('outB')
right_motor = LargeMotor('outC')
yaw_gyro = GyroSensor('in1')



# Function to compensate for deviation
def compensate_gyro_angle(gyro_angle, desired_angle, speed):
    deviation = desired_angle - gyro_angle
    if deviation > 1:
        turn_left(speed)
    elif deviation < -1:
        turn_right(speed)

# Function to drive forward
def drive_forward(speed):
    left_motor.run_direct(duty_cycle_sp=speed)
    right_motor.run_direct(duty_cycle_sp=speed)

# Function to turn left
def turn_left(speed):
    left_motor.run_direct(duty_cycle_sp=speed + 20)
    right_motor.run_direct(duty_cycle_sp=speed)

# Function to turn right
def turn_right(speed):
    left_motor.run_direct(duty_cycle_sp=speed)
    right_motor.run_direct(duty_cycle_sp=speed + 20)

# Initialize the gyro sensors
yaw_gyro.mode = 'GYRO-RATE'
yaw_gyro.mode = 'GYRO-ANG'

# Start the robot
speed = -100
desired_angle = 0
drive_forward(speed)
while left_motor.position < 9000 and right_motor.position < 9000:
    gyro_angle = yaw_gyro.value()
    compensate_gyro_angle(gyro_angle, desired_angle, speed)

left_motor.stop(stop_action='brake')
right_motor.stop(stop_action='brake')