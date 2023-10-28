#ifndef CONTROL_H
#define CONTROL_H
#include <ev3.h>
#include <string>
#include <math.h>
#include <stdio.h>
#include <sstream>
#include <stdlib.h>
extern int CURRENT_ORIENTATION;
extern int Clock_Rotation_Compensate;
extern double rotate_compen;
void accelerate(int speed);
void decelerate(int speed);
void update_deg(int* deg_L, int* deg_R);
void turnclockwise(int theta, int speed, int* deg_L, int* deg_R,int *clock_offset);
void turnanticlockwise(int theta, int speed, int* deg_L, int* deg_R,int *clock_offset);
void move_forward(int length,int speed, int* deg_L, int* deg_R,int *clock_offset);
void move_backward(int length,int speed, int* deg_L, int* deg_R,int *clock_offset);
void Stop_Motor();
void path(int* deg_L, int* deg_R,int *clock_offset);
void MoveToNext(int current[2], int next[2], int* deg_L, int* deg_R,int *clock_offset);
void PathTravel(int CurrentLocation[],int NextLocation[],int *clock_offset);
// ...
int get_err(int length,int speed, int* deg_L, int* deg_R,int *clock_offset);
int err_correction(int* prev_err);
#endif
