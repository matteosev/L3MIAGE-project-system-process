#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Pas de paramètre N (nombre de processus fils)");
        exit(1);
    }

    int N = atoi(argv[1]);
    
    int f_controller[2];
    int f[N][2];

    if (pipe(f_controller) == -1) {
        fprintf(stderr, "Erreur de pipe\n");
        exit(1);
    }
    
    for (int i = 0; i < N; i++) {

        // Le processus i initialise le pipe qui le relie au processus i+1
        // Le dernier processus ne communique avec aucun autre processus (i < N-1)
        if (i < N-1 && pipe(f[i]) == -1) {
            fprintf(stderr, "Erreur de pipe au processus n°%d\n", i);
            exit(1);
        }

        switch(fork()) {
            case -1:
                fprintf(stderr, "Erreur de fork\n"); exit(1);
            case 0:
                close(f_controller[0]);
                close(f[i][1]);

                // Le processus ferme le pipe de son prédécesseur en écriture
                if (i > 0)
                    close(f[i - 1][1]);

                printf("Fils pid %d\n", getpid());
                close(f_controller[1]);

                if (i > 0)
                    close(f[i - 1][0]);
            default:
                close(f_controller[1]);

                // Le processus père ferme tous les pipe fils en lecture
                for (int y = 0; y < N; y++)
                    close(f[y][0]);

                while(wait(NULL) != -1);

                printf("Tous les fils créés et morts\n");

                for (int y = 0; y < N; y++)
                    close(f[y][1]);

                close(f_controller[0]);
        }
    }
    return 0;
}
