#include <stdio.h>  // <list of functions used>
#include <math.h>   // <list of functions used>
//#include "scara.h"  // <list of functions and structures used> // Your code should be inside here.
#include <stdlib.h> // abs() 

#define PI                    3.14159265358979323846
#define NUM_LINES             8           // number of lines for the main loop
#define MAX_POINTS            50          // maximum number of points in a line
#define MAX_STRING            256         // for the commandString array size
#define LEFT_ARM_SOLUTION     1           // index that can be used to indicate left arm
#define RIGHT_ARM_SOLUTION    0           // index that can be used to indicate right arm
#define L1                    350.0       // inner arm length
#define L2                    250.0       // outer arm length
#define MAX_ABS_THETA1_DEG    150.0       // max angle of inner arm
#define MAX_ABS_THETA2_DEG    170.0       // max angle of outer arm relative to inner arm
#define SLOPE_TOL             (1.0e-5)    // for pen color based on slope
#define POINT_TOL             (1.0e-8)    // use to check if previous line end point is
                                          // the same as current line start point
                                          
/****************************************************************************************
 * Function: scaraIK
 *
 * Description:
 * This function will calculate two joint angles given the x,y coordinates.
 *
 * Inputs:
 * toolX - The tool position along the x-axis.
 * toolY - The tool position along the y-axis.
 * ang1 - Angle of joint 1 in degrees. Pointer
 * ang2 - Angle of joint 2 in degrees. Pointer
 * arm - Selects which solution to try.
 *
 * Returns:
 * inRange - (0) in range, (-1) out of range
 *
 *****************************************************************************************/
 int scaraIK(double toolX, double toolY, double* ang1, double* ang2, int arm) {
	int inRange = -1;
 
	double L = sqrt((toolX*toolX) + (toolY*toolY));
	double beta = atan2(toolY,toolX);
	printf("%lf\n", beta);
	double alpha = acos(((L2*L2)-(L*L)-L1*L1)/(-2*L*L1));
	if(arm == LEFT_ARM_SOLUTION){
	   *ang1 = beta + alpha;
	   printf("%lf\n",*ang1);
	}
	else if (arm == RIGHT_ARM_SOLUTION){
	   *ang1 = beta - alpha;
	   
	}
	else{
	   *ang1 = 1000; //to make it out of range cuz arm solution isnt valid
	}

	if(*ang1 < -PI){
		*ang1+= 2*PI;
	 }
	 else if (*ang1 > PI){
		*ang1-= 2*PI;
	 }

	*ang2 = atan2((toolY-(L1*sin(*ang1))),(toolX - (L1 * cos(*ang1))))-(*ang1);
 
	if(*ang2 < -PI){
		*ang2+= 2*PI;
	 }
	 else if (*ang2 > PI){
		*ang2-= 2*PI;
	 }
	 
	*ang1 = (*ang1*180)/PI;
	*ang2 = (*ang2*180)/PI;
	//test command
	//printf("/%lf,%lf/\n", fabs(*ang1), fabs(*ang2));
 
	if (((fabs(*ang1)) <= MAX_ABS_THETA1_DEG) && (fabs((*ang2)) <= MAX_ABS_THETA2_DEG)){
	   inRange = 0;
	}
	return inRange; 
 }
 
 int main(){
	double x = -420,
		   y = 20,
		   i,k;
	int arm = LEFT_ARM_SOLUTION;
	scaraIK(x,y,&i,&k,arm);
	printf("%lf,%lf,%lf,%lf,%d",x,y,i,k,arm);
 }