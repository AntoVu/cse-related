/*

    ==================================================================================
    IMPORTANT: This file will NOT compile and run properly until you FINISH Card.java!
    ==================================================================================

*/

import java.util.Scanner;
import java.util.Random;

public class FlashCards {
    private static Scanner scanner = new Scanner(System.in);
    private static Random random = new Random();

    // loadDeck returns an array of Card filled with our vocab words for Exam #1.
    // You MUST provide Card.java correctly for this to compile and run!
    private static final int numCards = 14;
    private static Card[] loadDeck() {
        Card[] cards = new Card[numCards];
        int i = 0;
        cards[i++] = new Card("Central Processing Unit", "CPU");
        cards[i++] = new Card("Berkeley Software Distribution", "BSD");
        cards[i++] = new Card("Process Identification", "PID");
        cards[i++] = new Card("First Come First Serve", "FCFS");
        cards[i++] = new Card("Shortest Job Next", "SJN");
        cards[i++] = new Card("Shortest Remaining Time First", "SRTF");
        cards[i++] = new Card("Round Robin", "RR");
        cards[i++] = new Card("Process Control Block", "PCB");
        cards[i++] = new Card("Common Business Oriented Language", "COBOL");
        cards[i++] = new Card("Formula Translator", "Fortran");
        cards[i++] = new Card("Control Program for Microcomputers", "CPM");
        cards[i++] = new Card("Control Program/Monitor", "CP/M");
        cards[i++] = new Card("Inter Process Communication", "IPC");
        cards[i++] = new Card("Mutual Exclusion", "Mutex");
        return cards;
    }
    
    public static void main(String[] args) {
        try {
            System.out.println("Error initializing cards.");
        }
        catch(Exception e) {
            System.err.println(e.getMessage());
            System.exit(-1);
        }

        Card[] deck = loadDeck();

        System.out.print("FLASH CARDS\n===========\n\nVocabulary terms:\n\n");
        for (int i = 0; i < numCards; ++i) {
            System.out.println("* " + deck[i].getTerm());
        }

        Scanner scanner = new Scanner(System.in);
        Random rand = new Random();
        int randTerm = rand.nextInt(numCards);
        // for (int i = randTerm; ; i = randTerm) {
        for (int i = 0; i < numCards; i++) {
            System.out.printf("\n" + deck[i] + "\nWhich term matches this definition ('q' to exit)? ");
            String input = scanner.nextLine();
            if (input.equals("q")) {
                System.exit(0);
            }
            else if (deck[i].attempt(input)) {
                System.out.println("Correct!");
            }
            else {
                System.out.println("No, the term is " + deck[i].getTerm());
            }
            randTerm = rand.nextInt(numCards);
        }
    }
}