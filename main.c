
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include "table/table.h"
#include "foncscontroller.h"

typedef struct {
    int code;
    int key;
    char value[129];
} Data;

int node_num;

int main(int argc, char **argv) {
    if (argc < 2) {
        fprintf(stderr, "Pas de paramètre N (nombre de processus fils)\n");
        exit(1);
    }

    int N = atoi(argv[1]);
    int pid_parent = getpid();

    // Declare the pipes
    int f_controller[2];
    int f[N][2];

    if (pipe(f_controller) == -1) {
        perror("Erreur de pipe\n");
        exit(1);
    }

    for (int i = 0; i < N; i++) {
        if (pipe(f[i]) == -1) {
            perror("Erreur de pipe\n");
            exit(1);
        }
    }

    // Création des N processus fils
    for (int i = 0; i < N; i++) {
        switch (fork()) {
            case -1:
                perror("Erreur de fork\n");
                exit(1);
            case 0:
                close(f_controller[0]);
                close(f[i][0]);
                node_num = i;
                i = N;  // On quitte le for
        }
    }

    PTable_entry table = NULL;
    Data data = {0, 0, ""};

    // Check if the current process is the parent only once
    if (getpid() == pid_parent) {
		while(1) {
			printf("ok\n");
            printf("%ld\n", read(f_controller[0], &data, sizeof(Data)));
				printf("Réponse reçue\n");

            data.code = get_command();

			switch(data.code) {
				case 0:
					break;
				case 1:
                	data.key = get_key();
                	get_string_stdin(data.value, 128);
					break;
				case 2:
                	data.key = get_key();
					break;
				case 3:
					break;
				default:
					continue;
			}
            write(f[N - 1][1], &data, sizeof(Data));
		}
		//while (wait(NULL) != -1);
    } else {
		while (1) {
            if (node_num == 0)
               	read(f[N - 1][0], &data, sizeof(Data));
            else
               	read(f[node_num - 1][0], &data, sizeof(Data));

			switch(data.code) {
				case 0:
					break;
				case 1:
            		if (node_num == data.key % N)
                		store(&table, data.key, data.value);
                		//printf("Données stockées dans le processus %d : clé=%d, valeur=%s\n", processToStore, data.key, data.value);
					break;
				case 2:
            		if (node_num == data.key % N)
                		lookup(table, data.key);
					write(f_controller[1], &data, sizeof(Data));
					break;
				case 3:
			}
            write(f[node_num][1], &data, sizeof(Data));
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




