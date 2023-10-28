# The arena is a 16x10 ft rectangle
# x is length, y is height
# All units are in feet

# input the center of the obstacles as well as the vertices in this 2d array
# for rectangles, order thusly: [center, top left, bottom right]
import math


obstacles = [[(4,1), (3.4,2),(4.6,0)],
             [(4,2), (3.4,3),(4.6,1)],
             [(4,3), (3.4,4),(4.6,2)],
             [(4,4), (3.4,4),(4.6,3)],
             [(4,5), (3.4,6),(4.6,4)],
             [(7,4), (6.4,5),(7.6,3)],
             [(7,5), (6.4,6),(7.6,4)],
             [(7,6), (6.4,7),(7.6,5)],
             [(7,7), (6.4,8),(7.6,6)],
             [(7,8), (6.4,9),(7.6,7)],
             [(7,9), (6.4,10),(7.6,8)],
             [(7,10),(6.4,11),(7.6,9)],
             [(10,3),(9.4,4),(10.6,2)],
             [(10,4),(9.4,5),(10.6,3)],
             [(10,5),(9.4,6),(10.6,4)],
             [(10,6),(9.4,7),(10.6,5)],
             [(10,7),(9.4,8),(10.6,6)],
             [(11,3),(10.4,4),(11.6,2)],
             [(12,3),(11.4,4),(12.6,2)],
             [(12,4),(11.4,5),(12.6,3)],
             [(13,3),(12.4,4),(13.6,2)],
             [(13,4),(12.4,5),(13.6,3)]]

#obstacles = [[(8,8), (9,9),(7,7)]]
#obstacles = [[(3, 4.6), (2.8, 5), (3.2, 4.2)]]
#obstacles = [[(3,3),(2,4),(4,2)]]
# Potential value at an obstacle, make large
obsPotential = 150
# TODO: Professor's suggestion: circumnavigation potential, instead of potential field coming straight out
# comes out at an angle so it shimmies along an obstacle. However, if it gets too close to a wall,
# the direction needs to change
# 3 entries, in the following format [center(x,y), radius]
goal = [(13.0, 7.0), 1.0]

#potential at the goal, make smaller than obstacle potential
goalPotential = 5000

# 2 entries, x then y value of starting location
start = (2.0,2.0)

# Safety margin around each obstacle, in feet, followed by a touple version used when padding around obstacles
margin = 0.443
topMargin = (-1*margin, margin)
bottomMargin = (margin, -1*margin)

# Maximum values of x and y for the testing area. Assumed to be in quadrant 1
xMax = 16.0
yMax = 10.0

# Helper function to calculate euclidean distance between two points
# Added to help with portability
def dist(x,y):
    return ((x[0]-y[0])**2 + (x[1]-y[1])**2)**0.5

# Helper function for taking the dot product of a matrix and a vector
def dotProd(matrix, vector):
    result = [matrix[0][0] * vector[0] + matrix[0][1] * vector[1], matrix[1][0] * vector[0] + matrix[1][1] * vector[1]]
    return result


# Function for adding the safety margin to the obstacles
def padObstacles():
    for i in obstacles:
        i[1] = tuple(map(lambda i, j: i+j, i[1], topMargin))
        i[2] = tuple(map(lambda i, j: i+j, i[2], bottomMargin))



# Calculates the field of the supplied obstacle at the given position
# Returns a vector
def getField(position, obs):
    #Need a line from current position to nearest point on supplied obstacle
    #Can find this line by using point-slope form from the center of the robot
    #to the center of the obstacle
    try:
        slope = (position[1] - obs[0][1]) / (position[0] - obs[0][0])
    except ZeroDivisionError:
        if position[1] > obs[1][1]:
            # This block executes if robot is positioned above the obstacle
            distance = position[1] - obs[1][1]
            fieldVector = [0 , obsPotential / (distance**2)]
            return fieldVector
        else:
            # This block executes if robot is positioned below the obstacle
            distance = obs[1][1] - position[1]
            fieldVector = [0 , (-1*obsPotential) / (distance**2)]
            return fieldVector
    else:
        intercept = position[1] - slope * position[0]
        # y=mx+b, x=(y-b)/m
        xvals = [obs[1][0], obs[2][0]]
        yvals = [obs[1][1], obs[2][1]]
        ygivenx = [slope * xvals[0] + intercept, slope * xvals[1] + intercept]
        if slope == 0:
            xgiveny = [xvals[0], xvals[1]]
        else:
            xgiveny = [(yvals[0] - intercept) / slope, (yvals[1] - intercept) / slope]

    if xgiveny[0] < obs[1][0] or xgiveny[0] > obs[2][0]:
        # Block for when first x value is not within the rectangle
        intersections = [[xvals[0],ygivenx[0]],[xvals[1],ygivenx[1]]]
    else:
        intersections = [[xgiveny[0],yvals[0]],[xgiveny[1],yvals[1]]]
    # Now that the intersections are known, find the distance from the current position to each, then calculate field vector from the closest intersection
    interDist = [dist(intersections[0], position), dist(intersections[1], position)]
    if interDist[0] > interDist[1]:
        closest = intersections[1]
        distance = dist(closest, position)
        # Create a normal vector pointing from the obstacle to the robot
        distVector = [(position[0] - closest[0]) / distance,(position[1] - closest[1]) / distance]
    else:
        closest = intersections[0]
        distance = dist(closest, position)
        # Create a normal vector pointing from the obstacle to the robot
        distVector = [(position[0] - closest[0]) / distance,(position[1] - closest[1]) / distance]
    
    distanceFactor = obsPotential / distance**2
    fieldVector = [distanceFactor * distVector[0], distanceFactor * distVector[1]]
    return fieldVector
# Can request the obstacle fields be rotated by a specified amount in radians, it defaults to none
def totalField(position, rotation = 0):
    #Loop over the obstacles and call getField for each, and sum the results
    result = [0,0]

    if rotation != 0:
        rotMatrix = [[math.cos(rotation), -1 * math.sin(rotation)],
                     [math.sin(rotation), math.cos(rotation)]]
    else:
        rotMatrix = [[1,0],
                     [0,1]]
    for i in obstacles:
        fieldInstance = getField(position, i)
        fieldInstance = dotProd(rotMatrix, fieldInstance)
        result = [result[0] + fieldInstance[0], result[1] + fieldInstance[1]]
    
    # Handling potential from the goal. Deciding to do this as a linear inverse instead of a quadratic inverse like the obstacles CHANGED constant field now
    goalDist = dist(position, goal[0])
    goalVector = [(goal[0][0] - position[0]) / goalDist, (goal[0][1] - position[1]) / goalDist]
    goalField = [(goalVector[0] * goalPotential), (goalVector[1] * goalPotential)]
    result = [result[0] + goalField[0], result[1] + goalField[1]]

    # Handling potential from the walls of the arena
    # Use the points along the boundries plus or minus the padding value
    posY = yMax - margin
    posYDist = posY - position[1]
    # The distance from where the walls will begin influencing the field of the robot
    influenceDist = 2.0
    # Vector coming from the positive y direction
    if posYDist < influenceDist:
        posYVector = [0, (-1 * obsPotential) / posYDist**2]
    else:
        posYVector = [0,0]
    negY = 0.0 + margin
    negYDist = position[1] - negY
    # Vector coming from the negative y direction
    if negYDist < influenceDist:
        negYVector = [0, obsPotential / negYDist**2]
    else:
        negYVector = [0,0]
    posX = xMax - margin
    posXDist = posX - position[0]
    # Vector coming from the postive x direction
    if posXDist < influenceDist:
        posXVector = [(-1 * obsPotential) / posXDist**2, 0]
    else:
        posXVector = [0,0]
    negX = 0 + margin
    negXDist = position[0] - posX
    # Vector coming from the negative x direction
    if negXDist < influenceDist:
        negXVector = [(1 * obsPotential) / negXDist**2, 0]
    else:
        negXVector = [0,0]
    posXVector = dotProd(rotMatrix, posXVector)
    negXVector = dotProd(rotMatrix, negXVector)
    posYVector = dotProd(rotMatrix, posYVector)
    negYVector = dotProd(rotMatrix, negYVector)
    wallVector = [posXVector[0] + negXVector[0], posYVector[1] + negYVector[1]]
    result = [result[0] + wallVector[0], result[1] + wallVector[1]]

    return result

#print("Field at start is: " + str(totalField(start)) + "\n")