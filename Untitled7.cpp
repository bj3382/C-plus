#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define MAX_TEXT_LEN 1000
#define ALPHABET_SIZE 26

// Expected frequency of letters in English text (based on statistical analysis)
const double english_frequencies[ALPHABET_SIZE] = {
    8.167, 1.492, 2.782, 4.253, 12.702, 2.228, 2.015, 6.094, 6.966, 0.153,
    0.772, 4.025, 2.406, 6.749, 7.507, 1.929, 0.095, 5.987, 6.327, 9.056,
    2.758, 0.978, 2.360, 0.150, 1.974, 0.074
};

// Function to calculate frequency of each letter in a text
void calculate_frequencies(const char *text, double frequencies[ALPHABET_SIZE]) {
    int count[ALPHABET_SIZE] = {0};
    int total_letters = 0;

    // Count occurrences of each letter
    for (int i = 0; text[i] != '\0'; i++) {
        if (text[i] >= 'A' && text[i] <= 'Z') {
            count[text[i] - 'A']++;
            total_letters++;
        } else if (text[i] >= 'a' && text[i] <= 'z') {
            count[text[i] - 'a']++;
            total_letters++;
        }
    }

    // Calculate frequencies as percentages
    for (int i = 0; i < ALPHABET_SIZE; i++) {
        if (total_letters > 0) {
            frequencies[i] = (double)count[i] / total_letters * 100.0;
        } else {
            frequencies[i] = 0;
        }
    }
}

// Function to decrypt the text with a given shift
void decrypt_additive_cipher(const char *ciphertext, char *plaintext, int shift) {
    for (int i = 0; ciphertext[i] != '\0'; i++) {
        if (ciphertext[i] >= 'A' && ciphertext[i] <= 'Z') {
            plaintext[i] = ((ciphertext[i] - 'A' - shift + ALPHABET_SIZE) % ALPHABET_SIZE) + 'A';
        } else if (ciphertext[i] >= 'a' && ciphertext[i] <= 'z') {
            plaintext[i] = ((ciphertext[i] - 'a' - shift + ALPHABET_SIZE) % ALPHABET_SIZE) + 'a';
        } else {
            plaintext[i] = ciphertext[i];  // Non-alphabetic characters remain unchanged
        }
    }
    plaintext[strlen(ciphertext)] = '\0';  // Null terminate the plaintext
}

// Function to calculate the difference between the text frequency and English frequency
double calculate_difference(double text_frequencies[ALPHABET_SIZE]) {
    double difference = 0.0;
    for (int i = 0; i < ALPHABET_SIZE; i++) {
        difference += abs(text_frequencies[i] - english_frequencies[i]);
    }
    return difference;
}

// Structure to store possible plaintext and the score
typedef struct {
    char plaintext[MAX_TEXT_LEN];
    double score;
} PossiblePlaintext;

// Compare function for sorting by score (lower score is better)
int compare(const void *a, const void *b) {
    PossiblePlaintext *plaintextA = (PossiblePlaintext *)a;
    PossiblePlaintext *plaintextB = (PossiblePlaintext *)b;
    return (plaintextA->score > plaintextB->score) - (plaintextA->score < plaintextB->score);
}

int main() {
    char ciphertext[MAX_TEXT_LEN];
    int top_n;

    
    printf("Enter the ciphertext: ");
    fgets(ciphertext, sizeof(ciphertext), stdin);
    ciphertext[strcspn(ciphertext, "\n")] = '\0';  // Remove newline character

    printf("Enter the number of top possible plaintexts to display: ");
    scanf("%d", &top_n);

    
    PossiblePlaintext possible_plaintexts[ALPHABET_SIZE];

    
    for (int shift = 0; shift < ALPHABET_SIZE; shift++) {
        
        decrypt_additive_cipher(ciphertext, possible_plaintexts[shift].plaintext, shift);

        
        double text_frequencies[ALPHABET_SIZE];
        calculate_frequencies(possible_plaintexts[shift].plaintext, text_frequencies);

        
        possible_plaintexts[shift].score = calculate_difference(text_frequencies);
    }

    
    qsort(possible_plaintexts, ALPHABET_SIZE, sizeof(PossiblePlaintext), compare);

    
    printf("\nTop %d possible plaintexts:\n", top_n);
    for (int i = 0; i < top_n && i < ALPHABET_SIZE; i++) {
        printf("%2d. Shift %d: %s (Score: %.2f)\n", i + 1, i, possible_plaintexts[i].plaintext, possible_plaintexts[i].score);
    }

    return 0;
}
