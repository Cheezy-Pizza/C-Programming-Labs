
#include <stdio.h>  // <list of functions used>
#include <math.h>   // <list of functions used>
#include <stdlib.h> //abs() 

#define L1                    350.0
#define L2                    250.0
#define MAX_ABS_THETA1_DEG    150.0
#define MAX_ABS_THETA2_DEG    170.0
#define MAX_STRING            256
#define PI                    3.14159265358979323846
#define LEFT_ARM_SOLUTION     0
#define RIGHT_ARM_SOLUTION    1

double RadToDeg(double angRad)
{
  return (angRad*180)/PI;
}

int scaraIK(double toolX, double toolY, double* ang1, double* ang2, char arm) {
   int inRange = -1;

   double L = sqrt((toolX*toolX) + (toolY*toolY));
   double beta = atan2(toolY,toolX);
   double alpha = acos(((L2*L2)-(L*L)-L1*L1)/(-2*L*L1));

   if(arm == 'L'){
      *ang1 = beta + alpha;
   }
   else if (arm == 'R'){
      *ang1 = beta - alpha;
   }
   else{
      *ang1 = 1000; //to make it out of range cuz arm solution isnt valid
   }

   *ang2 = atan2((toolY-(L1*sin(*ang1))),(toolX - (L1 * cos(*ang1))))-(*ang1);

   *ang1 = RadToDeg(*ang1);
   *ang2 = RadToDeg(*ang2);
   //test command
   //printf("/%lf,%lf/\n", fabs(*ang1), fabs(*ang2));

   if (((fabs(*ang1)) <= MAX_ABS_THETA1_DEG) && (fabs((*ang2)) <= MAX_ABS_THETA2_DEG)){
      inRange = 0;
   }
   return inRange; 
}

void main(void){
   double x=0,y=200;
   double theta1,theta2;
   char arm = 'R';
   scaraIK(x,y,&theta1,&theta2,arm);
   printf("%.1lf,%.1lf,%.1lf,%.1lf,%c",x,y,&theta1,&theta2,arm);
}