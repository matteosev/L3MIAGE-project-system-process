#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>
#include "table/table.h"

#define VALUE_MAX_LENGTH (128)
#define perror_exit(code, msg) do { perror(msg); exit(code); } while(0)

typedef struct {
    int code;
    int key;
    char value[129];
} Data;

// Lis au maximum n caractères depuis l'entrée standard
// Paramètres :
//	char *s : un pointeur vers la première case d'un tableau de taille n+1
//	int n : le nombre maximum de caractères à lire
// Note :
//	taille du tableau = n+1 -> 1 char réservé pour le zéro terminal
int get_string_stdin(char *s, int n) {
	int c;	// int et pas char => googler why-is-getchar-function-in-c-an-integer
	int i = 0;

	while ((c = getchar()) != '\n' && c != EOF);

	while (i < n && (c = getchar()) != '\n' && c != EOF)
		s[i++] = (char)c;

	s[i] = '\0';

	return i;
}

int main(int argc, char **argv) {

    if (argc < 2)
		perror_exit(1, "Pas de paramètre N (nombre de processus fils)\n");

    const int N = atoi(argv[1]);
    const int PID_PARENT = getpid();

	int node_num;
    int f_controller[2];
    int f[N][2];

    if (pipe(f_controller) == -1)
        perror_exit(1, "Erreur de pipe\n");

    for (int i = 0; i < N; i++) {
        if (pipe(f[i]) == -1)
            perror_exit(1, "Erreur de pipe\n");
    }

    // Création des N processus fils
    for (int i = 0; i < N; i++) {
        switch (fork()) {
            case -1:
                perror_exit(1, "Erreur de fork\n");
            case 0:
				close(f_controller[0]);
                close(f[i][0]);
				// On mémorise le numéro du node (ex: N=4 => 0 ou 1 ou 2 ou 3)
                node_num = i;
				  // On quitte le for
                i = N;
		}
    }

    // Controller
    if (getpid() == PID_PARENT) {
		while(1) {
    		Data data = {0, 0, ""};
			
			printf("Saisir commande (0 = exit, 1 = set, 2 = lookup, 3 = dump):\n");
			scanf("%d", &data.code);

			if (data.code == 1 || data.code == 2) {
				printf("Saisir la cle (decimal number): ");
				scanf("%d", &data.key);
			}

			switch(data.code) {

				case 0:
					for (int i = 0; i < N; i++)
						write(f[i][1], &data, sizeof(Data));

					while (wait(NULL) != -1);

					printf("bye bye!\n");
					exit(0);

				case 1:
					printf("Saisir la valeur (chaine de caracteres, max %d chars): ", VALUE_MAX_LENGTH);
                	get_string_stdin(data.value, VALUE_MAX_LENGTH);
            		write(f[N - 1][1], &data, sizeof(Data));
					break;

				case 2:
            		write(f[N - 1][1], &data, sizeof(Data));
					read(f_controller[0], &data, sizeof(Data));
					printf("La valeur recherchée : %s \n", data.value);
					break;

				case 3:
					write(f[N-1][1], &data, sizeof(Data));
					read(f_controller[0], &data, sizeof(Data));

					for (int i = 0; i < N - 1; i++) {
						write(f[i][1], &data, sizeof(Data));
						read(f_controller[0], &data, sizeof(Data));
					}

				default:
                    break;
			}
		}
    }
	// Node
	else {
    	PTable_entry table = NULL;
		while (1) {
    		Data data = {0, 0, ""};

            read(f[(node_num == 0 ? N : node_num) - 1][0], &data, sizeof(Data));

			switch(data.code) {

				case 0:
					exit(0);

				case 1:
            		if (node_num == data.key % N)
                		store(&table, data.key, data.value);
					else
            			write(f[node_num][1], &data, sizeof(Data));
					break;

				case 2:
            		if (node_num == data.key % N) {
						char *value = lookup(table, data.key);
                        strcpy(data.value, value == NULL ? "n'est pas trouvée" : value);
						write(f_controller[1], &data, sizeof(Data));
					} else
            			write(f[node_num][1], &data, sizeof(Data));
					break;

				case 3:
					if (table !=NULL)
                    	printf("process %d :\n",getpid());
					display(table);
					write(f_controller[1], &data, sizeof(Data));

				default :
                    break ;
			}
        }
    }

    close(f_controller[0]);
    close(f_controller[1]);

    for (int i = 0; i < N; i++) {
        close(f[i][0]);
        close(f[i][1]);
    }

    return 0;
}
