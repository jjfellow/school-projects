#include "control.h"
int Clock_Rotation_Compensate=3.7;
int CURRENT_ORIENTATION;
int p_err = 0;
double rotate_compen = 0.4;
//...
int get_err(int length,int speed, int* deg_L, int* deg_R,int *clock_offset){
  int err = 0;
  int diff = 0;
  ResetRotationCount(OUT_ABCD);
  long c = 125.66; // circumference of wheel in mm
  double motor_rot = (length * 360)/c;
  //RotateMotor(OUT_BC,speed,motor_rot);
  while(MotorRotationCount(OUT_B)<motor_rot)
  {
    if(!ButtonIsUp(BTNUP)){
      FreeEV3();
    }
    OnFwdSync(OUT_BC, speed);
    diff = MotorRotationCount(OUT_B)-MotorRotationCount(OUT_C);
    err += diff;
  }
  *clock_offset -= 0.01;
  update_deg(deg_L, deg_R);
  return err;
}
int err_correction(int prev_err){ // change speed on the B wheel
  int current_err = prev_err + MotorRotationCount(OUT_B)-MotorRotationCount(OUT_C);
  if(current_err < -12000){
    prev_err = 0;
    return 2;
  }else if(current_err > 12000){
    prev_err = 0;
    return -2;
  }else{
    prev_err = current_err;
    return 0;
  }
}
//...
void accelerate(int speed){
  int i;
for(i = 0; i <= 5 && i < speed; i++){
  OnFwdReg(OUT_BC, i);
  if(i != 5)
  Wait(750);
}
for(i = 5; i <= speed; i+=2){
  OnFwdReg(OUT_BC, i);
  if(i != speed)
  Wait(500);
}
}
void decelerate(int speed){
  int i;// = MotorPower(OUT_BC) - '0';
  //LcdPrintf(1,"%s\n", MotorPower(OUT_BC));
  //LcdPrintf(1,"%s\n", MotorPower(OUT_BC));
  for(i = speed; i >= 0; i-=2){
    OnFwdSync(OUT_BC, i);
    Wait(1000);
  }
}
//.305 size of one side of the tile
// =================================================================
// DO NOT TOUCH this function -- Rabinson
void update_deg(int* deg_L, int* deg_R){
  *deg_L += MotorRotationCount(OUT_B);
  *deg_R += MotorRotationCount(OUT_C);
}
void turnanticlockwise(int theta, int speed, int* deg_L, int* deg_R,int *clock_offset){
  ResetRotationCount(OUT_ABCD);
  //speed = 4; // change this to turn at different speed
  long R = 133/2; // in mm
  long c = 125.66; // circumference of wheel in mm
  double motor_rot = (R * theta * M_PI * 360)/ (180 * c);
  motor_rot = motor_rot*4;
  int rot = motor_rot;
  //int rot = motor_rot/1 - 3; // after the minus .... adjusting turn
  //int rot = motor_rot;
  while(MotorRotationCount(OUT_B) < rot){
    OnFwdReg(OUT_B, speed);
    OnRevReg(OUT_C, speed);
    if(!ButtonIsUp(BTNUP)){
      FreeEV3();
    }
  }
  //*clock_offset +=Clock_Rotation_Compensate;
  //Clock_Rotation_Compensate -= 0.01;
  update_deg(deg_L, deg_R);
}

void turnclockwise(int theta, int speed, int* deg_L, int* deg_R,int *clock_offset){
  ResetRotationCount(OUT_ABCD);
  long R = 122/2; // in mm
  long c = 175.9; // circumference of wheel in mm
  double motor_rot =( R * theta * M_PI * 360)/ (180 * c);
  motor_rot = motor_rot*5;
  rotate_compen +=rotate_compen;
  int rot = motor_rot+ 3;
  while(-1*MotorRotationCount(OUT_B) < rot){
    OnFwdReg(OUT_C, speed);
    OnRevReg(OUT_B, speed);
    if(!ButtonIsUp(BTNUP)){
      FreeEV3();
    }
  }
  *clock_offset +=0;
  update_deg(deg_L, deg_R);
}

void move_backward(int length,int speed, int* deg_L, int* deg_R,int *clock_offset)
{
  //char motor[10];
  ResetRotationCount(OUT_ABCD);
  //long R = 122.9/2; // in mm
  long c = 125.66; // circumference of wheel in mm
  double motor_rot = (length * 360)/c;
  //RotateMotor(OUT_BC,speed,motor_rot);
  motor_rot = motor_rot*5.0;
  //sprintf(motor,"%lf\n",motor_rot);
  //LcdPrintf(2,motor );
  while(MotorRotationCount(OUT_B)<motor_rot)
  {
    if(!ButtonIsUp(BTNUP)){
      FreeEV3();
    }
    OnFwdSync(OUT_BC, speed);
  }
  *clock_offset -= 0.0001;
  update_deg(deg_L, deg_R);
}

void move_forward(int length,int speed, int* deg_L, int* deg_R,int *clock_offset)
{
  //char motor[10];
  char motor_count[10];
  ResetRotationCount(OUT_ABCD);
  long R = 122.9/2; // in mm
  long c = 125.66; // circumference of wheel in mm
  double motor_rot = (length * 360)/c;
  motor_rot = motor_rot*5.0;
  //sprintf(motor,"%lf\n",motor_rot);
  //LcdPrintf(2,motor );
  while(-1*MotorRotationCount(OUT_C)<motor_rot)
  {
    //sprintf(motor_count,"%d\n",MotorRotationCount(OUT_C));
    //LcdPrintf(2,motor_count );
    if(!ButtonIsUp(BTNUP)){
      FreeEV3();
    }
    OnRevSync(OUT_BC, speed);
  }
  *clock_offset += 0.001;
  update_deg(deg_L, deg_R);
}
void Stop_Motor()
{
  Off(OUT_BC);
}

void MoveToNext(int current[2], int next[2], int* deg_L, int* deg_R,int *clock_offset)
{
  int rotation_speed = 15;
  int move_speed = 20;
  int theta = 90;
  if(next[0] > current[0]) //east
  {
    for(int i = 0; i < (4-CURRENT_ORIENTATION)%4; i++)
    {
      turnclockwise(theta+*clock_offset, rotation_speed, deg_L, deg_R,clock_offset);
      Stop_Motor();
    }
    CURRENT_ORIENTATION = 0;
  }
  else if(next[1] < current[1]) //south
  {
    for(int i = 0; i < (5-CURRENT_ORIENTATION)%4; i++)
    {
      turnclockwise(theta+*clock_offset, rotation_speed, deg_L, deg_R,clock_offset);
      Stop_Motor();
    }
    CURRENT_ORIENTATION = 1;
  }
  else if(next[0] < current[0]) //west
  {
    for(int i = 0; i < (6-CURRENT_ORIENTATION)%4; i++)
    {
      turnclockwise(theta+*clock_offset, rotation_speed, deg_L, deg_R,clock_offset);
      Stop_Motor();
    }
    CURRENT_ORIENTATION = 2;
  }
  else //north
  {
    for(int i = 0; i < (7-CURRENT_ORIENTATION)%4; i++)
    {
      turnclockwise(theta+*clock_offset, rotation_speed, deg_L, deg_R,clock_offset);
      Stop_Motor();
    }
    CURRENT_ORIENTATION = 3;
  }
  int dist = abs(next[0] - current[0]) + abs(next[1] - current[1]);
  move_forward(305*dist/2,move_speed, deg_L, deg_R,clock_offset);
  Stop_Motor();
}
void path(int* deg_L, int* deg_R,int *clock_offset)
{
  int rotation_speed = 4;
  int move_speed = 4;
  int theta = 90;
  //...
  move_forward(305*2,move_speed, deg_L, deg_R,clock_offset);
  turnclockwise(theta+*clock_offset, rotation_speed, deg_L, deg_R,clock_offset);
  //...
  move_forward(305*2,move_speed, deg_L, deg_R,clock_offset);
  turnclockwise(theta+*clock_offset, rotation_speed, deg_L, deg_R,clock_offset);
  move_forward(305*2,move_speed, deg_L, deg_R,clock_offset);
  turnclockwise(theta+*clock_offset, rotation_speed, deg_L, deg_R,clock_offset);
  turnclockwise(theta+*clock_offset, rotation_speed, deg_L, deg_R,clock_offset);
  turnclockwise(theta+*clock_offset, rotation_speed, deg_L, deg_R,clock_offset);
  move_forward(305*1,move_speed, deg_L, deg_R,clock_offset);
  turnclockwise(theta+*clock_offset, rotation_speed, deg_L, deg_R,clock_offset);
  turnclockwise(theta+*clock_offset, rotation_speed, deg_L, deg_R,clock_offset);
  turnclockwise(theta+*clock_offset, rotation_speed, deg_L, deg_R,clock_offset);
  move_forward(305*1,move_speed, deg_L, deg_R,clock_offset);
  turnclockwise(theta+*clock_offset, rotation_speed, deg_L, deg_R,clock_offset);
  move_forward(305*3,move_speed, deg_L, deg_R,clock_offset);
  turnclockwise(theta+*clock_offset, rotation_speed, deg_L, deg_R,clock_offset);
  move_forward(305*4,move_speed, deg_L, deg_R,clock_offset);
  turnclockwise(theta+*clock_offset, rotation_speed, deg_L, deg_R,clock_offset);
  turnclockwise(theta+*clock_offset, rotation_speed, deg_L, deg_R,clock_offset);
  turnclockwise(theta+*clock_offset, rotation_speed, deg_L, deg_R,clock_offset);
  move_forward(305*3,move_speed, deg_L, deg_R,clock_offset);
  turnclockwise(theta+*clock_offset, rotation_speed, deg_L, deg_R,clock_offset);
  move_forward(305*3,move_speed, deg_L, deg_R,clock_offset);
  turnclockwise(theta+*clock_offset, rotation_speed, deg_L, deg_R,clock_offset);
  turnclockwise(theta+*clock_offset, rotation_speed, deg_L, deg_R,clock_offset);
  turnclockwise(theta+*clock_offset, rotation_speed, deg_L, deg_R,clock_offset);
  move_forward(305*1,move_speed, deg_L, deg_R,clock_offset);
  turnclockwise(theta, rotation_speed, deg_L, deg_R,clock_offset);
  turnclockwise(theta+*clock_offset, rotation_speed, deg_L, deg_R,clock_offset);
  turnclockwise(theta+*clock_offset, rotation_speed, deg_L, deg_R,clock_offset);
  turnclockwise(theta+*clock_offset, rotation_speed, deg_L, deg_R,clock_offset);
  move_forward(305*6,move_speed, deg_L, deg_R,clock_offset);
  turnclockwise(theta+*clock_offset, rotation_speed, deg_L, deg_R,clock_offset);
  turnclockwise(theta+*clock_offset, rotation_speed, deg_L, deg_R,clock_offset);
  turnclockwise(theta+*clock_offset, rotation_speed, deg_L, deg_R,clock_offset);
}
