#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main(int argc, char *argv[]) {
    if (argc < 2)
        exit(1);

    int N = atoi(argv[1]);
    
    for (int i = 0; i < N; i++) {
        switch(fork()) {
            case -1:
                fprintf(stderr, "Erreur de fork\n"); exit(1);
            case 0:
                printf("Fils pid %d\n", getpid());
                exit(0);
        }
    }
    while(wait(NULL) != -1);
    printf("Tous les fils créés et morts\n");
    fflush(stdout);
    return 0;
}
