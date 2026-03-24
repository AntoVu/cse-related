// CSE 2312 Computer Org and Assembly Language Programming
// Lab 2 Question 1

#include <stdlib.h>   // EXIT_ macro
#include <stdio.h>    // printf
#include <inttypes.h> // PRI macro
#include <stdint.h>   // C99 uintX_t and intX_t types
#include <stdbool.h>  // C99 bool

extern bool isMultOf2(uint32_t x); // return 1 if x is a multiple of 2, return 0 otherwise

int main()
{
    uint32_t x;
	
    x = 1;
    if (isMultOf2(x))
        printf("%"PRIu32" is mult of 2\n", x);
    else
        printf("%"PRIu32" is not mult of 2\n", x);
	
	x = 12;
    if (isMultOf2(x))
        printf("%"PRIu32" is mult of 2\n", x);
    else
        printf("%"PRIu32" is not mult of 2\n", x);
	
	x = 123;
    if (isMultOf2(x))
        printf("%"PRIu32" is mult of 2\n", x);
    else
        printf("%"PRIu32" is not mult of 2\n", x);
	
	x = 1234;
    if (isMultOf2(x))
        printf("%"PRIu32" is mult of 2\n", x);
    else
        printf("%"PRIu32" is not mult of 2\n", x);

        
    return EXIT_SUCCESS;
}

