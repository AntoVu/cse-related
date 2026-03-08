import java.util.Arrays;
import java.util.List;

public class Parser {
    static List<String> tokens;
    static int pos = 0;

    static String lookahead() {
        if (pos < tokens.size()) return tokens.get(pos);
        return "EOF";
    }

    static void match(String t) {
        if (lookahead().equals(t)) pos++;
        else throw new RuntimeException("Expected " + t);
    }

    static void expr() {
        term();
        if (lookahead().equals("+")) {
            match("+");
            expr();
        }
    }

    static void term() {
        factor();
        if (lookahead().equals("*")) {
            match("*");
            term();
        }
    }

    static void factor() {
        if (lookahead().equals("id")) {
            match("id");
        }
        else if (lookahead().equals("(")) {
            match("(");
            expr();
            match(")");
        }
        else {
            throw new RuntimeException("Invalid expression");
        }
    }

    public static void main(String[] args) {
        tokens = Arrays.asList("id", "+", "id", "*", "id");
        expr();
        System.out.println("Parsing successful");
    }
}