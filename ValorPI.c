#include <stdio.h> // Header para operações I/O
#include <pthread.h> // Header para threads
#include <stdlib.h> // Header para a função atol()
#include <unistd.h> // Header para função getpid()
#include <time.h> // Header para função clock_gettime()
#include <math.h> // Header para a função fabs()

// o truns é um apontador para o mesmo espaço da memória que está na variável quando a chamamos.
void *get_pi(void *truns); 

// Função principal do programa onde é efetuada a contagem do tempo de execução do programa, inicializadas as threads e todo o output.
void main(int argc, char *argv[]) {
	long npoints = atol(argv[1]); // Transformar o valor recebido da terminal em long.
	int *pcount = 0, nruns, points_inside = 0, i, nthreads = atoi(argv[2]);
	double pi, elapsed, abs_pi = 3.14159265;
	float rel_err, abs_err, pre;
	struct timespec start, finish;
	
	// Iniciar a contagem do tempo de execução do programa. Utilização do tempo real é definido por CLOCK_MONOTONIC.
	clock_gettime(CLOCK_MONOTONIC, &start);

	// Criar array de thread_ids e dividir número de pontos pelo número de threads a iniciar.
	// Alocamos memória a thread_array, em que o tamanho dessa memória é o necessário para um "pthread", 
	// de seguida, multiplicamos este espaço de memória de p_thread pelo nº de threads
	// para conseguirmos fazer com que um array para cada thread_ID.
	pthread_t *thread_array;
	thread_array = malloc(nthreads * sizeof(pthread_t));
	nruns = npoints / nthreads;
		
	// Loop de iniciação de threads.
	// As mesmas irão correr a função 'get_pi' e recebem como parametro o número de pontos
	// que cada thread deve gerar.
	for (i = 0; i < nthreads; i++) {
		pthread_create(&thread_array[i], NULL, *get_pi, (void *) &nruns);
	}

	// Loop para o programa aguardar que cada thread termine. Os valores returnados
	// por cada thread serão guardados no apontador 'pcount'.
	for (i = 0; i < nthreads; i++) {
		pthread_join(thread_array[i],(void **)&pcount); 
		points_inside += *pcount;
	}

	// Descobrir o valor de PI com base nos pontos gerados pelas threads.
	pi = (double)points_inside / npoints * 4;
	
	// Calculo do erro relativo em percentagem.
	abs_err = fabs(abs_pi - pi);
	rel_err = abs_err / abs_pi; 
	pre = rel_err * 100;

	// Terminar a contagem do tempo de execução do programa e determinar o delta.
	clock_gettime(CLOCK_MONOTONIC, &finish);
	elapsed = (finish.tv_sec - start.tv_sec);
	elapsed += (finish.tv_nsec - start.tv_nsec) / 1000000000.0;

	printf("\n______________  CÁLCULO DE PI - MÉTODO DE MONTE CARLO_____________________\n");
    	printf("\nPara %ld pontos\n", npoints);
    	printf("\nNº Threads\tPontos_dentro\tAprox. Pi\tTempo\t\tErro relativo\n");

	printf("%i\t\t%i\t\t%1.8f\t%fs\t%.8g\n", nthreads, points_inside , pi, elapsed, pre);
	printf("\n\n");	
}

// Função a ser processada pelas threads. 
void *get_pi (void *truns) {
	double x, y, z;
	int i, count = 0;
	long seedx, seedy; 

	// Criar "seeds" diferentes para coordenadas X e Y, para garantirmos que são gerados números diferentes.
	seedx = pthread_self();
	seedy = pthread_self() - getpid();

	// Loop para gerar coordenadas X e Y.
	// Caso seja gerado um ponto entre 0 e 1 (inclusive), o valor de count será
	// aumentado em +1.
	int *thread_runs = (int *) truns;	
	for (i = 0; i < *thread_runs; i++) {
		x = (double)rand_r((unsigned int*)seedx)/RAND_MAX; //INT_MAX=2147483647
		y = (double)rand_r((unsigned int*)seedy)/RAND_MAX;
		z = x * x + y * y;
		if (z <= 1) count++;
	}

	// Alocar memória para apontador 'result' que irá receber um valor inteiro da
	// variavel count. Passo necessário porque variáveis locais não podem ser retornadas,
	// pois o seu espaço de memória é apagado quando a função termina.
	int* result = malloc(sizeof(int));
	*result = count;
	return (void *) result;	
}
