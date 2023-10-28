#ifndef MYMAP_H
#define MYMAP_H
#include<queue>
#include<stack>
#define COL 16*2 +1 //16 *2 +1 for box our X value
#define ROW 10*2 +1 // our Y value
#define MAX_OBSTACLES 25 /* maximum number of obstacles */
#define TILE_WIDTH 0.305
#define OBSTACLE_DISTANCE 2
int num_obstacles = 13; /* number of obstacles */
struct Point{
  int Row = 0;
  int Col = 0;
};

double obstacle[MAX_OBSTACLES][2] = /* obstacle locations */
{{0.61, 2.743},{0.915, 2.743},{1.219, 2.743},{1.829, 1.219},
{1.829, 1.524},{ 1.829, 1.829}, {1.829, 2.134},{2.743, 0.305},
{2.743, 0.61},{2.743, 0.915},{2.743, 2.743},{3.048, 2.743},
{3.353, 2.743},
{4*.305,6*.305},{-1,-1},{-1,-1},{-1,-1},{-1,-1},{-1,-1},{-1,-1},{-1,-1},{-1,-1},
{-1,-1},{-1,-1},{-1,-1}};
double start[2] = {0.305, 1.219}; /* start location */
double goal[2] = {3.658, 1.829}; /* goal location */
//double goal[2] = {0.305*0, 0.305*9 }; /* goal location */
double log_tile_width = TILE_WIDTH/2.0;
std::queue<Point> queueFrontier;
std::stack<Point> Robot_path;
std::stack<Point> contracted;
bool pathFlag = false;
#endif
