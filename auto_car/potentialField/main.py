#!/usr/bin/env python3

import map
import movement_virtual
import math
from queue import Queue

# TODO: add startup time and warning beeps

position = map.start
# Assuming the robot starts facing the positive X axis
# angle will be stored as a unit vector
rotation = [1,0]
# Base increment to drive forward
driveStep = 0.25

#posQueue = Queue(maxsize = 6)

# Helper function for computing vector dot product
def dotProd(x,y):
    return x[0]*y[0] + x[1]*y[1]

def matrixVectorDotProd(matrix, vector):
    result = [matrix[0][0] * vector[0] + matrix[0][1] * vector[1], matrix[1][0] * vector[0] + matrix[1][1] * vector[1]]
    return result

# Does not return anything, instead it updates the rotation variable
def rotate(angle):
    global rotation
    rotMatrix = [[math.cos(angle), -1 * math.sin(angle)],[math.sin(angle), math.cos(angle)]]
    rotation = matrixVectorDotProd(rotMatrix, rotation)
    # Remake rotation vector into a unit vector
    mag = (rotation[0]**2 + rotation[1]**2)**0.5
    rotation = [rotation[0] / mag, rotation[1] / mag]

def feet_to_mm(feet):
    return feet * 304.8
map.padObstacles()
logFile = open("log.txt", "w")
logFile.close()
# 
angleList = [0, math.pi / 4, math.pi / -4, 0, math.pi / -4, math.pi / 4]
angleIndex = 0
count = 0
# equation of a circle: (x-a)^2 + (y-b)^2 = r^2
# Disk equation:  (x-a)^2 + (y-b)^2 <= r^2
# Using the inverse of the disk equation as the loop control
while (position[0] - map.goal[0][0])**2 + (position[1] - map.goal[0][1])**2 > map.goal[1]**2:
    """
    Loop logic:
    Get current field
    Rotate car to align with field
    update rotation
    Drive forward
    update position
    Repeat
    """
    logFile = open("log.txt", "a")
    field = map.totalField(position, angleList[angleIndex])
    fieldMag = ((field[0])**2 + (field[1])**2)**0.5
    unitField = [field[0] / fieldMag, field[1] / fieldMag]
    ufAngle = math.acos(dotProd([1,0], unitField))
    rotAngle = math.acos(dotProd([1,0], rotation))
    if unitField[1] < 0:
        ufAngle *= -1
    if rotation[1] < 0:
        rotAngle *= -1
    
    rotationDif = ufAngle - rotAngle
    if math.degrees(rotationDif) > 0.9 or math.degrees(rotationDif) < -0.9:
        movement_virtual.rotate_to_angle(-1 * math.degrees(rotationDif))
        rotate(rotationDif)
    driveDist = driveStep
    movement_virtual.move_forward(feet_to_mm(driveDist))

    dX = driveDist * math.cos(ufAngle)
    dY = driveDist * math.sin(ufAngle)
    position = [position[0] + dX, position[1] + dY]
    # This block determines the circumnavigation behavior
    # if the robot does not move more than double its driveStep in Queue(maxsize) moves,
    # change behavior

    if position[0] < 1 or position[0] > 15 or position[1] < 1 or position[1] > 9:
        angleIndex += 1
        if angleIndex > 5:
            angleIndex = 0
    '''
    if not posQueue.full():
        posQueue.put(position)
    else:
        prevPos = posQueue.get()
        if map.dist(prevPos, position) < 1.3 * driveStep:
            angleIndex += 1
            if angleIndex > 1:
                angleIndex = 0
            while not posQueue.empty():
                posQueue.get()
        posQueue.put(position)
    '''
    logFile.write("Position:      " + str(position) + "\n")
    logFile.write("RotDif:        " + str(math.degrees(rotationDif)) + "\n")
    logFile.write("Rotation:      " + str(rotation) + "\n")
    logFile.write("Field:         " + str(field) + "\n")
    logFile.write("Field Rotation:" + str(math.degrees(angleList[angleIndex])) + "\n")
    logFile.write("UField:        " + str(unitField) + "\n\n")
    logFile.close()
    count += 1

logFile = open("log.txt", "a")
logFile.write("Solution found after " + str(count) + " moves.\n")
logFile.close()

movement_virtual.stop_motors()
