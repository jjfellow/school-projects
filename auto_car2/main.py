#!/usr/bin/env python3
'''
    Justin Fellows and Ivan Chu
    11/26/23
    Project 2
    CSE 4360
'''
from wander import wanderFunc
from wallFollow import wallFollowFunc
from extinguish import extinguishFunc
from fireFind import fireFindFunc
import time
from ev3dev.ev3 import *

foundGoal = False
foundWall = False
leftMotor = Motor('outB')
rightMotor = Motor('outA')
fanMotor = Motor('outC')

light_sensor = ColorSensor('in3')
light_sensor.MODE_COL_AMBIENT
bgLightValue = light_sensor.value()
startTime = time.process_time()
# After how many seconds should the robot search for the candle
searchInterval = 20
foundGoal = fireFindFunc(bgLight = bgLightValue)

# First stage for finding the goal
while not foundGoal:
    # Behavior Priority, highest to lowest
    # fireFind, wallFollow, wander
    # if a fire has been found, ignore wallFollow and wander
    # if a wall has been found, ignore wander
    # extinguish is only called after fire has been found
    # Check for the candle every seachInterval seconds
    if time.process_time() > (startTime + searchInterval) and not foundGoal:
        foundGoal = fireFindFunc(bgLight = bgLightValue)
        startTime = time.process_time()
    
    if foundGoal:
        # exit first stage while loop
        break
    if not foundWall:
        foundWall = wanderFunc()
    else:
        foundGoal = wallFollowFunc(bgLight = bgLightValue)

# This part of the code is executed after the goal has been found, and the robot should be pointed at it
leftMotor.stop(stop_action='brake')
rightMotor.stop(stop_action='brake')
lightValue = light_sensor.value()
nearCandleVal = 26 # Light sensor's reading when near the fire, update in the field

# Drive straight at the candle
leftMotor.run_forever(speed_sp = 300)
rightMotor.run_forever(speed_sp = 300)
startTime = time.process_time()
while lightValue < nearCandleVal:
    lightValue = light_sensor.value()
    if time.process_time() > (startTime + 1.6) and lightValue <= bgLightValue + 4:
        break



leftMotor.stop(stop_action='brake')
rightMotor.stop(stop_action='brake')
extinguishFunc(bgLight = bgLightValue)
