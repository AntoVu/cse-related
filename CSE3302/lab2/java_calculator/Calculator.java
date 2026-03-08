public class Calculator {
    // Encapsulated num1 and num2 values
    private float num1;
    private float num2;

    // Construct calculator with the two values
    public Calculator(float num1, float num2) {
        this.num1 = num1;
        this.num2 = num2;
    }

    // Add the two values together
    public float add() {
        return this.num1 + this.num2;
    }

    // Subtracts the two values from each other
    public float subtract() {
        return this.num1 - this.num2;
    }

    // Multiplies the two values together
    public float multiply() {
        return this.num1 * this.num2;
    }

    // Divide the two values from each other
    public float divide() {
        return this.num1 / this.num2;
    }
}