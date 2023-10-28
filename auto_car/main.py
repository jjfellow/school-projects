#!/usr/bin/env python3

'''
    Team 1: Justin Fellows and Ivan Chu
    CSE 4360-001 Autonomous Robot Design and Programming
    University of Texas at Arlington
    27 October 2023
'''

import aStar
import  movement
from time import sleep

# This function call chain calls every other py file's init functions and returns the path to the goal
path, position = aStar.getPath()
goal = aStar.getGoal()


rotation = ["North", "South", "East", "West"]
rotIndex = 0


logFile = open("log.txt", "w")
logFile.close()

while position[0] != goal[0][0] or position[1] != goal[0][1]:
    # Logging current status
    logFile = open("log.txt", "a")
    logFile.write("Position: " + str(position) + "\n")
    logFile.write("Rotation: " + rotation[rotIndex] + "\n\n")
    logFile.close()
    
    # Path is East
    if path[0][0] > position[0]:
        if rotIndex == 0:
            movement.turn_right(90)
            rotIndex = 2
        elif rotIndex == 1:
            movement.turn_left(90)
            rotIndex = 2
        elif rotIndex == 3:
            movement.turn_left(180)
            rotIndex = 2
        movement.move_forward(0.5)
    # Path is West
    elif path[0][0] < position[0]:
        if rotIndex == 0:
            movement.turn_left(90)
            rotIndex = 3
        elif rotIndex == 1:
            movement.turn_right(90)
            rotIndex = 3
        elif rotIndex == 2:
            movement.turn_right(180)
            rotIndex = 3
        movement.move_forward(0.5)
    # Path is North
    elif path[0][1] > position[1]:
        if rotIndex == 1:
            movement.turn_right(180)
            rotIndex = 0
        elif rotIndex == 2:
            movement.turn_left(90)
            rotIndex = 0
        elif rotIndex == 3:
            movement.turn_right(90)
            rotIndex = 0
        movement.move_forward(0.5)
        # Path is South
    if path[0][1] < position[1]:
        if rotIndex == 0:
            movement.turn_left(180)
            rotIndex = 1
        elif rotIndex == 2:
            movement.turn_right(90)
            rotIndex = 1
        elif rotIndex == 3:
            movement.turn_left(90)
            rotIndex = 1
        movement.move_forward(0.5)
    position = path[0]
    path.pop(0)
