import java.util.Scanner;

public class Main {
    public static void main(String[] args) {
        // Set up scanner for user input
        Scanner in = new Scanner(System.in);

        // Input and result variables
        float num1 = 0;
        float num2 = 0;
        char operation = ' ';
        float result = 0;

        // Validate first input
        System.out.print("Enter the first value you would like to perform an operation on: ");
        while (true) {
            if (in.hasNextFloat()) {
                num1 = in.nextFloat();
                in.nextLine(); // get rid of left over newline
                break;
            } else {
                System.out.print("Invalid input for first number, please try again: ");
                in.next(); // get rid of invalid token
            }
        }

        // Validate second input
        System.out.print("Enter the second value you would like to perform an operation on: ");
        while (true) {
            if (in.hasNextFloat()) {
                num2 = in.nextFloat();
                in.nextLine(); // get rid of left over newline
                break;
            } else {
                System.out.print("Invalid input for second number, please try again: ");
                in.next(); // get rid of invalid token
            }
        }

        // Construct Calculator object with the two numbers
        Calculator values = new Calculator(num1, num2);

        // Validate operation input
        while (true) {
            System.out.print("Enter the operation you would like to perform (i.e. +, -, *, or /): ");
            String input = in.nextLine().trim();

            // Ensure exactly one character was entered
            if (input.length() != 1) {
                System.out.println("Invalid operation, please choose again.");
                continue;
            }

            // Get the operation character
            operation = input.charAt(0);

            // Choose the right operation from the input
            switch (operation) {
                case '+':
                    result = values.add();
                    break;
                case '-':
                    result = values.subtract();
                    break;
                case '*':
                    result = values.multiply();
                    break;
                case '/':
                    if (num2 == 0) {
                        System.out.println("Cannot divide by 0, please try again.");
                        continue;
                    }
                    result = values.divide();
                    break;
                default:
                    System.out.println("Invalid operation, please choose again.");
                    continue;
            }
            break;
        }

        System.out.printf("The result of %.3f %c %.3f is %.5f%n", num1, operation, num2, result);

        // Close scanner
        in.close();
    }
}