#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_PARAGRAPH 10001
#define BUFFER_SIZE 100 // increase if smth breaks

int count_space(const char *string);
void to_lowercase(const char *source, char *destination, int max);
void insertion_sort(char **ptrs, int n);
int compare_no_case(const char *a, const char *b);
void binary_search(char **ptrs, int n, const char *fname_words);

int main(int argc, char** argv)
{
	char *fname_par = NULL;
	char *fname_words = NULL;

	int mode = 0; // 0 - normal, 1 - verbose

	if (argc < 4 ) {
		printf("Not enough arguments. Need mode and 2 filenames, e.g.:   0 small.txt words1.txt\n");
		return 0;
	}
	else {
		fname_par = argv[2];
		fname_words = argv[3];
		mode = atoi(argv[1]);
	}

	printf("mode: %d  |  paragraph: %s  |  words: %s\n", mode, fname_par, fname_words);  // KEEP THIS LINE

	// Open paragraph file
	FILE *par_file = fopen(fname_par, "r");
	if (par_file == NULL) {
		printf("Error opening paragraph file.\n");
		return 1;
	}

	// Allocate memoryu on the heap for paragraph
	char *paragraph = malloc(MAX_PARAGRAPH);
	if (paragraph == NULL) {
		printf("Error allocating memory.");
		return 1;
	}

	// Read everything in the file then close it
	// https://www.geeksforgeeks.org/c/fread-function-in-c/
	int num_read = fread(paragraph, 1, MAX_PARAGRAPH - 1, par_file);
	fclose(par_file);
	paragraph[num_read] = '\0';

	// Count num of spaces then add 1 to it for the number of words
	int num_words = count_space(paragraph) + 1;

	// Make large enough array to hold one pointer for each number of words
	char **ptrs = malloc(num_words * sizeof(*ptrs));
	if (ptrs == NULL && num_words > 0) {
		printf("Error allocating ptrs.\n");
		free(paragraph);
		return 1;
	}

	// https://www.w3schools.com/c/ref_string_strtok.php
	char *token;
	int index = 0;
	token = strtok(paragraph, " \t\n)(.,?!:;");
	while (token != NULL && index < num_words) {
		ptrs[index] = token;
		index++;
		token = strtok(NULL, " \t\n)(.,?!:;");
	}
	num_words = index;

	// Print the data for verbose mode
	if (mode == 1) {
		printf("\n-- Original data --\n");
        printf("  i  |   pointers[i]    | word\n");
        printf("-----|------------------|------------------\n");
		for (int i = 0; i < num_words; i++) {
			printf("%4d | %16p | %s\n", i, (void*)ptrs[i], ptrs[i]);
		}

		insertion_sort(ptrs, num_words);
		printf("\n-- Clean and sorted data --\n");
		printf("  i  |   pointers[i]    | word\n");
		printf("-----|------------------|------------------\n");
		for (int i = 0; i < num_words; i++) {
			char lower[BUFFER_SIZE];
			to_lowercase(ptrs[i], lower, sizeof(lower));
			printf("%4d | %16p | %s\n", i, (void*)ptrs[i], lower);
   		}
	}
	// Print data for regular mode
	else {
		printf("\n-- Original data --\n");
		for (int i = 0; i < num_words; i++) {
			printf("%d %s\n", i, ptrs[i]);
		}

		insertion_sort(ptrs, num_words);
		printf("\n-- Clean and sorted data --\n");
		for (int i = 0; i < num_words; i++) {
			char lower[BUFFER_SIZE];
			to_lowercase(ptrs[i], lower, sizeof(lower));
			printf("%d %s\n", i, lower);
		}
	}
	// Print the binary search after the others since I think they're the same for both modes
	binary_search(ptrs, num_words, fname_words);

	// Free them so valgrind doesn't yell at me
	free(ptrs);
	free(paragraph);

	return 0;
}

// Count number of spaecs
int count_space(const char *string) {
	int count = 0;
	for (int i = 0; i < (int)strlen(string); i++) {
		if (*(string + i) == ' ') {
			count++;
		}
	}
	return count;
}

void to_lowercase(const char *source, char *destination, int max) {
    int i = 0;
    while (source[i] && i < max - 1) {
        destination[i] = tolower(source[i]);
        i++;
    }
    destination[i] = '\0';
}

int compare_no_case(const char *a, const char *b) {
	while (*a && *b) {
		if (tolower(*a) != tolower(*b)) {
			return (tolower(*a) < tolower(*b)) ? -1 : 1;
		}
		a++;
		b++;
	}
	if (*a) {
		return 1;
	}
	if (*b) {
		return -1;
	}
	return 0;
}

// Insertion sort notes https://ranger.uta.edu/~alex/courses/3318/daily/notes/08_26_002.pdf
void insertion_sort(char **ptrs, int n) {
	for (int i = 1; i < n; i++) {
		char *curr = ptrs[i];
		int j = i-1;
		while (j >= 0 && compare_no_case(ptrs[j], curr) > 0) {
			ptrs[j+1] = ptrs[j];
			j--;
		}
		ptrs[j+1] = curr;
	}
}

// Binary search notes TH 08/28 https://ranger.uta.edu/~alex/courses/3318/daily/_daily.html
void binary_search(char **ptrs, int n, const char *fname_words) {
	FILE *word_file = fopen(fname_words, "r");
	if (word_file == NULL) {
		printf("Error opening words file.\n");
		return;
	}

	char buffer[BUFFER_SIZE];
	printf("\n-- Binary search --\n");
	while(fgets(buffer, sizeof(buffer), word_file)) {
		int len = strlen(buffer);
		if (len && buffer[len-1] == '\n') {
			buffer[len-1] = '\0';
		}

		char lower_buffer[BUFFER_SIZE];
		to_lowercase(buffer, lower_buffer, sizeof(lower_buffer));

		printf("%s\n", lower_buffer);

		int left = 0;
		int right = n - 1;
		int found = 0;
		int iterations = 0;
		while (left <= right) {
			// int mid = left + (left + right) / 2; // this broke the code
			int mid = (left + right) / 2;
			if (iterations > 0) {
				printf(", ");
			}
			printf("%d", mid);
			iterations++;

			int compare = compare_no_case(lower_buffer, ptrs[mid]);
			if (compare == 0) {
				found = 1;
				break;
			}
			else if (compare < 0) {
				right = mid - 1;
			}
			else {
				left = mid + 1;
			}
		}
		printf(", (%d iterations) %s\n", iterations, found ? "found" : "not found");
	}
	fclose(word_file);
}