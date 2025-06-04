/*|Factorials|--------------------------------------------------------------------
#
# Project: Lab2
# Program: ROBT1270
#
# Description:
#   finding n factorial
# n!
#
# Author: Peter Stogneff
# Date Created: 01/15/2025
# Last Modified: 01/15/2025
# -----------------------------------------------------------------------------*/

/*|Defininitions and Includes|--------------------------------------------------------------------*/
#include <stdio.h>		// getchar, getchar
#define N 20 //limits: 0 - 19. 20 gives a negative because of int overflow, and a factorial has to be 0 or bigger

/*|Main|--------------------------------------------------------------------*/
int main() {
	// Variable Definitions	
	int number = 1;
	int sum = 1;
 
	// sum the numbers 1 to 5
	while(number <= N) {
		sum=number*sum;
		number++;
	}
	
	// print the factorial of 1-5 which is 120
	printf("\nThe factorial of 1 to %d is %d", N, sum);

	// wait for user input
	printf("\n\nPress ENTER to continue...");
	getchar();

	// Return and exit main
	return(0);
}
