'''
    Team 1: Justin Fellows and Ivan Chu
    CSE 4360-001 Autonomous Robot Design and Programming
    University of Texas at Arlington
    27 October 2023
'''

import math

# Maximum values of x and y for the testing area. Assumed to be in quadrant 1
xMax = 16
yMax = 10

newXMax = 2*yMax + 2
newYMax = 2*xMax + 2

# Store the layout of the track in this array, halving the grid size
layout = [[0 for _ in range(newXMax)]  for _ in range(newYMax)]
# A note about the layout, 0 is free space, 1 is obstacle, 2 is safety padding, 3 is the start, 4 is the goal

# This dictionary will store node traversal graph
graph = {}

obstacles = [[4,1],
             [4,2],
             [4,3],
             [4,4],
             [4,5],
             [7,4],
             [7,5],
             [7,6],
             [7,7],
             [7,8],
             [7,9],
             [7,10],
             [10,3],
             [10,4],
             [10,5],
             [10,6],
             [10,7],
             [11,3],
             [12,3],
             [12,4],
             [13,3],
             [13,4]]

# 2 entries, x then y value of starting location
start = [2,2]



# 3 entries, in the following format [center(x,y), radius]
goal = [[13, 7], 1.0]

def manhattanDist(x,y):
    return abs(x[0] - y[0]) + abs(x[1] - y[1])

# Doubling the size of the grid, so i can represent half grids on the original grid
def shiftGrid():
    global layout
    global start
    global goal
    for o in obstacles:
        x = o[0] * 2
        y = o[1] * 2
        layout[x][y] = 1
        layout[x-1][y] = 1
        layout[x+1][y] = 1
        layout[x][y-1] = 1
        layout[x-1][y-1] = 1
        layout[x+1][y-1] = 1
        layout[x][y+1] = 1
        layout[x-1][y+1] = 1
        layout[x+1][y+1] = 1
    start = [start[0]*2, start[1]*2]
    goal = [[goal[0][0] * 2, goal[0][1] * 2], goal[1] * 2]
    layout[start[0]][start[1]] = 3
    layout[goal[0][0]][goal[0][1]] = 4

def padGrid():
    global layout
    # Bless me father for I have sinned, I have made if block spaghetti
    # This pads around each obstacle, including corners
    for i in range(xMax * 2):
        for j in range(yMax * 2):
            if layout[i][j] == 1:
                if i > 0:
                    if layout[i-1][j] != 1:
                        layout[i-1][j] = 2
                if i < (xMax * 2) - 1:
                    if layout[i+1][j] != 1:
                        layout[i+1][j] = 2
                if j > 0:
                    if layout[i][j-1] != 1:
                        layout[i][j-1] = 2
                if j < (yMax * 2) - 1:
                    if layout[i][j+1] != 1:
                        layout[i][j+1] = 2
                if i > 0 and j > 0:
                    if layout[i-1][j-1] != 1:
                        layout[i-1][j-1] = 2
                if i < (xMax * 2) - 1 and j < (yMax * 2) - 1:
                    if layout[i+1][j+1] != 1:
                        layout[i+1][j+1] = 2
                    if j > 0:
                        if layout[i+1][j-1] != 1:
                            layout[i+1][j-1] = 2
                    if i > 0:
                        if layout[i-1][j+1] != 1:
                            layout[i-1][j+1] = 2
    
    # This pads around the edge of the arena
    for i in range(xMax * 2):
        if layout[i][0] != 1:
            layout[i][0] = 2
        if layout[i][(yMax * 2)-1] != 1:
            layout[i][(yMax * 2)-1] = 2
    for j in range(yMax * 2):
        if layout[0][j] != 1:
            layout[0][j] = 2
        if layout[(xMax * 2)-1][j] != 1:
            layout[(xMax * 2)-1][j] = 2

def printGrid():
    for i in range(xMax * 2):
        for j in range(yMax * 2):
            print(layout[i][j], end=" ")
        print("")

# Helper function to update a dictionary without deleting what is already there
def updateNestedDict(dict1, dict2):
    for key, value in dict2.items():
        if isinstance(value, dict):
            dict1[key] = updateNestedDict(dict1.get(key, {}), value)
        else:
            dict1[key] = value
    return dict1

# Function to make a node traversal graph from the grid
def graphMake():
    global graph
    for i in range(xMax * 2):
        for j in range(yMax * 2):
            if layout[i][j] == 0 or layout[i][j] == 3 or layout[i][j] == 4:
                dictList = {}
                try:
                    # Checking the space left
                    if layout[i-1][j] == 0 or layout[i-1][j] == 3 or layout[i-1][j] == 4:
                        dictList.update({(i,j):{(i-1,j):1}})
                        graph = updateNestedDict(graph, dictList)
                        dictList.clear()
                except IndexError:
                    pass
                try:
                    # Checking the space above
                    if layout[i][j+1] == 0 or layout[i][j+1] == 3 or layout[i][j+1] == 4:
                        dictList.update({(i,j):{(i,j+1):1}})
                        graph = updateNestedDict(graph, dictList)
                        dictList.clear()
                except IndexError:
                    pass
                try:
                    # Checking the space below
                    if layout[i][j-1] == 0 or layout[i][j-1] == 3 or layout[i][j-1] == 4:
                        dictList.update({(i,j):{(i,j-1):1}})
                        graph = updateNestedDict(graph, dictList)
                        dictList.clear()
                except IndexError:
                    pass
                try:
                    # Checking the space right
                    if layout[i+1][j] == 0 or layout[i+1][j] == 3 or layout[i+1][j] == 4:
                        dictList.update({(i,j):{(i+1,j):1}})
                        graph = updateNestedDict(graph, dictList)
                        dictList.clear()
                except IndexError:
                    pass
                

def init():
    shiftGrid()
    padGrid()
    graphMake()

if __name__ == "__main__":
    init()
    printGrid()
    print(graph)
