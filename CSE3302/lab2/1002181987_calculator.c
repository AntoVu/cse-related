#include <stdio.h>

// Adds two floats together
float add(float x, float y) {
    return (x + y);
}

// Subtracts a float from the other
float subtract(float x, float y) {
    return (x - y);
}

// Multiplies two floats together
float multiply(float x, float y) {
    return (x * y);
}

// Divides a float from the other
float divide(float x, float y) {
    return (x / y);
}

int main() {
    // Variables to check if the entered values are valid
    int validInput1 = 0;
    int validInput2 = 0;
    int validOperation = 0;

    // Input and result variables
    float num1 = 0;
    float num2 = 0;
    char operation;
    float result = 0;

    // Validate first input
    while (!validInput1) {
        printf("Enter the first value you would like to perform an operation on: ");
        while (scanf("%f", &num1) != 1) {
            printf("Invalid input for first number, please try again: ");
            // getchar used to get rid of newline characters, used multiple times later on
            while (getchar() != '\n');
        };
        while (getchar() != '\n');
        validInput1 = 1;
    }

    // Validate second input
    while (!validInput2) {
        printf("Enter the second value you would like to perform an operation on: ");
        while (scanf("%f", &num2) != 1) {
            printf("Invalid input for second number, please try again: ");
            while (getchar() != '\n');
        };
        while (getchar() != '\n');
        validInput2 = 1;
    }

    // Validate operation input using switch statement to choose which operation
    while (!validOperation) {
        printf("Enter the operation you would like to perform (i.e. +, -, *, or /): ");
        scanf("%c", &operation);
        switch(operation) {
            case '+':
                result = add(num1, num2);
                validOperation = 1;
                break;
            case '-':
                result = subtract(num1, num2);
                validOperation = 1;
                break;
            case '*':
                result = multiply(num1, num2);
                validOperation = 1;
                break;
            case '/':
                if (num2 == 0) {
                    printf("Cannot divide by 0, please try again.\n");
                    while (getchar() != '\n');
                    break;
                }
                result = divide(num1, num2);
                validOperation = 1;
                break;
            default:
                while (getchar() != '\n');
                printf("Invalid operation, please choose again.\n");
        }
    }
    printf("The result of the operation %.3f %c %.3f is %.5f.\n", num1, operation, num2, result);
    
    return 0;
}