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
    private static final int numCards = 29;
    private static Card[] loadDeck() {
        Card[] cards = new Card[numCards];
        int i = 0;
        cards[i++] = new Card("Amplitude Modulation", "AM");
        cards[i++] = new Card("Frequency Modulation", "FM");
        cards[i++] = new Card("Phase Modulation", "PM");
        cards[i++] = new Card("Internet Protocol", "IP");
        cards[i++] = new Card("Domain Name System", "DNS");
        cards[i++] = new Card("Media Access Control", "MAC");
        cards[i++] = new Card("Open Systems Interconnection", "OSI");
        cards[i++] = new Card("Transmission Control Protocol", "TCP");
        cards[i++] = new Card("User Datagram Program", "UDP");
        cards[i++] = new Card("File Transfer Protocol", "FTP");
        cards[i++] = new Card("Hypertext Transfer Protocol", "HTTP");
        cards[i++] = new Card("Simple Mail Transfer Protocol", "SMTP");
        cards[i++] = new Card("Internet Access Message Protocol", "IMAP");
        cards[i++] = new Card("Point of Presence", "PoP");
        cards[i++] = new Card("Quality of Service", "QoS");
        cards[i++] = new Card("Low Earth Orbit", "LEO");
        cards[i++] = new Card("Geostationary Equalatorial Orbit", "GEO");
        cards[i++] = new Card("Mean Time Between Failure", "MTBF");
        cards[i++] = new Card("Denial of Service", "DoS");
        cards[i++] = new Card("Data Encryption Standard", "DES");
        cards[i++] = new Card("Advanced Encryption Standard", "AES");
        cards[i++] = new Card("Rivest Shamir Adleman", "RSA");
        cards[i++] = new Card("Application", "OSI Model: A");
        cards[i++] = new Card("Presentation", "OSI Model: P");
        cards[i++] = new Card("Session", "OSI Model: S");
        cards[i++] = new Card("Transport", "OSI Model: T");
        cards[i++] = new Card("Network", "OSI Model: N");
        cards[i++] = new Card("Data Link", "OSI Model: D");
        cards[i++] = new Card("Physical", "OSI Model: P");
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