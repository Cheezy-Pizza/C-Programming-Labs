/*|ROBT 1270: SCARA Console| --------------------------------------------------
#
# Project: ROBT 1270 - SCARA Simulator Advanced Control
# Program: scaraConsole.cpp
#
# Description:
#   This program contains the code for the Scara Command Console.
#
# Author: <Your Name>
# Date Created: <Day Started>
# Last Modified: <Today's Date>
# -----------------------------------------------------------------------------*/

/*|Includes|-------------------------------------------------------------------*/
#include "scaraConsole.h"

/*|Global Variables|-----------------------------------------------------------*/
extern CRobot robot;     // the global robot Class instance.  Can be used everywhere

CMD scaraCommands[MAX_CMD] = {
    {"moveScaraJ", 2},
    {"moveScaraL", 5},
    {"scaraPenUp", 0},
    {"scaraPenDown", 0},
    {"scaraSpeed", 1},
    {"scaraPenColor", 3},
    {"quit", 0},
    {"rotate", 2}
};

/*|Function Definitions|-------------------------------------------------------*/
SCARA_CONSOLE initScaraConsole(void){
    // Variable Declarations
    SCARA_CONSOLE con;

    // Customize Output
	system("COLOR 0A");
	system("CLS");

	// Initialize SCARA Simulator V3
	if(!robot.Initialize()) exit(0);
	robot.Send("PEN_UP\n");
	robot.Send("HOME\n");
	robot.Send("CLEAR_TRACE\n");
	robot.Send("CLEAR_POSITION_LOG\n");
	robot.Send("CLEAR_REMOTE_COMMAND_LOG\n");

    // Default Position for Scara
    con.scaraRobot = initScaraState(600, 0, LEFT_ARM_SOLUTION,{'u',{255, 0, 0}},'H');
    scaraSetState(con.scaraRobot);

    return con;
}

void readScaraConsole(SCARA_CONSOLE *con){
    printf("\n>>>");
    gets_s(con->userInput, MAX_SCARA_STRING);
}


void executeScaraCommand(SCARA_CONSOLE* con){
    LINE_DATA line;
    switch(con->cmdInd){
        case MOVE_SCARA_J:
            printf("moveScaraJ, Correct Command!");
            con->scaraRobot.armPos.x = atof(con->args[0]);
            con->scaraRobot.armPos.y = atof(con->args[1]);
            moveScaraJ(&con->scaraRobot);
            break;
        case MOVE_SCARA_L:
            printf("moveScaraL, Correct Command!");
            line.xA = atof(con->args[0]);
            line.yA = atof(con->args[1]);
            line.xB = atof(con->args[2]);
            line.yB = atof(con->args[3]);
            line.numPts = atof(con->args[4]);
            moveScaraL(&con->scaraRobot, line);
            break;
        case SCARA_PEN_UP:
            printf("scaraPenUp, Correct Command!");
            con->scaraRobot.toolPos.penPos = 'U';
            scaraSetState(con->scaraRobot);
            break;
        case SCARA_PEN_DOWN:
            printf("scaraPenDown, Correct Command!");
            con->scaraRobot.toolPos.penPos = 'D';
            scaraSetState(con->scaraRobot);
            break;
        case SCARA_SPEED:
            con->scaraRobot.motorSpeed = *(con->args[0]);
            if(!scaraSetState(con->scaraRobot)){
                printf("Error: invalid input, should be H, M, or S");
            }
            else printf("scaraSpeed, Correct Command!");
            break;
        case SCARA_PEN_COLOR:
            printf("scaraPenColor, Correct Command!");
            con->scaraRobot.toolPos.penColor.r = atoi(con->args[0]);
            con->scaraRobot.toolPos.penColor.g = atoi(con->args[1]);
            con->scaraRobot.toolPos.penColor.b = atoi(con->args[2]);
            scaraSetState(con->scaraRobot);
            break;
        case QUIT:
            printf("Quitting...");
            break;
        case ROTATE:
            con->scaraRobot.armPos.theta1 = atof(con->args[0]);
            con->scaraRobot.armPos.theta2 = atof(con->args[1]);
            if (((fabs(con->scaraRobot.armPos.theta1)) <= MAX_ABS_THETA1_DEG) && ((fabs(con->scaraRobot.armPos.theta2)) <= MAX_ABS_THETA2_DEG)){
                rotate(con->scaraRobot.armPos.theta1, con->scaraRobot.armPos.theta2);
                printf("rotate, Correct Command!");
            }
            else {
                printf("Error: angles out of range");
            }
            break;            
        default:
            printf("You shouldn't be here");
            break;
    }

}


int parseScaraCommand(SCARA_CONSOLE* con){
    con->command = strtok(con->userInput,":;, ");
    con->nArgs = 0;
    for (int i = 0; i <= MAX_ARGS; i++){
        con->args[i] = strtok(NULL, ":;, ");
        if (con->args[i]){
            con->nArgs++;
            //some tests
            //printf("Arg: %s ", con->args[i]);
            //printf("Arg No. %d\n",con->nArgs);
        }
    }   
    return con->nArgs;
}

int validateScaraCommand(SCARA_CONSOLE *con){
    //check name
    con->cmdInd = 100; //set filter number, to see if it's reset

    for(int i = 0; i < MAX_CMD; i++){
        if(!strcmp(con->command, scaraCommands[i].name)){
           con->cmdInd = i; 
        }
    }

    if (con->cmdInd == 100){
        printf("No Matching Scara Command. Example commands:\
        \n- moveScaraJ x, y\
        \n- moveScaraL x1, y1, x2, y2, n\
        \n- scaraPenUp\
        \n- scaraPenDown\
        \n- scaraSpeed s\
        \n- scaraPenColor r, g, b\
        \n- rotate theta1, theta2\
        \n- quit");
        return 0;
    }

    //check number of arguments
    if (con->nArgs < scaraCommands[con->cmdInd].nArgs){
        printf("Error: not enough arguments, should be %d arguments", scaraCommands[con->cmdInd].nArgs);
        return 0;
    }
    else if (con->nArgs > scaraCommands[con->cmdInd].nArgs){
        printf("Error: too many arguments, should be %d arguments", scaraCommands[con->cmdInd].nArgs);
        return 0;
    }

    //check float strtof (check if arguments are correct: either num or char)
    char* nullStr;
    if(con->cmdInd == MOVE_SCARA_J || 
       con->cmdInd == MOVE_SCARA_L || 
       con->cmdInd == SCARA_PEN_COLOR || 
       con->cmdInd == ROTATE){
        for (int i = 0; i < con->nArgs; i++){
            strtof(con->args[0], &nullStr);
            if(strlen(nullStr) != 0){
                printf("Error: at least one invalid argument, should be a float or integer");
                return 0;
            }
        }
    }

    /* unused. The execute command tests scaraSpeed directly using scara.cpp setSpeed
    char* charInput;
    if(con->cmdInd == SCARA_SPEED){
        for (int i = 0; i < con->nArgs; i++){
            strtof(con->args[i], &nullStr);
            if(strlen(nullStr) != 1){
                printf("Error: at least one invalid argument, should be a single character");
                return 0;
            }
        }
    }*/

    //if all checks pass
    return 1;
}