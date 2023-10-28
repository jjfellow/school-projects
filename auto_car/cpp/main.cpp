#include"control.h"
#include"mymap.h"
#include"mypath.h"
#include<math.h>
int main()
{
  InitEV3();
  ResetRotationCount(OUT_ABCD);
  Point nLocation;
  int *x_pos = (int*)malloc(sizeof(int));
  int *y_pos = (int*)malloc(sizeof(int));
  int *deg_L = (int*)malloc(sizeof(int));
  int *deg_R = (int*)malloc(sizeof(int));
  int *clock_offset = (int*)malloc(sizeof(int));
  int *prev_err = (int*)malloc(sizeof(int));
  int CurrentLocation[2] = {0,0};
  int NextLocation[2] = {0,0};
  *x_pos = 0;
  *y_pos = 0;
  *deg_L = 0;
  *deg_R = 0;
  *clock_offset = 0;
  *prev_err = 0;
  initializeMap();
  InitializeQueue();
  UpdateMap();
  CreatePath();
  ContractedPath();
  CURRENT_ORIENTATION = 0;
  //starting pos
  contracted.pop();
  CurrentLocation[0] = start_pos[0];
  CurrentLocation[1] = start_pos[1];
  while(!contracted.empty())
  {
    Point nextLoc = contracted.top();
    NextLocation[0] = nextLoc.Col;
    NextLocation[1] = nextLoc.Row;
    contracted.pop();
    ///////////////////////////////////////////////
    MoveToNext(CurrentLocation, NextLocation, deg_L, deg_R, clock_offset);
    /////////////////////////////////////////////////////////////
    CurrentLocation[0] = NextLocation[0];
    CurrentLocation[1] = NextLocation[1];
  }
free(x_pos);
free(y_pos);
free(deg_L);
free(deg_R);
free(prev_err);
FreeEV3();
return(0);
}
