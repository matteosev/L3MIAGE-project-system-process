#include <stdlib.h>
#include <stdio.h>
#include "table/table.h"

int main(int argc, char *argv[]) {
	PTable_entry te = (PTable_entry) malloc(sizeof(Table_entry));
	
	for (int i = 0; i < 10; i++) {
		// Initialisation de la string à stocker
		char s[i + 2];
		s[i] = '\0';
		s[i + 1] = '\0';

		// Construction de la string
		for (int y = 0; y < i; y++)
			s[y] = '0';

		// Stockage de la string
		store(&te, i, s);
	}
	
	display(te);

	printf("table[%d] = %s\n", 2, lookup(te, 2));
	printf("table[%d] = %s\n", 100, lookup(te, 100));
}
