public class Card {
    private String term = "";
    private String definition = "";

    public Card(String term, String definition) {
        if (term == null || definition == null) throw new IllegalArgumentException("Null value for term or definition!");
        if (term.length() == 0 || definition.length() == 0) throw new IllegalArgumentException("Invalid length for term or definition!");

        this.term = term;
        this.definition = definition;
    }

    @Override
    public String toString() {
        return definition;
    }

    public boolean attempt(String response) {
        return (response.toUpperCase().equals(term.toUpperCase()));
    }

    public String getTerm() {
        return term;
    }
}