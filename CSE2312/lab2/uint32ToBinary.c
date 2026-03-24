// CSE 2312 Computer Org and Assembly Language Programming
// Lab 2 question 2

#include <stdlib.h>   // EXIT_ macro
#include <stdio.h>    // printf
#include <inttypes.h> // PRI macro
#include <stdint.h>   // C99 uintX_t and intX_t types

extern void uint32ToBinary(char str[], uint32_t x); // convert a 32-bit integer into a string

int main(void)
{
    uint32_t a;
    char str[33]; // 32 bits + null
    a = 0x12345678;
    uint32ToBinary(str, a); 
    printf("a = 0x%08"PRIx32" = %s(base-2)\n", a, str); 
	
    return EXIT_SUCCESS;
}


