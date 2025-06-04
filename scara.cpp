/*|ROBT 1270: SCARA |----------------------------------------------------------
#
# Project: ROBT 1270 - SCARA Simulator Intermediate Control
# Program: scara.cpp
#
# Description:
#   This program contains code for drawing lines with the SCARA.
#
# Author: Peter Stogneff
# Date Created: 04/09/2025 (My Birthday!! ^-^)
# Last Modified: 04/09/2025
# -----------------------------------------------------------------------------*/

/*|Includes|-------------------------------------------------------------------*/
#include "scara.h"

/*|Global Variables|-----------------------------------------------------------*/
CRobot robot;     // the global robot Class instance.  Can be used everywhere
                  // robot.Initialize()
                  // robot.Send()
                  // robot.Close()

/*|Function Definitions|-------------------------------------------------------*/
/****************************************************************************************
* Function: scaraDisplayState
*
* Description:
*	Displays the SCARA information. Feel free to modify.
*
* Inputs:
*	scaraState  - Contains SCARA information
*
* Returns: void
*
* Last Modified: May 1, 2021 by Isaiah Regacho
*****************************************************************************************/
void scaraDisplayState(SCARA_ROBOT scaraState) {
	SCARA_POS arm = scaraState.armPos;
	SCARA_TOOL tool = scaraState.toolPos;

	printf("|SCARA STATE|\n");

	// Display Position
	printf("| Theta 1 | Theta 2 |    X    |    Y    |   Arm   |\n");
	printf("|%9.2lf|%9.2lf|%9.2lf|%9.2lf|    %d    |\n", arm.theta1, arm.theta2, arm.x, arm.y, arm.armSol);

	// Display Tool
	printf("|Position |   RED   |  GREEN  |   BLUE  |\n");
	printf("|    %c    |   %3d   |   %3d   |   %3d   |\n", tool.penPos, tool.penColor.r, tool.penColor.g, tool.penColor.b);
}

/****************************************************************************************
* Function: moveScaraL
*
* Description:
*	Moves the Scara Arm Linearly based on line data sent
*
* Inputs:
*	scaraState  - Contains SCARA information
*   line - contains line / color information
*
* Returns: nothing yet
*
* Last Modified: April 14, 2025 by Peter Stogneff
*****************************************************************************************/
int moveScaraL(SCARA_ROBOT *scaraState, LINE_DATA line){
    // Memory allocation for variable sized array of SCARA_ROBOT
    SCARA_ROBOT *robline = (SCARA_ROBOT*) malloc(sizeof(SCARA_ROBOT)*(line.numPts+1));//+1 to allocate one more position: The beginning
	if(robline == NULL){
		return 0;
	}
	
	setColor(line.color.r, line.color.g, line.color.b);

	for(int i = 0;i<=line.numPts;i++){ //iterates between beginning (0) and ending (total points)
		robline[i] = *scaraState; //set current iteration to the base values
		//set x and y positions of the struct
		robline[i].armPos.x = ((line.xB-line.xA)/100)*((float)100/line.numPts * i)+line.xA;
		robline[i].armPos.y = ((line.yB-line.yA)/100)*((float)100/line.numPts * i)+line.yA;
		//printf("Positions: %lf, %lf\n", robline[i].armPos.x, robline[i].armPos.y);
		//set the angles in the struct, and also check if its in range
		int inRange = scaraIK(robline[i].armPos.x, 
						  	  robline[i].armPos.y, 
						  	  &robline[i].armPos.theta1, 
						  	  &robline[i].armPos.theta2, 
						  	  robline[i].armPos.armSol);
		//  \/ TEST CODE \/
		//printf("Angles: %lf, %lf\n", robline[i].armPos.theta1, robline[i].armPos.theta2);
		//printf("ARM: %d, %d\n", inRange, robline[i].armPos.armSol);
		if (inRange == -1){ //out of range (-1)
			scaraSetState(robline[i]);	//then set tool 
			if (robline[i].armPos.armSol == LEFT_ARM_SOLUTION){ //if arm started in left solution
				inRange = scaraIK(robline[i].armPos.x, 
									  robline[i].armPos.y, 
									  &robline[i].armPos.theta1, 
									  &robline[i].armPos.theta2, 
									  RIGHT_ARM_SOLUTION); //checks other arm solution
				scaraState->armPos.armSol = RIGHT_ARM_SOLUTION; //updates pointer struct to reflect arm change decision
				if(inRange == 0 && i != 0){ //if other arm solution finds valid angles and its not on first line position

					inRange = scaraIK(robline[i-1].armPos.x,
									  robline[i-1].armPos.y, 
									  &robline[i-1].armPos.theta1, 
									  &robline[i-1].armPos.theta2, 
									  RIGHT_ARM_SOLUTION); //recalc angles, but for prev iteration
					rotate(robline[i-1].armPos.theta1,robline[i-1].armPos.theta2); //move to other solution, same position as prev iteration
					robline[i].toolPos.penPos = 'D'; //pen down
					scaraSetState(robline[i]); //send pen down command
				}
			}
				
			else { //if arm started in right position
				inRange = scaraIK(robline[i].armPos.x, 
									  robline[i].armPos.y, 
									  &robline[i].armPos.theta1, 
									  &robline[i].armPos.theta2, 
									  LEFT_ARM_SOLUTION); //checks other arm solution
				scaraState->armPos.armSol = LEFT_ARM_SOLUTION; //updates pointer struct to reflect arm change decision
				if(inRange == 0 && i != 0){ //if other arm solution finds valid angles and its not on first line position

					inRange = scaraIK(robline[i-1].armPos.x, 
									  robline[i-1].armPos.y, 
									  &robline[i-1].armPos.theta1, 
									  &robline[i-1].armPos.theta2, 
									  LEFT_ARM_SOLUTION); //recalc angles, but for prev iteration
					rotate(robline[i-1].armPos.theta1,robline[i-1].armPos.theta2); //move to other solution, same position as prev iteration
					robline[i].toolPos.penPos = 'D'; //pen down
					scaraSetState(robline[i]); //send pen command
				}
			}
		}
		if (inRange == 0){ //if its in range, move the angles
			if(i !=0){
				int inRangePrev = scaraIK(robline[i-1].armPos.x, 
										  robline[i-1].armPos.y, 
										  &robline[i-1].armPos.theta1, 
										  &robline[i-1].armPos.theta2, 
										  scaraState->armPos.armSol);
				if(inRangePrev == 0){ //pen down if its not first move, and if last move was not out of range
					robline[i].toolPos.penPos = 'D'; //first move gets it into right position
					//color+=50; Some failed color stuff XD
					//robline[i].toolPos.penColor = {color%255,0,color%255};
				}
			}
			scaraSetState(robline[i]); //send current state to robot
			rotate(robline[i].armPos.theta1,robline[i].armPos.theta2);
		}

	}
	    // Free up memory allocated.
	free(robline);
	return 1;
}

/****************************************************************************************
* Function: moveScaraJ
*
* Description:
*	Moves the Scara Sim joints. As simple as it gets.
*
* Inputs:
*	scaraState  - Contains SCARA information
*
* Returns: nothing yet
*
* Last Modified: April 11, 2025 by Peter Stogneff
*****************************************************************************************/
int moveScaraJ(SCARA_ROBOT *scaraState){
	int inRange = scaraIK(scaraState->armPos.x, 
						  scaraState->armPos.y, 
						  &scaraState->armPos.theta1, 
						  &scaraState->armPos.theta2, 
						  scaraState->armPos.armSol);
	if (inRange == 0){	
		scaraSetState(*scaraState);	
		rotate(scaraState->armPos.theta1,scaraState->armPos.theta2);
	}
	else if(inRange != 0){
		inRange = scaraIK(scaraState->armPos.x, 
						  scaraState->armPos.y, 
						  &scaraState->armPos.theta1, 
						  &scaraState->armPos.theta2, 
						  !scaraState->armPos.armSol);
	}
	if (inRange == 0){	
		scaraSetState(*scaraState);	
		rotate(scaraState->armPos.theta1,scaraState->armPos.theta2);
	}
	return inRange;
}

/****************************************************************************************
* Function: initLine
*
* Description:
*	Initializes the line segment struct based on inputted information
*
* Inputs:
*	xA - starting X value
*	xB - ending X value
*	yA - starting Y value
*	yB - ending Y value
*	numPts - number of intermediate points on the line
*
* Returns: LINE_DATA struct type. All of the line data
*
* Last Modified: April 13, 2025 by Peter Stogneff
*****************************************************************************************/
LINE_DATA initLine(double xA, double yA, double xB, double yB, int numPts){
	LINE_DATA lineData;
	lineData.xA = xA;
	lineData.xB = xB;
	lineData.yA = yA;
	lineData.yB = yB;
	lineData.numPts = numPts;
	lineData.color.r = 0;
	lineData.color.g = 0;
	lineData.color.b = 0;
	double slope = ((yB-yA)/(xB-xA));
	printf("Slope: %d\n",slope);
	printf("inf slope: %lf\n",(xB-xA));
	if ((abs((xB-xA)) < SLOPE_TOL));
	else if (slope < 0) lineData.color.r = 255;
	else if (slope== 0) lineData.color.g = 255;
	else if (slope > 0) lineData.color.b = 255;
	return lineData;
}  

/****************************************************************************************
* Function: initScaraState
*
* Description:
*	puts all inputted SCARA information into a struct
*
* Inputs:
*	x - X-coordinate of the SCARA arm
*	y - Y-coordinate of the SCARA arm
*	armSol - left/right arm solution for the IK
*	penState - up/down for the pen
*	mtrSpeed - H/M/L motor speed for the arm movements
*
* Returns: SCARA_ROBOT struct type. Contains all SCARA arm info
*
* Last Modified: April 11, 2025 by Peter Stogneff
*****************************************************************************************/
SCARA_ROBOT initScaraState(double x, double y, int armSol, SCARA_TOOL penState, char mtrSpeed){
	SCARA_ROBOT scaraState;
	scaraState.armPos.x = x;
	scaraState.armPos.y = y;
	scaraState.armPos.armSol = armSol;
	scaraState.toolPos = penState;
	scaraState.motorSpeed = mtrSpeed;
	return scaraState;
}

/****************************************************************************************
* Function: scaraSetState
*
* Description:
*	sends all auxillary commands to the SCARA robot arm from the struct info
*
* Inputs:
*	scaraState  - Contains SCARA information
*
* Returns: void
*
* Last Modified: April 13, 2025 by Peter Stogneff
*****************************************************************************************/
int scaraSetState(SCARA_ROBOT scaraState){
	static SCARA_ROBOT scaraPrevState;
	int complete = 0;
	if(scaraPrevState.motorSpeed != scaraState.motorSpeed){
		complete = setSpeed(scaraState.motorSpeed);
	}
	if(scaraPrevState.toolPos.penPos != scaraState.toolPos.penPos){
		setPen(scaraState.toolPos.penPos);
	}
	if(scaraPrevState.toolPos.penColor.r != scaraState.toolPos.penColor.r ||
	   scaraPrevState.toolPos.penColor.g != scaraState.toolPos.penColor.g ||
	   scaraPrevState.toolPos.penColor.b != scaraState.toolPos.penColor.b ){
		setColor(scaraState.toolPos.penColor.r, 
				 scaraState.toolPos.penColor.g, 
				 scaraState.toolPos.penColor.b);
	}
	scaraPrevState = scaraState;
	return complete;
}

/****************************************************************************************
* Function: rotate
*
* Description:
*	rotates the SCARA arm
*
* Inputs:
*	ang1 - first angle of SCARA arm
*	ang2 - second angle of SCARA arm - smaller arm angle
*
* Returns: void
*
* Last Modified: April 7, 2025 by Peter Stogneff
*****************************************************************************************/
void rotate(double ang1, double ang2){
	char VAR[MAX_STRING];
	//if arm is within bounds, sends robot command to rotate to angles
	if (((fabs(ang1)) <= MAX_ABS_THETA1_DEG) && ((fabs(ang2)) <= MAX_ABS_THETA2_DEG)){
		sprintf(VAR, "ROTATE_JOINT ANG1 %lf ANG2 %lf\n", ang1, ang2);
        robot.Send(VAR);
	}
}

/****************************************************************************************
* Function: setColor 
*
* Description:
*	sets the pen color of the SCARA arm
*
* Inputs:
*	r - red color value of pen (0-255)
*	g - green color value of pen (0-255)
*	b - blue color value of pen (0-255)
*
* Returns: void
*
* Last Modified: April 7, 2025 by Peter Stogneff
*****************************************************************************************/
void setColor(int r, int g, int b){
	char VAR[MAX_STRING];
	sprintf(VAR,"PEN_COLOR %d %d %d\n", r, g, b);
	robot.Send(VAR);
}

/****************************************************************************************
* Function: setPen
*
* Description:
*	sets SCARA pen to either up or down
*
* Inputs:
*	pen - U or D for up or down
*
* Returns: void
*
* Last Modified: April 7, 2025 by Peter Stogneff
*****************************************************************************************/
void setPen(char pen){
	if (pen == 'U' || pen == 'u'){
		robot.Send("PEN_UP\n");
	 }
	 else if (pen == 'D' || pen == 'd'){
		robot.Send("PEN_DOWN\n");
	 }
	 else{
		printf("That's not right...\n");
	 }
}

/****************************************************************************************
* Function: setSpeed
*
* Description:
*	sets the speed the SCARA arm will move at
*
* Inputs:
*	speed - H/M/L High, Medium, or Low speeds
*
* Returns: void
*
* Last Modified: April 7, 2025 by Peter Stogneff
*****************************************************************************************/
int setSpeed(char speed){
	//  - MOTOR_SPEED HIGH/MEDIUM/LOW
	if (speed == 'H' || speed == 'h'){
		robot.Send("MOTOR_SPEED HIGH\n");
	}
	else if (speed == 'M' || speed == 'm'){
		robot.Send("MOTOR_SPEED MEDIUM\n");
	}
	else if (speed == 'L' || speed == 'l'){
		robot.Send("MOTOR_SPEED LOW\n");
	}
	else{
		//printf("silly boy, you did it wrong\n");
		return 0;
	}
	return 1;
}

/****************************************************************************************
* Function: scaraFK
*
* Description:
* This function will calculate the x,y coordinates given two joint angles.
*
* Inputs:
* ang1 - Angle of joint 1 in degrees.
* ang2 - Angle of joint 2 in degrees.
* toolX - The tool position along the x-axis. Pointer
* toolY - The tool position along the y-axis. Pointer
*
* Returns:
* inRange - (0) in range, (-1) out of range
*
*****************************************************************************************/
int scaraFK(double ang1, double ang2, double* toolX, double* toolY) {
	double Rang1 = (ang1*180)/PI;
	double Rang2 = (ang2*180)/PI;
	int inRange = -1;
	*toolX = L1 * cos(Rang1) + L2 * cos(Rang1 + Rang2);
	*toolY = L1 * sin (Rang1) + L2 * cos(Rang1 + Rang2);
	if (((fabs(ang1)) <= MAX_ABS_THETA1_DEG) && ((fabs(ang2)) <= MAX_ABS_THETA2_DEG)){
	   inRange = 0;
	}
	return inRange; 
 }
 
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
 * Last Modified: April 14, 2025 by Peter Stogneff (fixed first angle bounds)
 *****************************************************************************************/
 int scaraIK(double toolX, double toolY, double* ang1, double* ang2, int arm) {
	int inRange = -1;
 
	double L = sqrt((toolX*toolX) + (toolY*toolY));
	double beta = atan2(toolY,toolX);
	double alpha = acos(((L2*L2)-(L*L)-L1*L1)/(-2*L*L1));
 
	if(arm == LEFT_ARM_SOLUTION){
	   *ang1 = beta + alpha;
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