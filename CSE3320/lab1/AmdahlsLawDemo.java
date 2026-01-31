import java.util.Random;
import java.util.concurrent.TimeUnit;
import java.util.Arrays;

public class AmdahlsLawDemo {

    private static final int DATA_COUNT = 50000;
    private static final int MIN_VAL = 1;
    private static final int MAX_VAL = 100000;

    /**
     * Generates an array of random numbers in memory.
     * This represents the 'serial' or non-optimizable part of our program.
     * @param count The number of integers to generate.
     * @return An array of random integers.
     */
    public static int[] generateData(int count) {
        System.out.println("Generating " + count + " random numbers...");
        int[] data = new int[count];
        Random rand = new Random();
        for (int i = 0; i < count; i++) {
            data[i] = rand.nextInt(MAX_VAL - MIN_VAL + 1) + MIN_VAL;
        }
        return data;
    }

    // --- Algorithm Implementations ---

    /**
     * TODO: Implement your chosen 'slow' or unoptimized algorithm here.
     * Example: Bubble Sort for an array of numbers.
     * The function should take the data array and sort it in place.
     * @param data The array of integers to be processed.
     */
    public static void unoptimizedAlgorithm(int[] data) {
        // --- Your unoptimized algorithm implementation goes here ---
        int temp = -1;
        for (int i = 0; i < DATA_COUNT-1; i++) {
            for (int j = 0; j < DATA_COUNT - i - 1; j++) {
                if (data[j] > data[j+1]) {
                    temp = data[j];
                    data[j] = data[j+1];
                    data[j+1] = temp;
                }
            }
        }
        // System.out.println("Warning: unoptimizedAlgorithm is not implemented.");
        // Remove this line after implementing
    }

    /**
     * TODO: Implement your chosen 'fast' or optimized algorithm here.
     * Example: Quick Sort, Merge Sort, or simply Arrays.sort().
     * The function should take the data array and sort it in place.
     * @param data The array of integers to be processed.
     */
    public static void optimizedAlgorithm(int[] data) {
        // --- Your optimized algorithm implementation goes here ---
        Arrays.sort(data);
        // System.out.println("Warning: optimizedAlgorithm is not implemented.");
        // Remove this line after implementing
    }


    // --- Main Analysis ---

    public static void main(String[] args) {
        System.out.println("\n--- Running Unoptimized Version ---");

        // Time the entire process for the unoptimized version
        long startTotalUnoptimized = System.nanoTime();

        // Serial part: Generating data in memory
        int[] data1 = generateData(DATA_COUNT);

        // Processing part: This is the part we are 'optimizing'
        long startProcessingUnoptimized = System.nanoTime();
        unoptimizedAlgorithm(data1);
        long endProcessingUnoptimized = System.nanoTime();

        long endTotalUnoptimized = System.nanoTime();

        // TODO: Calculate the time differences for the unoptimized run in seconds.
        //       - Use variables of type 'double' to store the times.
        //       - Total time = (endTotalUnoptimized - startTotalUnoptimized) / 1_000_000_000.0;
        //       - Processing-only time = (endProcessingUnoptimized - startProcessingUnoptimized) / 1_000_000_000.0;
        //       - Print both calculated times so you can record them for your report.

        double T_unoptimized = (endTotalUnoptimized - startTotalUnoptimized) / 1_000_000_000.0;
        double T_processing_unoptimized = (endProcessingUnoptimized - startProcessingUnoptimized) / 1_000_000_000.0;
        System.out.println("\n- Unoptimized Time -\nTotal: " + T_unoptimized + "\nProcess: " + T_processing_unoptimized);


        System.out.println("\n--- Running Optimized Version ---");

        // Time the entire process for the optimized version
        long startTotalOptimized = System.nanoTime();

        // Serial part: Generating data in memory
        int[] data2 = generateData(DATA_COUNT);

        // Processing part: Using the optimized function
        long startProcessingOptimized = System.nanoTime();
        optimizedAlgorithm(data2);
        long endProcessingOptimized = System.nanoTime();

        long endTotalOptimized = System.nanoTime();
        
        // TODO: Calculate the time differences for the optimized run in seconds.
        //       - Calculate the total execution time.
        //       - Calculate the processing-only time.
        //       - Print both calculated times so you can record them for your report.

        double T_optimized = (endTotalOptimized - startTotalOptimized) / 1_000_000_000.0;
        double T_processing_optimized = (endProcessingOptimized - startProcessingOptimized) / 1_000_000_000.0;
        System.out.println("\n- Unoptimized Time -\nTotal: " + T_optimized + "\nProcess: " + T_processing_optimized);


        // --- Calculations for the Report ---
        System.out.println("\n--- Amdahl's Law Analysis Data ---");
        System.out.println("Use the timing data you collected and printed above to complete your report.");

        // TODO: Students, use your calculated timing data to find the following factors.
        //       You will need to define variables for your timings (e.g., tTotalUnoptimized)
        //       to perform these calculations in your code or manually for the report.
        // 1. Calculate P (Proportion of optimizable code):
        //    P = tProcessingUnoptimized / tTotalUnoptimized
        //
        // 2. Calculate S (Speedup of the optimized part):
        //    S = tProcessingUnoptimized / tProcessingOptimized
        //
        // 3. Calculate the Measured Overall Speedup:
        //    Measured Speedup = tTotalUnoptimized / tTotalOptimized
        //
        // 4. Calculate the Theoretical Overall Speedup using Amdahl's Law:
        //    Theoretical Speedup = 1 / ((1 - P) + (P / S))
        //
        // 5. Compare the Measured and Theoretical speedups in your report and discuss.

        double P = T_processing_unoptimized / T_unoptimized;
        double S = T_processing_optimized / T_optimized;
        double speedup = T_unoptimized / T_optimized;
        double theorteical = 1 / ((1-P) + (P/S));
        System.out.println("\nP: " + P + "\nS: " + S + "\nSpeedup: " + speedup + "\nTheoretical: " + theorteical);
    }
}

