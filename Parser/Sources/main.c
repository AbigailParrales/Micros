/*
 * main implementation: use this 'C' sample to create your own application
 *
 */





#include "derivative.h" /* include peripheral declarations */

int e_to_three(char *s)
	{
	    char *p;
	    int count = 0;
	    for (p = s; *p; ++p) {
	        if (*p == 'e') {
	            *p = '3';
	            count++;
	        }
	    }
	    return count;
	}

int main(void)
{
	
	int counter = 0;
	
	
	
	
	for(;;) {	   
	   	counter++;
	}
	
	return 0;
}
