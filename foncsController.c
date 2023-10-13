#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include "table.h"


int valprocess(int N , int K){
	return N % K; 

}
int get_command()
{
	printf("Saisir commande (0 = exit, 1 = set, 2 = lookup, 3 = dump):\n");
	int command;
	scanf("%d",command);
	if (d!=0 & )
	return command;
}
int get_key()
{
	printf("Saisir la cle (decimal number):\n");
	int key;
	scanf("%d",key);
	return key ;
}

int get_string_stdin(char *s, int n) {
	int char_input;
	int i = 0;
	
	while (i < n && (char_input = getchar()) != '\n' && char_input != EOF) {
		s[i] = (char)char_input;
		i++;
	}

	return i;
}


