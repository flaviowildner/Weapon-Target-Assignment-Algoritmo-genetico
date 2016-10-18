#include "stdafx.h"
#include <stdlib.h>
#include <time.h>
#include <math.h>

struct alvo {
	float valor;
	int tipo;
};

struct arma {
	int tipo;
	struct alvo alvo;
};

struct cromossomo {
	struct arma *armas;
	float fitness;
};


int i, j, k;
int totalArmas = 0;
int numeroTipoArmas = 0, numeroAlvos = 0;
int nPopulacao = 0;
int taxaMutacao;
int iteracoes;
float floatTemp;
float tempProbabilidades;
float aux;
float somatorio = 0;
float mult = 1;
int a = 0;

struct arma *VetorArmas = NULL;
struct alvo *VetorAlvos = NULL;
float **matrizProbabilidades = NULL;

struct cromossomo *populacao = NULL;
struct cromossomo *novaPopulacao = NULL;
struct cromossomo MelhorSolucao;


float melhor_exaustivo = 999999.0;
int *melhor_exaustivo_vet = NULL;



void ordenarPopulacao() {
	int min;
	int i, j;
	struct cromossomo aux;
	for (i = 0; i < nPopulacao; i++)
	{
		min = i;
		for (j = i + 1; j < nPopulacao; j++) {
			if (populacao[j].fitness < populacao[min].fitness)
				min = j;
		}
		if (i != min) {
			aux = populacao[i];
			populacao[i] = populacao[min];
			populacao[min] = aux;
		}
	}
}

void mutacao() {
	int random;
	int i, j;


	for (i = 0; i < nPopulacao; i++) {
		for (j = 0; j < totalArmas; j++) {
			if (rand() % 100 < taxaMutacao) {

				//printf("---%d %d\n", i, j);

				random = rand() % numeroAlvos;
				while (random == populacao[i].armas[j].alvo.tipo) {
					random = rand() % numeroAlvos;
				}
				populacao[i].armas[j].alvo.tipo = random;
			}
		}
	}
}

void evaluate() {
	float mult = 0;
	float somatorio = 0;
	float aux;
	int i, j;

	for (i = 0; i < nPopulacao; i++) {
		for (j = 0; j < numeroAlvos; j++) {
			aux = VetorAlvos[j].valor;
			mult = 1;
			for (k = 0; k < totalArmas; k++) {
				if (populacao[i].armas[k].alvo.tipo == j) {
					mult = (1 - matrizProbabilidades[populacao[i].armas[k].tipo][j]) * mult;
				}
			}
			somatorio += aux * mult;
		}
		populacao[i].fitness = somatorio;
		somatorio = 0;
	}
	ordenarPopulacao();
	if (populacao[0].fitness < MelhorSolucao.fitness) {
		for (j = 0; j < totalArmas; j++) {
			MelhorSolucao.armas[j].alvo.tipo = populacao[0].armas[j].alvo.tipo;
			MelhorSolucao.fitness = populacao[0].fitness;
			i = 0;
		}
	}
}

void imprimirpopulacao(struct cromossomo *populacao) {
	for (i = 0; i < nPopulacao; i++) {
		printf("%d: ", i);
		for (j = 0; j < totalArmas; j++) {
			printf("%d ", populacao[i].armas[j].alvo.tipo);
		}
		printf("%f\n", populacao[i].fitness);
	}
	printf("\n\n");
}

void start() {
	int intTemp;
	float floatTemp;
	float tempProbabilidades;
	printf("Insira o numero de tipos de armas: ");
	scanf_s("%d", &numeroTipoArmas);

	for (i = 0; i < numeroTipoArmas; i++) {
		printf("Insira a quantidade de armas do tipo %d: ", i);
		scanf_s("%d", &intTemp);

		totalArmas += intTemp;

		VetorArmas = (struct arma*)realloc(VetorArmas, totalArmas * sizeof(struct arma));

		for (k = totalArmas - intTemp; k < totalArmas; k++) {
			VetorArmas[k].tipo = i;
		}
	}
	MelhorSolucao.armas = (struct arma*)malloc(totalArmas * sizeof(struct arma));
	intTemp = 0;


	printf("Insira o numero de alvos: ");
	scanf_s("%d", &numeroAlvos);
	VetorAlvos = (struct alvo*)malloc(numeroAlvos * sizeof(struct alvo));


	for (i = 0; i < numeroAlvos; i++) {
		printf("Insira o valor do alvo %d: ", i);
		scanf_s("%f", &floatTemp);
		VetorAlvos[i].valor = floatTemp;
		VetorAlvos[i].tipo = i;
	}


	matrizProbabilidades = (float**)malloc(numeroTipoArmas * sizeof(float*));
	for (i = 0; i < numeroTipoArmas; i++) {
		matrizProbabilidades[i] = (float*)malloc(numeroAlvos * sizeof(float));
	}


	for (i = 0; i < numeroTipoArmas; i++) {
		for (j = 0; j < numeroAlvos; j++) {
			printf("Insira a chance da arma de tipo %d destruir o alvo %d: ", i + 1, j + 1);
			scanf_s("%f", &tempProbabilidades);
			matrizProbabilidades[i][j] = tempProbabilidades;
			//matrizProbabilidades[i][j] = ((float)rand() / (float)(RAND_MAX));
		}
	}
}

void iniciarpopulacao() {
	populacao = (struct cromossomo*)malloc(nPopulacao * sizeof(struct cromossomo));
	for (i = 0; i < nPopulacao; i++) {
		populacao[i].armas = (struct arma*)malloc(totalArmas * sizeof(struct arma));
		for (j = 0; j < totalArmas; j++) {
			populacao[i].armas[j].tipo = VetorArmas[j].tipo;
			populacao[i].armas[j].alvo = VetorAlvos[rand() % numeroAlvos];
		}
	}

	novaPopulacao = (struct cromossomo*)malloc(nPopulacao * sizeof(struct cromossomo));
	for (i = 0; i < nPopulacao; i++) {
		novaPopulacao[i].armas = (struct arma*)malloc(totalArmas * sizeof(struct arma));
		for (j = 0; j < totalArmas; j++) {
			novaPopulacao[i].armas[j].tipo = VetorArmas[j].tipo;
		}
	}
}

void crossover() {
	int selecionado1, selecionado2 = -1;
	float faixa, random, random2;
	int corte = 0;
	float total = 0;

	for (i = 0; i < nPopulacao; i++) {
		total += populacao[i].fitness;
	}
	total = ceil(total);
	//imprimirpopulacao(populacao);

	for (i = 0; i < nPopulacao; i++) {


		//SELECAO DOS PAIS
		random = rand() % (int)total;
		random2 = rand() % (int)total;
		corte = (rand() % (totalArmas - 1)) + 1;


		faixa = 0;
		for (selecionado1 = 0; selecionado1 < nPopulacao; selecionado1++) {
			faixa += populacao[selecionado1].fitness;
			if (faixa > random) {
				break;
			}
		}



		faixa = 0;
		for (selecionado2 = 0; selecionado2 < nPopulacao; selecionado2++) {
			faixa += populacao[selecionado2].fitness;
			if (random2 < faixa) {
				if (selecionado1 == selecionado2)
					faixa += populacao[selecionado2].fitness;
				else
					break;
			}
		}

		selecionado1 = abs(selecionado1 - (nPopulacao - 1));
		selecionado2 = abs(selecionado2 - (nPopulacao - 1));
		//printf("%d %d %d\n", selecionado1, selecionado2, corte);



		//REPRODUCAO
		for (j = 0; j < corte; j++) {
			novaPopulacao[i].armas[j].alvo.tipo = populacao[selecionado1].armas[j].alvo.tipo;
		}
		for (j = corte; j < totalArmas; j++) {
			novaPopulacao[i].armas[j].alvo.tipo = populacao[selecionado2].armas[j].alvo.tipo;
		}


	}
	//imprimirpopulacao(novaPopulacao);


	for (i = 0; i < nPopulacao; i++) {
		for (j = 0; j < totalArmas; j++) {
			populacao[i].armas[j].alvo.tipo = novaPopulacao[i].armas[j].alvo.tipo;
		}
	}
}

int proxima(int a[], int N, int M) {
	int t = N - 1;
	while (t >= 0) {
		a[t] = (a[t] + 1) % M;
		if (a[t] == 0) t--;
		else return 0;
	}
	return -1;
}

float evaluate_exaustivo(int *v) {
	float mult = 1;
	float somatorio = 0;
	float aux;
	int i, j;
	for (j = 0; j < numeroAlvos; j++) {
		aux = VetorAlvos[j].valor;
		mult = 1;
		for (k = 0; k < totalArmas; k++) {
			if (v[k] == j) {
				mult = (1 - matrizProbabilidades[VetorArmas[k].tipo][j]) * mult;
			}
		}
		somatorio += aux * mult;
	}
	return somatorio;
}

void imp_seq_n_base_m(int seq[], int n, int m) {
	int i;
	int k;
	float curr = 99999;
	for (i = 0; i<n; i++) seq[i] = 0;
	do {
		curr = evaluate_exaustivo(seq);
		if (curr < melhor_exaustivo) {
			melhor_exaustivo = curr;
			for (k = 0; k < totalArmas; k++) {
				melhor_exaustivo_vet[k] = seq[k];
			}
		}
	} while (proxima(seq, n, m) == 0);
}

int main()
{
	int i, j;
	int msec;
	srand((unsigned)time(NULL));


	iteracoes = 100;
	nPopulacao = 80;
	taxaMutacao = 5;
	MelhorSolucao.fitness = 9999999;

	clock_t time_start;
	clock_t time_end;
	clock_t diff;



	start();
	iniciarpopulacao();

	//time_start = clock();
	evaluate();
	for (i = 0; i < iteracoes; i++) {
		crossover();
		mutacao();
		evaluate();
	}
	//time_end = clock();


	printf("\nResultado: %f\n", MelhorSolucao.fitness);
	for (i = 0; i < totalArmas; i++) {
		printf("%d ", MelhorSolucao.armas[i].alvo.tipo);
	}
	printf("\n");

	/*diff = time_end - time_start;
	msec = diff * 1000 / CLOCKS_PER_SEC;
	printf("\nTime1 taken %d seconds %d milliseconds\n", msec / 1000, msec % 1000);*/


	//EXAUSTIVO
	/*time_start = clock();
	exaustivo();
	time_end = clock();


	diff = time_end - time_start;
	msec = diff * 1000 / CLOCKS_PER_SEC;
	printf("\nTime2 taken %d seconds %d milliseconds\n", msec / 1000, msec % 1000);*/

	return 0;
}