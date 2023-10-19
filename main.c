
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include "table.h"
#include "foncscontroller.h"

typedef struct {
    int code;
    int key;
    char value[128];
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
        fprintf(stderr, "Erreur de pipe\n");
        exit(1);
    }

    for (int i = 0; i < N; i++) {
        if (pipe(f[i]) == -1) {
            fprintf(stderr, "Erreur de pipe\n");
            exit(1);
        }
    }

    // Création des N processus fils
    for (int i = 0; i < N; i++) {
        switch (fork()) {
            case -1:
                fprintf(stderr, "Erreur de fork\n");
                exit(1);
            case 0:
                close(f_controller[0]);
                close(f[i][0]);
                node_num = i;
                i = N;  // On quitte le for
        }
    }

    PTable_entry table = NULL;
    Data data;

    // Check if the current process is the parent only once
    if (getpid() == pid_parent) {
        
            data.code = get_command();
            if (data.code == 1) {
                data.key = get_key();
                int std = get_string_stdin(data.value, 128);
            }

            if (data.code == 0){
                printf("bye bye!\n");
                exit(0);
            }

            write(f[N - 1][1], &data, sizeof(Data));
        

        while (wait(NULL) != -1);



    } else {
       // while (1) {
            if (node_num == 0) {
                while (read(f[N - 1][0], &data, sizeof(Data)) == 0);
            } else {
                while (read(f[node_num - 1][0], &data, sizeof(Data)) == 0);
            }

            int processToStore = data.key % N;
            if (node_num == processToStore) {
                store(&table, data.key, data.value);
                printf("Données stockées dans le processus %d : clé=%d, valeur=%s\n", processToStore, data.key, data.value);
                fflush(stdout);
            }

            write(f[node_num][1], &data, sizeof(Data));
            display(table);

            if (data.code == 0) {
                exit(1);
            }
        //}
    }

    close(f_controller[0]);
    close(f_controller[1]);

    for (int i = 0; i < N; i++) {
        close(f[i][0]);
        close(f[i][1]);
    }

    return 0;
}


