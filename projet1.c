#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main(int argc, char *argv[]) {
	if (argc < 2) {
		fprintf(stderr, "Pas de paramètre N (nombre de processus fils)\n");
		exit(1);
	}

	int N = atoi(argv[1]);
	int pid_parent = getpid();
	int node_num;

	int f_controller[2];
	int f[N][2];

	if (pipe(f_controller) == -1) {
		fprintf(stderr, "Erreur de pipe\n");
		exit(1);
	}

	// Le pipe i relie le node i au node i+1
	// Exception : le pipe N-1 relie le node N-1 au node 0
	for (int i = 0; i < N; i++) {
		if (pipe(f[i]) == -1) {
			fprintf(stderr, "Erreur de pipe\n", i);
			exit(1);
		}
	}

	// Création des N processus fils
	for (int i = 0; i < N; i++) {
		switch(fork()) {
			case -1:
				fprintf(stderr, "Erreur de fork\n"); exit(1);
			case 0:
				close(f_controller[0]);
				close(f[i][0]);
				node_num = i;
				i = N;	// On quitte le for
		}
	}

	if (getpid() == pid_parent) {
		while(wait(NULL) != -1)
			;

		printf("Tous les fils créés et morts\n");
	}
	else {
		printf("Fils pid %d n°%d\n", getpid(), node_num);
	}

	close(f_controller[0]);
	close(f_controller[1]);

	for (int i = 0; i < N; i++) {
		close(f[i][0]);
		close(f[i][1]);
	}

	return 0;
}
