from time import sleep
from ev3dev.auto import *

# Initialize motors
left_motor = LargeMotor('outA')
right_motor = LargeMotor('outD')

# Initialize gyro sensor
gyro = GyroSensor('in4')

C = 176.9 # Circumference in mm

# Move Forward
def move_forward(distance): # distance in mm
    rotations = (distance * 360) / C

    # Move forward for a certain number of rotation
    #print("Beginning to move forward\n")
    left_motor.run_to_rel_pos(position_sp=rotations, speed_sp=200)
    right_motor.run_to_rel_pos(position_sp=rotations, speed_sp=200)
    #print("Stopping!\n")
    sleep(1.8)

# Move Backward
def move_backward(distance): # distance in mm
   
    rotations = (distance * 360) / C

    # Move backward for a certain number of rotation
    #print("Beginning to move backward\n")
    left_motor.run_to_rel_pos(position_sp=-rotations, speed_sp=-200)
    right_motor.run_to_rel_pos(position_sp=-rotations, speed_sp=-200)
    #print("Stopping!")
    sleep(2)

# Rotate to a given angle
def rotate_to_angle(angle):
    # Calibrate the gyro sensor
    gyro.mode = 'GYRO-ANG'
    while gyro.angle is None:
        sleep(0.01)

    # Reset the angle
    gyro.mode = 'GYRO-RATE'

    # If the given angle is negative, reverse the motor directions
    if angle < 0:
        left_motor.run_forever(speed_sp=-150)
        right_motor.run_forever(speed_sp=150)
    else:
        left_motor.run_forever(speed_sp=150)
        right_motor.run_forever(speed_sp=-150)

    # Continue until the gyro sensor hits the given angle
    while True:
        sleep(0.001)
        print("Target Angle: " + str(angle) + " Gyro Angle: " + str(gyro.angle) + "\n")
        if (angle >= 0 and gyro.angle >= angle) or (angle < 0 and gyro.angle < angle):
            break

    # Stop the motors
    left_motor.stop(stop_action='brake')
    right_motor.stop(stop_action='brake')

# Stop the motor
def stop_motors():
    # Stop motors
    left_motor.stop(stop_action='brake')
    right_motor.stop(stop_action='brake')


'''
# Testing / example commands

move_forward(200)
sleep(5)
move_backward(200)

#stop_motors()

#rotate_to_angle(90)
'''
#rotate_to_angle(-90)