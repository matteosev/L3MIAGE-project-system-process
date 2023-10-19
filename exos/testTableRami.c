#include <stdio.h>
#include <stdlib.h>
#include "table.h"
int main(int argc, char *argv[]) {
	PTable_entry tab = (PTable_entry) malloc(sizeof(Table_entry));
	for(int i=1;i<argc;i++)
		{
			store(&tab,i,argv[i]);
		}
	display(tab);
	printf("valeur existante %s \n" ,lookup(tab,1));
	printf("valeur absente %S \n" ,lookup(tab,7));
	
	return 0 ; 

}
