#!/usr/bin/env python3
'''
    Team 1: Justin Fellows and Ivan Chu
    CSE 4360-001 Autonomous Robot Design and Programming
    University of Texas at Arlington
    27 October 2023
'''

import heapq
import map

def astar_search(graph, start, goal):
    frontier = []
    heapq.heappush(frontier, (0, start))
    came_from = {}
    cost_so_far = {}
    came_from[start] = None
    cost_so_far[start] = 0

    while frontier:
        _, current = heapq.heappop(frontier)

        if current == goal:
            break

        for next in graph[current]:
            new_cost = cost_so_far[current] + graph[current][next]
            if next not in cost_so_far or new_cost < cost_so_far[next]:
                cost_so_far[next] = new_cost
                priority = new_cost + manhattan_heuristic(goal, next)
                heapq.heappush(frontier, (priority, next))
                came_from[next] = current

    return came_from, cost_so_far

#Heuristic is manhattan distance
def manhattan_heuristic(a, b):
    (x1, y1) = a
    (x2, y2) = b
    return abs(x1 - x2) + abs(y1 - y2)

def reconstruct_path(came_from, start, goal):
    current = goal
    path = []
    while current != start:
        path.append(current)
        current = came_from[current]
    #path.append(start) 
    path.reverse()     
    return path

def getPath():
    map.init()
    came_from, cost_so_far = astar_search(map.graph, tuple(map.start), tuple(map.goal[0]))
    path = reconstruct_path(came_from, tuple(map.start), tuple(map.goal[0]))
    return path, map.start

def getGoal():
    return map.goal

if __name__ == "__main__":
    map.init()
    graph = map.graph
    came_from, cost_so_far = astar_search(graph, tuple(map.start), tuple(map.goal[0]))
    path = reconstruct_path(came_from, tuple(map.start), tuple(map.goal[0]))
    print(path)
'''
graph = {
    (0, 0): {(0, 1): 1, (1, 0): 1},
    (0, 1): {(0, 2): 1, (1, 1): 1},
    (1, 0): {(1, 1): 1
'''


