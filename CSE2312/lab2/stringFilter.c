// CSE 2312 Computer Org and Assembly Language Programming
// Lab 2 question 3

#include <stdlib.h>   // EXIT_ macro
#include <stdio.h>    // printf
#include <stdint.h>   // C99 uintX_t and intX_t types


extern void stringFilter(char strTo[], const char strFrom[]); // copy all decimal number characters (0~9) in strFrom to strTo 



int main(void)
{
    char str1[] = "My 10-digit ID number is 1002181987."; // Replace "xxx" in the string with your 10-digit student ID number.
    char str2[20];
	stringFilter(str2, str1);
    printf("strFrom = %s\n", str1);
    printf("strTo = %s\n", str2);

    return EXIT_SUCCESS;
}


