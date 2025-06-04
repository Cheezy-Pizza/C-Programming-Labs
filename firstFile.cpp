/*|Name of the Project|
#
# Project: Name of the Project (NOP)
# Program: comments.cpp
#
# Description:
#   This file is used as a template and should be a description of the
#   code's functionality
#
# Author: <Name>
# Date Created: <Date>
# Last Modified: <Today>
*/

/*|Includes|*/
#include <stdio.h>      // printf - used for console interaction

/*|Macros|*/
#define MAX_STEPS 100   // define an arbitrary macro for the sake of example

/*|Main|*/
int main(){

    //variables
    int c = 100;
    int a = 3;
    //main thing that happens
    c = c+a;
    printf("%d\n", c);
    c = 100;
    c += a;
    printf("%d", c);
    return 0;
}