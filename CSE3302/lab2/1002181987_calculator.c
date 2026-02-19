#include <stdio.h>

float add(float x, float y) {
    return (x + y);
}

float subtract(float x, float y) {
    return (x - y);
}

float multiply(float x, float y) {
    return (x * y);
}

float divide(float x, float y) {
    return (x / y);
}


// TS DONT WORK YET
int main() {
    int done = 0;
    float num1 = 0;
    float num2 = 0;
    char operation;
    float result = 0;
    while (!done) {
        printf("Enter the first value you would like to perform an operation on: ");
        while (scanf("%f", &num1) != 1) {
            printf("Invalid input for first number, please try again: ");
            getchar();
        };
        printf("Enter the second value you would like to perform an operation on: ");
        while (scanf("%f", &num2) != 1) {
            printf("Invalid input for second number, please try again: ");
            getchar();
        };
        getchar();
        printf("Enter the operation you would like to perform (i.e. +, -, *, or /): ");
        scanf("%c", &operation);
        switch(operation) {
            case '+':
                result = add(num1, num2);
                done = 1;
                break;
            case '-':
                result = subtract(num1, num2);
                done = 1;
                break;
            case '*':
                result = multiply(num1, num2);
                done = 1;
                break;
            case '/':
                if (num2 == 0) {
                    printf("Cannot divide by 0, please try again.\n");
                    break;
                }
                result = divide(num1, num2);
                done = 1;
                break;
            default:
                printf("Invalid operation, please choose again.\n");
        }
    }
    printf("The result of the operation %.3f %c %.3f is %.5f.\n", num1, operation, num2, result);
    
    return 0;
}