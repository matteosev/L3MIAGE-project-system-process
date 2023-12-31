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
			fprintf(stderr, "Erreur de pipe\n");
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

	// Le parent transmet l'entier au node n°0 en utilisant le pipe n°N-1 (dernier)
	if (getpid() == pid_parent) {
		int data = 2456;
		write(f[N - 1][1], &data, sizeof(int));

		while(wait(NULL) != -1);

		printf("Tous les fils créés et morts\n");
	}
	// Le node i
	//		- attend de recevoir l'entier du node i-1 (exception : 0 attend N-1)
	//		- transmet l'entier au node i+1 (exception : N-1 transmet à 0)
	else {
		int data;

		if (node_num == 0)
			while(read(f[N - 1][0], &data, sizeof(int)) == 0);
		else
			while(read(f[node_num - 1][0], &data, sizeof(int)) == 0);

		data++;
		write(f[node_num][1], &data, sizeof(int));
		printf("Fils n°%d - entier transmis = %d\n", node_num, data);
		fflush(stdout);
	}

	close(f_controller[0]);
	close(f_controller[1]);

	for (int i = 0; i < N; i++) {
		close(f[i][0]);
		close(f[i][1]);
	}

	return 0;
}
