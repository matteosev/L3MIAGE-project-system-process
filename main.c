#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include "table/table.h"
#include "foncscontroller.h"
#include <string.h>

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

    // Check if the current process is the parent only once
    if (getpid() == pid_parent) {
		while(1) {
    		Data data = {0, 0, ""};
            data.code = get_command();

			switch(data.code) {
				case 0:
					for (int i = 0; i < N; i++)
						write(f[i][1], &data, sizeof(Data));
					while (wait(NULL) != -1);
					printf("bye bye!\n");
					exit(0);
				case 1:
                	data.key = get_key();
                	get_string_stdin(data.value, 128);
            		write(f[N - 1][1], &data, sizeof(Data));
					break;
				case 2:
                	data.key = get_key();
            		write(f[N - 1][1], &data, sizeof(Data));
					read(f_controller[0], &data, sizeof(Data));
					printf("Valeur trouvée = %s \n", data.value);
					break;
				case 3:
					write(f[N-1][1], &data, sizeof(Data));
					read(f_controller[0], &data, sizeof(Data));
					for (int i = 0; i < N - 1; i++) {
						write(f[i][1], &data, sizeof(Data));
						read(f_controller[0], &data, sizeof(Data));
					}
				default:
			}
		}
    } else {
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
                        strcpy(data.value, value == NULL ? "" : value);
						write(f_controller[1], &data, sizeof(Data));
					} else
            			write(f[node_num][1], &data, sizeof(Data));
					break;
				case 3:
					display(table);
					write(f_controller[1], &data, sizeof(Data));
				default :
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
