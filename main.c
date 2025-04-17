#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#define MAX_TERMS 16   // Maximum number of terms
#define MAX_LEN 10     // Maximum length of a binary string

// Function to convert a decimal number to a binary string
void DecimalToBinary(int num, int num_vars, char *binary) {
    for (int i = num_vars - 1; i >= 0; i--) {
        binary[num_vars - 1 - i] = (num & (1 << i)) ? '1' : '0';  // Convert each bit
    }
    binary[num_vars] = '\0';  // Null-terminate the string
}

// Function to check if two minterms differ by exactly one bit
bool CheckMintermDifference(char *minterm1, char *minterm2) {
    int diff_count = 0;
    for (int i = 0; i < strlen(minterm1); i++) {
        if (minterm1[i] != minterm2[i]) diff_count++;  // Count differing bits
    }
    return diff_count == 1;  // Return true if exactly one bit differs
}

// Function to merge two minterms by replacing the differing bit with '-'
void MergeMinterms(char *minterm1, char *minterm2, char *mergedMinterm) {
    for (int i = 0; i < strlen(minterm1); i++) {
        mergedMinterm[i] = (minterm1[i] != minterm2[i]) ? '-' : minterm1[i];  // Replace differing bit with '-'
    }
    mergedMinterm[strlen(minterm1)] = '\0';  // Null-terminate the string
}

// Function to generate prime implicants
void getPrimeImplicants(char minterms[][MAX_LEN], int minterm_count, char primeImplicants[][MAX_LEN], int *prime_count) {
    bool merged[MAX_TERMS] = {false};  // Array to track merged minterms
    char mergedMinterm[MAX_LEN];       // Array to store merged minterms
    int new_prime_count = 0;

    // Compare each pair of minterms
    for (int i = 0; i < minterm_count; i++) {
        for (int j = i + 1; j < minterm_count; j++) {
            if (CheckMintermDifference(minterms[i], minterms[j])) {
                MergeMinterms(minterms[i], minterms[j], mergedMinterm);  // Merge if they differ by one bit
                bool found = false;
                for (int k = 0; k < new_prime_count; k++) {
                    if (strcmp(primeImplicants[k], mergedMinterm) == 0) found = true;  // Check for duplicates
                }
                if (!found) strcpy(primeImplicants[new_prime_count++], mergedMinterm);  // Add unique merged minterms
                merged[i] = merged[j] = true;
            }
        }
    }
    // Add unmerged minterms to prime implicants
    for (int i = 0; i < minterm_count; i++) {
        if (!merged[i]) strcpy(primeImplicants[new_prime_count++], minterms[i]);
    }
    *prime_count = new_prime_count;  // Update the count of prime implicants
}

// Function to check if a prime implicant matches a minterm
bool matches(char *pattern, char *minterm) {
    for (int i = 0; i < strlen(pattern); i++) {
        if (pattern[i] != '-' && pattern[i] != minterm[i]) return false;  // Check if all bits match
    }
    return true;
}

// Function to create the prime implicant chart
void CreatePrimeImplicantChart(char primeImplicants[][MAX_LEN], int prime_count, char minterms[][MAX_LEN], int minterm_count, char chart[][MAX_TERMS]) {
    for (int i = 0; i < prime_count; i++) {
        for (int j = 0; j < minterm_count; j++) {
            chart[i][j] = matches(primeImplicants[i], minterms[j]) ? '1' : '0';  // Mark the chart if the prime implicant matches the minterm
        }
    }
}

// Function to find essential prime implicants
void FindEssentialPrimeImplicants(char chart[][MAX_TERMS], int prime_count, int minterm_count, bool essential[]) {
    for (int j = 0; j < minterm_count; j++) {
        int count = 0, essential_index = -1;
        for (int i = 0; i < prime_count; i++) {
            if (chart[i][j] == '1') {
                count++;
                essential_index = i;
            }
        }
        if (count == 1) essential[essential_index] = true;  // Mark the prime implicant as essential if it covers only one minterm
    }
}

// Function to convert a binary prime implicant to a Boolean expression
void ConvertToBooleanExpression(char *primeImplicant, char *expression, int num_vars) {
    char variables[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    for (int i = 0; i < num_vars; i++) {
        if (primeImplicant[i] == '0') { strncat(expression, &variables[i], 1); strcat(expression, "'"); }
        else if (primeImplicant[i] == '1') strncat(expression, &variables[i], 1);
    }
}

// Function to print the simplified Boolean function
void SimplifyBooleanFunction(char primeImplicants[][MAX_LEN], int prime_count, int num_vars) {
    printf("Simplified Boolean Function:\n");
    for (int i = 0; i < prime_count; i++) {
        char expression[MAX_LEN] = "";
        ConvertToBooleanExpression(primeImplicants[i], expression, num_vars);
        if (i != 0) printf(" + ");
        printf("%s", expression);
    }
    printf("\n");
}

// Function to print the simplified Boolean function with essential prime implicants
void SimplifyEssentialPrimeImplicants(char primeImplicants[][MAX_LEN], bool essential[], int prime_count, int num_vars) {
    printf("Simplified Boolean Function with Essential Prime Implicants:\n");
    bool first = true;
    for (int i = 0; i < prime_count; i++) {
        if (essential[i]) {
            char expression[MAX_LEN] = "";
            ConvertToBooleanExpression(primeImplicants[i], expression, num_vars);
            if (!first) printf(" + ");
            printf("%s", expression);
            first = false;
        }
    }
    printf("\n");
}

int main() {
    int minterm_count, prime_count = 0, num_vars;
    char minterms[MAX_TERMS][MAX_LEN], primeImplicants[MAX_TERMS][MAX_LEN], chart[MAX_TERMS][MAX_TERMS] = {{0}};
    bool essential[MAX_TERMS] = {false};
    int decimal_minterms[MAX_TERMS];

    // Read the number of variables and minterms
    printf("Enter the number of variables: ");
    scanf("%d", &num_vars);
    printf("Enter the number of minterms: ");
    scanf("%d", &minterm_count);
    for (int i = 0; i < minterm_count; i++) {
        printf("Enter decimal minterm %d: ", i + 1);
        scanf("%d", &decimal_minterms[i]);
        if (decimal_minterms[i] < 0) { printf("Invalid input: Negative numbers are not allowed.\n"); return 1; }
        DecimalToBinary(decimal_minterms[i], num_vars, minterms[i]);  // Convert to binary
    }

    // Generate prime implicants and create the prime implicant chart
    getPrimeImplicants(minterms, minterm_count, primeImplicants, &prime_count);
    CreatePrimeImplicantChart(primeImplicants, prime_count, minterms, minterm_count, chart);

    // Print prime implicants and the chart
    printf("Prime Implicants:\n");
    for (int i = 0; i < prime_count; i++) printf("%s\n", primeImplicants[i]);
    printf("Prime Implicant Chart:\n");
    for (int i = 0; i < prime_count; i++) {
        for (int j = 0; j < minterm_count; j++) printf("%c ", chart[i][j]);
        printf("\n");
    }

    // Find essential prime implicants and print simplified Boolean functions
    FindEssentialPrimeImplicants(chart, prime_count, minterm_count, essential);
    SimplifyBooleanFunction(primeImplicants, prime_count, num_vars);
    SimplifyEssentialPrimeImplicants(primeImplicants, essential, prime_count, num_vars);

    return 0;
}
