#include <stdio.h>
#include <string.h>

// Permutation functions
int P10[] = {3, 5, 2, 7, 4, 10, 1, 9, 8, 6};
int P8[] = {6, 3, 7, 4, 8, 5, 10, 9};
int P4[] = {2, 4, 3, 1};
int IP[] = {2, 6, 3, 1, 4, 8, 5, 7};
int IP_inv[] = {4, 1, 3, 5, 7, 2, 8, 6};
int EP[] = {4, 1, 2, 3, 2, 3, 4, 1};

// S-boxes
int S0[4][4] = {{1, 0, 3, 2}, {3, 2, 1, 0}, {0, 2, 1, 3}, {3, 1, 3, 2}};
int S1[4][4] = {{0, 1, 2, 3}, {2, 0, 1, 3}, {3, 0, 1, 0}, {2, 1, 0, 3}};

// Function to perform a permutation
void permute(int* input, int* output, int* perm, int n) {
    for (int i = 0; i < n; i++) {
        output[i] = input[perm[i] - 1];
    }
}

// Function to left shift the key
void leftShift(int* input, int n) {
    int temp = input[0];
    for (int i = 0; i < n - 1; i++) {
        input[i] = input[i + 1];
    }
    input[n - 1] = temp;
}

// XOR operation
void XOR(int* a, int* b, int* result, int n) {
    for (int i = 0; i < n; i++) {
        result[i] = a[i] ^ b[i];
    }
}

// Function to generate the subkeys
void generateKeys(int* key, int* K1, int* K2) {
    int permutedKey[10], left[5], right[5], combined[10];
    
    // Perform P10 permutation
    permute(key, permutedKey, P10, 10);
    
    // Split the key into two halves
    for (int i = 0; i < 5; i++) {
        left[i] = permutedKey[i];
        right[i] = permutedKey[i + 5];
    }
    
    // Left shift both halves
    leftShift(left, 5);
    leftShift(right, 5);
    
    // Combine left and right and apply P8 permutation to generate K1
    for (int i = 0; i < 5; i++) {
        combined[i] = left[i];
        combined[i + 5] = right[i];
    }
    permute(combined, K1, P8, 8);
    
    // Left shift again for both halves
    leftShift(left, 5);
    leftShift(left, 5);
    leftShift(right, 5);
    leftShift(right, 5);
    
    // Combine left and right and apply P8 to generate K2
    for (int i = 0; i < 5; i++) {
        combined[i] = left[i];
        combined[i + 5] = right[i];
    }
    permute(combined, K2, P8, 8);
}

// S-Box function
void sBox(int* input, int* output) {
    int row, col, s0Output, s1Output;
    
    // S0
    row = input[0] * 2 + input[3];
    col = input[1] * 2 + input[2];
    s0Output = S0[row][col];
    
    // S1
    row = input[4] * 2 + input[7];
    col = input[5] * 2 + input[6];
    s1Output = S1[row][col];
    
    // Convert S0 and S1 output to binary
    output[0] = (s0Output >> 1) & 1;
    output[1] = s0Output & 1;
    output[2] = (s1Output >> 1) & 1;
    output[3] = s1Output & 1;
}

// Function to perform the Fk function
void Fk(int* input, int* key, int* result) {
    int left[4], right[4], expandedRight[8], temp[8], sboxInput[8], sboxOutput[4], P4Output[4];
    
    // Split the input into two halves
    for (int i = 0; i < 4; i++) {
        left[i] = input[i];
        right[i] = input[i + 4];
    }
    
    // Expand and permute the right half
    permute(right, expandedRight, EP, 8);
    
    // XOR the expanded right half with the key
    XOR(expandedRight, key, temp, 8);
    
    // S-box substitution
    for (int i = 0; i < 8; i++) {
        sboxInput[i] = temp[i];
    }
    sBox(sboxInput, sboxOutput);
    
    // Apply P4 permutation
    permute(sboxOutput, P4Output, P4, 4);
    
    // XOR the result with the left half
    XOR(left, P4Output, result, 4);
    
    // Combine the result with the unchanged right half
    for (int i = 0; i < 4; i++) {
        result[i + 4] = right[i];
    }
}

// SDES encryption function
void SDES(int* input, int* key, int* output, int mode) {
    int permutedInput[8], subkeys[2][8], temp[8], result[8];
    
    // Perform initial permutation
    permute(input, permutedInput, IP, 8);
    
    // Generate subkeys
    generateKeys(key, subkeys[0], subkeys[1]);
    
    // Apply Fk function with K1 or K2 based on mode (encryption/decryption)
    Fk(permutedInput, subkeys[mode == 0 ? 0 : 1], temp);
    
    // Swap the halves
    for (int i = 0; i < 4; i++) {
        result[i] = temp[i + 4];
        result[i + 4] = temp[i];
    }
    
    // Apply Fk again with K2 or K1 based on mode
    Fk(result, subkeys[mode == 0 ? 1 : 0], temp);
    
    // Apply inverse permutation
    permute(temp, output, IP_inv, 8);
}

// CBC mode encryption and decryption
void CBC(int* plaintext, int* key, int* IV, int* ciphertext, int mode) {
    int temp[8], result[8];
    
    // XOR plaintext with IV for encryption, or ciphertext with IV for decryption
    XOR(plaintext, IV, temp, 8);
    
    // Perform encryption or decryption
    SDES(temp, key, result, mode);
    
    // Store the result
    for (int i = 0; i < 8; i++) {
        ciphertext[i] = result[i];
    }
}

// Function to print binary arrays
void printBinary(int* array, int size) {
    for (int i = 0; i < size; i++) {
        printf("%d", array[i]);
    }
    printf("\n");
}

int main() {
    int key[10] = {0, 1, 1, 1, 1, 1, 1, 1, 0, 1};       
    int IV[8] = {1, 0, 1, 0, 1, 0, 1, 0};               
    int plaintext[8] = {0, 0, 0, 0, 0, 0, 1, 1};        // Plaintext: 0000 0001 0010 0011
    int ciphertext[8];
    int decryptedtext[8];
    
    printf("Original plaintext: ");
    printBinary(plaintext, 8);
    
    // Encrypt using CBC mode
    CBC(plaintext, key, IV, ciphertext, 0);
    
    printf("Ciphertext: ");
    printBinary(ciphertext, 8);
    
    // Decrypt using CBC mode
    CBC(ciphertext, key, IV, decryptedtext, 1);
    
    printf("Decrypted plaintext: ");
    printBinary(decryptedtext, 8);
    
    return 0;
}
