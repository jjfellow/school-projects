'''
    Team 1: Justin Fellows and Ivan Chu
    CSE 4360-001 Autonomous Robot Design and Programming
    University of Texas at Arlington
    27 October 2023
'''

#!/usr/bin/env python3
from time import sleep
from ev3dev2.sensor.lego import GyroSensor
from ev3dev2.motor import * 

import math




# Turn right
def turn_right(angle, speed=10):
    left_motor = LargeMotor('outB')
    right_motor = LargeMotor('outA')
    gyro = GyroSensor()
    current_angle = gyro.angle
    target_angle = current_angle + angle
    print("targ angle: " + str(target_angle))

    left_motor.on(speed)
    right_motor.on(-speed)
    while gyro.angle < target_angle - 15:
        pass
    left_motor.off()
    right_motor.off()
    

# Turn left
def turn_left(angle, speed=10):
    left_motor = LargeMotor('outB')
    right_motor = LargeMotor('outA')
    gyro = GyroSensor()
    current_angle = gyro.angle
    target_angle = current_angle - angle
    print("targ angle: " + str(target_angle))
    left_motor.on(-speed)
    right_motor.on(speed)
    while gyro.angle > target_angle - 5:
        pass    
    
    left_motor.off()
    right_motor.off()

#Move Forward
def move_forward(distance_feet, speed = 200): 
    left_motor = LargeMotor('outB')
    right_motor = LargeMotor('outA')
    # distance in inches 
    distance_inches = distance_feet * 12

    # distance in millimeters
    distance_mm = distance_inches * 25.4

    C = 56 * math.pi # Circumference in mm
    rotations = (distance_mm * 360) / C

    offsetCoef = 1.0125
    # Move forward for a certain number of rotation
    right_motor.run_to_rel_pos(position_sp=rotations, speed_sp=speed, stop_action = "brake")
    left_motor.run_to_rel_pos(position_sp=rotations * offsetCoef, speed_sp=speed * offsetCoef, stop_action = "brake")
    
    sleep(3.5 * distance_feet)
    right_motor.off()
    left_motor.off()
    

# Testing / example commands
if __name__ =="__main__":
    #move_forward(2)
    #sleep(8)
    for i in range(10):
        move_forward(1)
    