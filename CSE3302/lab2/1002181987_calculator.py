# Adds two floats together
def add(x, y):
    return x + y

# Subtracts a float from the other
def subtract(x, y):
    return x - y

# Multiplies two floats together
def multiply(x, y):
    return x * y

# Divides a float from the other
def divide(x, y):
    return x / y

# Validate first input
while True:
    try:
        num1 = float(input("Enter the first value you would like to perform an operation on: "))
        break
    except ValueError:
        print("Invalid input for first number, please try again.")

# Validate second input
while True:
    try:
        num2 = float(input("Enter the second value you would like to perform an operation on: "))
        break
    except ValueError:
        print("Invalid input for second number, please try again.")

# Validate operation using same concept as switch statement
# Python apparently doesn't have switch statements which is news to me
while True:
    operation = input("Enter the operation you would like to perform (i.e. +, -, *, or /): ")

    # Ensure exactly one character was entered
    if len(operation) != 1:
        print("Invalid operation, please choose again.")
        continue

    if operation == '+':
        result = add(num1, num2)
        break
    elif operation == '-':
        result = subtract(num1, num2)
        break
    elif operation == '*':
        result = multiply(num1, num2)
        break
    elif operation == '/':
        if num2 == 0:
            print("Cannot divide by 0, please try again.")
            continue
        result = divide(num1, num2)
        break
    else:
        print("Invalid operation, please choose again.")

print(f"The result of {num1:.3f} {operation} {num2:.3f} is {result:.5f}")