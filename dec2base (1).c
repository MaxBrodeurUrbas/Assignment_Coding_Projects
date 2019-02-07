
/*
 * dec2base.c
 *
 *  Created on: Sep 18, 2017
 *      Author: Max Brodeur-Urbas
 *      McGill ID: 260806240
 */

#include <stdio.h>

int main(int argc, char *argv[]) {
	int a,b,n=0;
	char lut[] = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";
	char emt[32];
	int Q;
	int h;

	if (argc == 2){				/*if only one input, program will assume base 2*/
			b = 2;
			sscanf(argv[1], "%d", &a);
		}


	else if (argc == 3){			/* if the user inputs two numbers, first is made the argument and second the base*/
			sscanf(argv[1], "%d", &a);
			sscanf(argv[2], "%d", &b);
	}


	else {						/*if user doesn't input 1 or 2 numbers, program won't function so user is warned with message */
		printf ("you have the wrong number of arguments \n");
		return 0;
	}


		Q=a;						/*Q begins as the inputed argument*/

    printf("The Base-%d form of %d is: ", b, a);

	while (Q>0){					/*through while loop; Q is divided repeatedly by base until Q=0 at which point loop breaks*/
		Q = a / b;
		int R = a % b;
		emt[n]= lut[R];			/*beginning at pos. 0 in array, remainder is used as index for look up table and that value is put into the emt[] array*/
		n++;						/*n is the counter that began at 0 but goes up by 1 through each iteration to move along array*/
		a = Q;					/*a is made equal to Q so loop can continue iterating*/
	}

	for (h = n-1; h>=0 ; h--) {	/*h starts at n-1 (because n was primed for next iteration) and counts down to print values in array in reverse*/
		printf("%c",emt[h]);
	}
	printf("\n");
	return(0);
}


