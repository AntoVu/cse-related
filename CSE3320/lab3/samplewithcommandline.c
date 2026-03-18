#include <stdio.h>

int main(int argc, char *argv[]) {
    printf("Your command line argument was: %c", *argv[1]);
    return 0;
}