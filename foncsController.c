#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include "table/table.h"
#include "foncscontroller.h"



int numprocess(int N , int K){
	return N % K; 

}
int get_command() {
    printf("Saisir commande (0 = exit, 1 = set, 2 = lookup, 3 = dump):\n");
    int command;
    scanf("%d", &command); // Pass the address of 'command'
    return command;
}

int get_key() {
    printf("Saisir la cle (decimal number):\n");
    int key;
    scanf("%d", &key); // Pass the address of 'key'
    return key;
}

int get_string_stdin(char *s, int n) {
    printf("Saisir la valeur (chaine de caracteres, max %d chars): ", n - 1);

    int char_input;
    int i = 0;

    // Clear the input buffer before reading the value
    int c;
    while ((c = getchar()) != '\n' && c != EOF);

    while (i < n - 1 && (char_input = getchar()) != '\n' && char_input != EOF) {
        s[i] = (char)char_input;
        i++;
    }
    s[i] = '\0'; // Null-terminate the string

    return i;
}
