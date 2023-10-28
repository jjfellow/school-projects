#include<iostream>
#include<math.h>
#include"float.h"
#include"mymap.h"
int enviorment[ROW][COL];
double distance[ROW][COL];
int goal_pos[2];
int start_pos[2];
void initializeMap()
{
  for(int rowIndex=0;rowIndex<ROW;rowIndex++)
  {
    for(int colIndex=0;colIndex<COL;colIndex++)
    {
      enviorment[rowIndex][colIndex] = 0;
      distance[rowIndex][colIndex] = DBL_MAX;
    }
  }
  for(int obsRowindex = 0;obsRowindex<MAX_OBSTACLES;obsRowindex++)
  {
    if(obstacle[obsRowindex][0] >=0 && obstacle[obsRowindex][1] >=0)
    {
      int cIndex = int(round(obstacle[obsRowindex][0]/log_tile_width));
      int rIndex = int(round(obstacle[obsRowindex][1]/log_tile_width));
      //printf("obstacle position (%d,%d) \n",cIndex,rIndex);
      for(int erindex = rIndex-OBSTACLE_DISTANCE;erindex<=rIndex+OBSTACLE_DISTANCE;erindex++)
      {
        for(int ecindex = cIndex-OBSTACLE_DISTANCE; ecindex<=cIndex+OBSTACLE_DISTANCE;ecindex++)
        {
          enviorment[erindex][ecindex] = -1;
        }
      }
      goal_pos[0] = int(round(goal[0]/log_tile_width));
      goal_pos[1] = int(round(goal[1]/log_tile_width));
      // printf("goal position (%d,%d) \n",goal_pos[1],goal_pos[0]);
      enviorment[goal_pos[1]][goal_pos[0]] = 25;
      start_pos[0] = int(round(start[0]/log_tile_width));
      start_pos[1] = int(round(start[1]/log_tile_width));
      // printf("start position (%d,%d) \n",start_pos[1],start_pos[0]);
      enviorment[start_pos[1]][start_pos[0]] = 15;
      //printf("obstacle 1 position (%d,%d) \n",rIndex,cIndex);
      distance[start_pos[1]][start_pos[0]] = 0;
    }
  }
}
void printenviorment()
{
  for(int rowIndex=0;rowIndex<ROW;rowIndex++)
  {
    for(int colIndex=0;colIndex<COL;colIndex++)
    {
      printf("%3d",enviorment[rowIndex][colIndex]);
    }
    std::cout << std::endl;
  }
}
void printdistance()
{
  for(int rowIndex=0;rowIndex<ROW;rowIndex++)
  {
    for(int colIndex=0;colIndex<COL;colIndex++)
    {
      if(distance[rowIndex][colIndex] == DBL_MAX)
      {
        printf("%3d",-1);
      }
      else
      printf("%3.f",distance[rowIndex][colIndex]);
    }
    std::cout << std::endl;
  }
}
void ClearQueue()
{
  while(!queueFrontier.empty())
  {
    queueFrontier.pop();
  }
}
void InitializeQueue()
{
  ClearQueue();
  Point current;
  current.Row = start_pos[1];
  current.Col = start_pos[0];
  queueFrontier.push(current);
}
void updateQueue(int cRow,int cCol)
{
  // printf("Inside update queue\n ;");
  Point new_point;
  new_point.Row = cRow;
  new_point.Col = cCol;
  queueFrontier.push(new_point);
}
void UpdateMap()
{
  int count = 1;
  while(!queueFrontier.empty())
  {
    count++;
    Point current = queueFrontier.front();
    if(current.Row == goal_pos[1] && current.Col == goal_pos[0])
    {
      pathFlag = true;
      break;
    }
    else
    {
      // printf("explored node (%d,%d) \n",current.Row,current.Col);
      queueFrontier.pop();
      //Move in negative x
      if(current.Row > 0)
      {
        if(enviorment[current.Row-1][current.Col] != -1 && enviorment[current.Row-1][current.Col] !=2)
        {
          double mindistance = distance[current.Row][current.Col] + 1;
          if(distance[current.Row-1][current.Col] > mindistance)
          {
            distance[current.Row-1][current.Col] = mindistance;
            updateQueue(current.Row-1,current.Col);
          }
        }
      }
      //Move in positive x
      if(current.Row < ROW-1)
      {
        if(enviorment[current.Row+1][current.Col] != -1 && enviorment[current.Row+1][current.Col] !=2)
        {
          double mindistance = distance[current.Row][current.Col] + 1;
          if(distance[current.Row+1][current.Col] > mindistance)
          {
            distance[current.Row+1][current.Col] = mindistance;
            updateQueue(current.Row+1,current.Col);
          }
        }
      }
      //Move in Negative y
      if(current.Col > 0)
      {
        if(enviorment[current.Row][current.Col-1] != -1 && enviorment[current.Row][current.Col-1] !=2)
        {
          double mindistance = distance[current.Row][current.Col] + 1;
          if(distance[current.Row][current.Col -1] > mindistance)
          {
            distance[current.Row][current.Col-1] = mindistance;
            updateQueue(current.Row,current.Col-1);
          }
        }
      }
      //Move in Positive y
      if(current.Col < COL-1)
      {
        if(enviorment[current.Row][current.Col+1] != -1 && enviorment[current.Row][current.Col+1] !=2)
        {
          double mindistance = distance[current.Row][current.Col] + 1;
          if(distance[current.Row][current.Col +1] > mindistance)
          {
            distance[current.Row][current.Col+1] = mindistance;
            updateQueue(current.Row,current.Col+1);
          }
        }
      }
      //mark map as visited
      enviorment[current.Row][current.Col] = 2;
    }
  }
}
void updatepath(int cRow,int cCol)
{
  Point new_point;
  new_point.Row = cRow;
  new_point.Col = cCol;
  Robot_path.push(new_point);
}

struct Point GetPath()
{
  Point RoboPath = Robot_path.top();
  Robot_path.pop();
  return RoboPath;
}
void CreatePath()
{
  if(pathFlag)
  {
    updatepath( goal_pos[1],goal_pos[0] );
    Point cur_point;
    cur_point.Row = goal_pos[1];
    cur_point.Col = goal_pos[0];
    while(true)
    {
      if(cur_point.Row == start_pos[1] && cur_point.Col == start_pos[0])
      {
        //updatepath(cur_point.Row,cur_point.Col);
        break;
      }
      if(cur_point.Row >0)
      {
        if(distance[cur_point.Row][cur_point.Col]>distance[cur_point.Row-1][cur_point.Col])
        {
          updatepath(cur_point.Row-1,cur_point.Col);
          cur_point.Row = cur_point.Row-1;
          continue;
        }
      }
      if(cur_point.Row <ROW-1)
      {
        if(distance[cur_point.Row][cur_point.Col]>distance[cur_point.Row+1][cur_point.Col])
        {
          updatepath(cur_point.Row+1,cur_point.Col);
          cur_point.Row = cur_point.Row+1;
          continue;
        }
      }
      if(cur_point.Col <COL-1)
      {
        if(distance[cur_point.Row][cur_point.Col]>distance[cur_point.Row][cur_point.Col+1])
        {
          updatepath(cur_point.Row,cur_point.Col+1);
          cur_point.Col = cur_point.Col+1;
          continue;
        }
      }
      if(cur_point.Col > 0)
      {
        if(distance[cur_point.Row][cur_point.Col]>distance[cur_point.Row][cur_point.Col-1])
        {
          updatepath(cur_point.Row,cur_point.Col-1);
          cur_point.Col = cur_point.Col-1;
          continue;
        }
      }
    }
  }
  else
  {
    // message on screen path not available
    LcdPrintf(2,"Path Now found" );
    FreeEV3();
  }
}
//quan
bool straightReduction(Point first, Point second, Point third)
{
  if (first.Row == second.Row && third.Row == second.Row)
  {
    return true;
  }
  else if (first.Col == second.Col && third.Col == second.Col)
  {
    return true;
  }
  else
  {
    return false;
  }
}

void ContractedPath()
{
  std::stack<Point> temp;
  if(Robot_path.size()>=3)
  {
    Point first = Robot_path.top();
    Robot_path.pop();
    Point second = Robot_path.top();
    Robot_path.pop();
    Point third = Robot_path.top();
    Robot_path.pop();
    if(straightReduction(first, second, third))
    {
      temp.push(first);
      temp.push(third);
    }
    else
    {
      temp.push(first);
      temp.push(second);
      temp.push(third);
    }
    while(!Robot_path.empty())
    {
      second = temp.top();
      temp.pop();
      first = temp.top();
      temp.pop();
      third = Robot_path.top();
      Robot_path.pop();
      if(straightReduction(first, second, third))
      {
        temp.push(first);
        temp.push(third);
      }
      else
      {
        temp.push(first);
        temp.push(second);
        temp.push(third);
      }
    }
    while(!temp.empty())
    {
      contracted.push(temp.top());
      temp.pop();
    }
  }
  else
  {
    while(!Robot_path.empty())
    {
      Point p1 = Robot_path.top();
      contracted.push(p1);
    }
  }
}
