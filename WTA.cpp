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
int numeroTotalArmas = 0;
int numeroTipoArmas = 0, numeroAlvos = 0;
int nPopulacao = 0;
int taxaMutacao;
int iteracao;
int maxIteracoes;
float pontoBuscaLocal = 1;
int tempoLimite;
int criterioParada;



struct arma *VetorArmas = NULL;
struct alvo *VetorAlvos = NULL;
float **matrizProbabilidades = NULL;

struct cromossomo *populacao = NULL;
struct cromossomo *novaPopulacao = NULL;
struct cromossomo melhorSolucao;



void ordenarPopulacao() {
	int nPopulacaoTemp = nPopulacao;
	int i = nPopulacao / 2, pai, filho;
	struct cromossomo aux;
	for (;;) {
		if (i > 0) {
			i--;
			aux = populacao[i];
		}
		else {
			nPopulacaoTemp--;
			if (nPopulacaoTemp == 0) return;
			aux = populacao[nPopulacaoTemp];
			populacao[nPopulacaoTemp] = populacao[0];
		}
		pai = i;
		filho = i * 2 + 1;
		while (filho < nPopulacaoTemp) {
			if ((filho + 1 < nPopulacaoTemp) && (populacao[filho + 1].fitness > populacao[filho].fitness))
				filho++;
			if (populacao[filho].fitness > aux.fitness) {
				populacao[pai] = populacao[filho];
				pai = filho;
				filho = pai * 2 + 1;
			}
			else {
				break;
			}
		}
		populacao[pai] = aux;
	}
}

void mutacao() {
	int random;
	int i, j;


	for (i = 0; i < nPopulacao; i++) {
		for (j = 0; j < numeroTotalArmas; j++) {
			if (rand() % 100 < taxaMutacao) {

				random = rand() % numeroAlvos;
				while (random == populacao[i].armas[j].alvo.tipo) {
					random = rand() % numeroAlvos;
				}
				populacao[i].armas[j].alvo.tipo = random;
			}
		}
	}
}

float avaliar(cromossomo &populacao) {
	float mult = 0;
	float somatorio = 0;
	float aux;


	for (j = 0; j < numeroAlvos; j++) {
		aux = VetorAlvos[j].valor;
		mult = 1;

		for (k = 0; k < numeroTotalArmas; k++) {
			if (populacao.armas[k].alvo.tipo == j) {
				mult = (1 - matrizProbabilidades[populacao.armas[k].tipo][j]) * mult;
			}
		}
		somatorio += aux * mult;

	}
	populacao.fitness = somatorio;
	return somatorio;
}

void avaliar_populacao() {
	for (i = 0; i < nPopulacao; i++)
		avaliar(populacao[i]);

	ordenarPopulacao();
	if (populacao[0].fitness < melhorSolucao.fitness) {
		melhorSolucao = populacao[0];
		iteracao = 0;
		for (k = 0; k < numeroTotalArmas; k++) {
			printf_s("%1d ", melhorSolucao.armas[k].alvo.tipo);
		}
		printf_s("\t%f\n", melhorSolucao.fitness);
	}
}

void buscal_local() {
	int i, j, k;
	struct cromossomo temp;
	temp.armas = (struct arma*)malloc(numeroTotalArmas * sizeof(struct arma));
	for (i = 0; i < numeroTotalArmas; i++)
		temp.armas[i].tipo = VetorArmas[i].tipo;

	for (i = 0; i < nPopulacao; i++) {
		for (k = 0; k < numeroTotalArmas; k++)
			temp.armas[k].alvo.tipo = populacao[i].armas[k].alvo.tipo;
		for (j = 0; j < numeroTotalArmas; j++) {
			for (k = 0; k < numeroAlvos; k++) {
				temp.armas[j].alvo.tipo = k;
				if (avaliar(temp) < populacao[i].fitness) {
					populacao[i].armas[j].alvo.tipo = k;
					avaliar(populacao[i]);
				}
			}
			temp.armas[j].alvo.tipo = populacao[i].armas[j].alvo.tipo;
		}
	}
}

void imprimirpopulacao(struct cromossomo *populacao) {
	for (i = 0; i < nPopulacao; i++) {
		printf("%d: ", i);
		for (j = 0; j < numeroTotalArmas; j++) {
			printf("%d ", populacao[i].armas[j].alvo.tipo);
		}
		printf("%f\n", populacao[i].fitness);
	}
	printf("\n\n");
}

void start(FILE *in_file) {
	int intTemp;
	float floatTemp;
	float tempProbabilidades;
	//printf("Insira o numero de tipos de armas: ");
	fscanf_s(in_file, "%d", &numeroTipoArmas);

	for (i = 0; i < numeroTipoArmas; i++) {
		//printf("Insira a quantidade de armas do tipo %d: ", i);
		fscanf_s(in_file, "%d", &intTemp);

		numeroTotalArmas += intTemp;

		VetorArmas = (struct arma*)realloc(VetorArmas, numeroTotalArmas * sizeof(struct arma));

		for (k = numeroTotalArmas - intTemp; k < numeroTotalArmas; k++) {
			VetorArmas[k].tipo = i;
		}
	}
	melhorSolucao.armas = (struct arma*)malloc(numeroTotalArmas * sizeof(struct arma));
	intTemp = 0;


	//printf("Insira o numero de alvos: ");
	fscanf_s(in_file, "%d", &numeroAlvos);
	VetorAlvos = (struct alvo*)malloc(numeroAlvos * sizeof(struct alvo));


	for (i = 0; i < numeroAlvos; i++) {
		//printf("Insira o valor do alvo %d: ", i);
		fscanf_s(in_file, "%f", &floatTemp);
		VetorAlvos[i].valor = floatTemp;
		VetorAlvos[i].tipo = i;
	}


	matrizProbabilidades = (float**)malloc(numeroTipoArmas * sizeof(float*));
	for (i = 0; i < numeroTipoArmas; i++) {
		matrizProbabilidades[i] = (float*)malloc(numeroAlvos * sizeof(float));
	}


	for (i = 0; i < numeroTipoArmas; i++) {
		for (j = 0; j < numeroAlvos; j++) {
			//printf("Insira a chance da arma de tipo %d destruir o alvo %d: ", i + 1, j + 1);
			fscanf_s(in_file, "%f", &tempProbabilidades);
			matrizProbabilidades[i][j] = tempProbabilidades;
			//matrizProbabilidades[i][j] = ((float)rand() / (float)(RAND_MAX));
		}
	}
}

void iniciarpopulacao() {
	populacao = (struct cromossomo*)malloc(nPopulacao * sizeof(struct cromossomo));
	for (i = 0; i < nPopulacao; i++) {
		populacao[i].armas = (struct arma*)malloc(numeroTotalArmas * sizeof(struct arma));
		for (j = 0; j < numeroTotalArmas; j++) {
			populacao[i].armas[j].tipo = VetorArmas[j].tipo;
			populacao[i].armas[j].alvo = VetorAlvos[rand() % numeroAlvos];
		}
	}

	novaPopulacao = (struct cromossomo*)malloc(nPopulacao * sizeof(struct cromossomo));
	for (i = 0; i < nPopulacao; i++) {
		novaPopulacao[i].armas = (struct arma*)malloc(numeroTotalArmas * sizeof(struct arma));
		for (j = 0; j < numeroTotalArmas; j++) {
			novaPopulacao[i].armas[j].tipo = VetorArmas[j].tipo;
		}
	}
	avaliar_populacao();
}

void crossover() {
	int selecionado1, selecionado2 = -1;
	float faixa = 0, random = 0, random2 = 0;
	int corte = 0;
	float total = 0;
	int over = 1;

	for (i = 0; i < nPopulacao; i++) {
		total += populacao[i].fitness;
	}

	if (total > RAND_MAX)
		over += (int)(total / RAND_MAX);

	for (i = 0; i < nPopulacao; i++) {

		//SELECAO DOS PAIS
		corte = (rand() % (numeroTotalArmas - 1)) + 1;


		for (j = 0; j < over; j++)
			random += (float)rand();

		random /= (over * RAND_MAX / total);
		faixa = 0;
		for (selecionado1 = 0; selecionado1 < nPopulacao; selecionado1++) {
			faixa += populacao[selecionado1].fitness;
			if (random < faixa) {
				break;
			}
		}


		do {
			random2 = 0;
			for (j = 0; j < over; j++)
				random2 += (float)rand();

			random2 /= (over * RAND_MAX / total);
			faixa = 0;
			for (selecionado2 = 0; selecionado2 < nPopulacao; selecionado2++) {
				faixa += populacao[selecionado2].fitness;
				if (random2 < faixa) {
					break;
				}
			}
		} while (selecionado1 == selecionado2);

		selecionado1 = abs(selecionado1 - (nPopulacao - 1));
		selecionado2 = abs(selecionado2 - (nPopulacao - 1));
		////////////////////////


		//REPRODUCAO
		for (j = 0; j < corte; j++) {
			novaPopulacao[i].armas[j].alvo.tipo = populacao[selecionado1].armas[j].alvo.tipo;
		}
		for (j = corte; j < numeroTotalArmas; j++) {
			novaPopulacao[i].armas[j].alvo.tipo = populacao[selecionado2].armas[j].alvo.tipo;
		}
		/////////////////

	}


	for (i = 0; i < nPopulacao; i++) {
		for (j = 0; j < numeroTotalArmas; j++) {
			populacao[i].armas[j].alvo.tipo = novaPopulacao[i].armas[j].alvo.tipo;
		}
	}
}

int proxima(cromossomo &seq, int N, int M) {
	int t = N - 1;
	while (t >= 0) {

		seq.armas[t].alvo.tipo = (seq.armas[t].alvo.tipo + 1) % M;
		if (seq.armas[t].alvo.tipo == 0)
			t--;
		else
			return 0;
	}
	return -1;
}

void imp_seq_n_base_m(cromossomo &seq, int n, int m) {
	int i;
	int k;
	float curr = 999999;
	for (i = 0; i < n; i++)
		seq.armas[i].alvo.tipo = 0;
	do {

		curr = avaliar(seq);

		if (curr < melhorSolucao.fitness) {
			melhorSolucao.fitness = curr;
			for (k = 0; k < numeroTotalArmas; k++) {
				melhorSolucao.armas[k].alvo.tipo = seq.armas[k].alvo.tipo;
			}

			/*for (k = 0; k < numeroTotalArmas; k++) {
				printf_s("%d ", seq.armas[k].alvo.tipo);
			}
			printf_s("\t%f\n", melhorSolucao.fitness);*/
		}
	} while (proxima(seq, n, m) == 0);
}

void exaustivo() {
	int i = 0;
	cromossomo current;
	current.armas = (struct arma*)malloc(numeroTotalArmas * sizeof(struct arma));
	for (i = 0; i < numeroTotalArmas; i++)
		current.armas[i].tipo = VetorArmas[i].tipo;


	imp_seq_n_base_m(current, numeroTotalArmas, numeroAlvos);
}

void algoritmoGenetico() {
	clock_t tempo_inicio;
	clock_t tempo_fim;
	clock_t tempo_current;

	if (criterioParada == 1) {
		for (iteracao = 0; iteracao < maxIteracoes; iteracao++) {
			crossover();
			mutacao();
			avaliar_populacao();
		}
	}
	else if (criterioParada == 2) {
		tempo_inicio = clock();
		tempo_fim = tempo_inicio + tempoLimite * 1000;
		tempo_current = clock();
		while (tempo_current < tempo_fim) {
			crossover();
			mutacao();
			avaliar_populacao();
			tempo_current = clock();
		}
	}
}

void algoritmoGeneticoComBuscalocal() {
	clock_t tempo_inicio;
	clock_t tempo_fim;
	clock_t tempo_current;
	if (criterioParada == 1) {
		for (iteracao = 0; iteracao < maxIteracoes; iteracao++) {
			crossover();
			mutacao();
			avaliar_populacao();
			if (iteracao > pontoBuscaLocal)
				buscal_local();
			ordenarPopulacao();
			//avaliar_populacao();
		}
	}
	else if (criterioParada == 2) {
		tempo_inicio = clock();
		tempo_fim = tempo_inicio + tempoLimite * 1000;
		tempo_current = clock();
		while (tempo_current < tempo_fim) {
			crossover();
			mutacao();
			avaliar_populacao();
			buscal_local();
			ordenarPopulacao();
			tempo_current = clock();
		}
	}
}

int main()
{
	int i;
	int mode;
	int msec;



	srand((unsigned)time(NULL));
	clock_t tempo_inicio;
	clock_t tempo_fim;
	clock_t tempo_exaust;
	clock_t tempo_metaheuristica;
	clock_t tempo_current;

	FILE *out_file;
	FILE *in_file;
	FILE *param;


	fopen_s(&out_file, "Resultados.txt", "a+");
	fopen_s(&in_file, "Entrada.txt", "r");
	fopen_s(&param, "Parametros.txt", "r");
	if (in_file == NULL || param == NULL) {
		printf_s("Arquivos de entrada inexistentes.\n");
		return 0;
	}
	fscanf_s(param, "%d", &maxIteracoes);
	fscanf_s(param, "%d", &nPopulacao);
	fscanf_s(param, "%d", &taxaMutacao);
	fscanf_s(param, "%f", &pontoBuscaLocal);
	pontoBuscaLocal = pontoBuscaLocal / 100 * maxIteracoes;





	start(in_file);
	


	do {
		system("cls");
		printf_s("1- Metodo exaustivo\n2- Metodo meta-heuristica\n3- Metodo meta-heuristica com busca local\n4- Benchmark heuristica sem busca local\n5- Benchmark heuristica com busca local\nOpcao: ");
		scanf_s("%d", &mode);
	} while (mode < 1 || mode > 7);

	
	if (mode != 1) {
		do {
			system("cls");
			printf_s("Criterio de parada:\n1- Iteracoes sem melhora\n2- Tempo limite de execucao\nOpcao: ");
			scanf_s("%d", &criterioParada);
		} while (criterioParada < 1 || criterioParada > 2);

		if (criterioParada == 2) {
			do {
				system("cls");
				printf_s("Digita o tempo limite em segundos: ");
				scanf_s("%d", &tempoLimite);
			} while (tempoLimite < 0);
			pontoBuscaLocal = 0;
		}
	}
	



	fprintf_s(out_file, "Para %d armas e %d alvos:\n", numeroTotalArmas, numeroAlvos);

	//EXAUSTIVO
	melhorSolucao.fitness = 9999999;
	if (mode == 1) {
		printf("Rodando exaustivo...\n");
		tempo_inicio = clock();

		exaustivo();

		tempo_fim = clock();
		tempo_exaust = tempo_fim - tempo_inicio;

		fprintf_s(out_file, "Exaustivo:\n%f\n", melhorSolucao.fitness);
		for (i = 0; i < numeroTotalArmas; i++)
			fprintf_s(out_file, "%d ", melhorSolucao.armas[i].alvo.tipo);

		msec = tempo_exaust * 1000 / CLOCKS_PER_SEC;
		fprintf_s(out_file, "\nTempo de execucao: %d segundos %d milisegundos\n\n", msec / 1000, msec % 1000);
	}
	///////////////////////////////////////


	//HEURISTICAS
	melhorSolucao.fitness = 9999999;
	if (mode == 2 || mode == 3) {
		printf("Rodando meta-heuristica...\n");
		tempo_inicio = clock();

		iniciarpopulacao();
		if (mode == 2) {
			algoritmoGenetico();
		}	
		else {
			algoritmoGeneticoComBuscalocal();
		}

		tempo_fim = clock();
		tempo_metaheuristica = tempo_fim - tempo_inicio;

		if (mode == 2)
			fprintf_s(out_file, "Meta-heuristica:\n%f\n", melhorSolucao.fitness);
		else
			fprintf_s(out_file, "Meta-heuristica com busca local:\n%f\n", melhorSolucao.fitness);
		for (i = 0; i < numeroTotalArmas; i++)
			fprintf_s(out_file, "%d ", melhorSolucao.armas[i].alvo.tipo);

		msec = tempo_metaheuristica * 1000 / CLOCKS_PER_SEC;
		fprintf_s(out_file, "\nTempo de execucao: %d segundos %d milisegundos\n\n", msec / 1000, msec % 1000);
	}
	//////////////////////////////////////


	
	//BENCHMARK
	if (mode == 4 || mode == 5) {
		int iteracoesBench;
		float mediaMeta = 0;
		float mediaTimeMeta = 0;
		system("cls");
		printf("Digite o numero de testes: ");
		scanf_s("%d", &iteracoesBench);

		fprintf_s(out_file, "Benchmark\n");

		
		if (mode == 4) {
			printf("Heuristica sem busca local\n");
			fprintf_s(out_file, "\n\nHeuristica sem busca local:\n");
			for (i = 0; i < iteracoesBench; i++) {
				printf("Tentativa %d...\n", i + 1);
				melhorSolucao.fitness = 9999999;
				tempo_inicio = clock();

				iniciarpopulacao();
				algoritmoGenetico();

				tempo_fim = clock();
				tempo_metaheuristica = tempo_fim - tempo_inicio;

				mediaMeta += melhorSolucao.fitness;
				mediaTimeMeta += tempo_metaheuristica;

				fprintf_s(out_file, "%f\n", melhorSolucao.fitness);
				msec = tempo_metaheuristica * 1000 / CLOCKS_PER_SEC;
				fprintf_s(out_file, "%d segundos %d milisegundos\n\n", msec / 1000, msec % 1000);
			}
			mediaMeta /= iteracoesBench;
			mediaTimeMeta /= iteracoesBench;

			fprintf_s(out_file, "Media: %f\n", mediaMeta);

			msec = mediaTimeMeta * 1000 / CLOCKS_PER_SEC;
			fprintf_s(out_file, "Tempo medio: %d segundos %d milisegundos\n", msec / 1000, msec % 1000);
		}
		else {

			printf("Heuristica com busca local\n");
			fprintf_s(out_file, "\n\nMeta-heuristica com busca local:\n");
			for (i = 0; i < iteracoesBench; i++) {
				printf("Tentativa %d...\n", i + 1);
				melhorSolucao.fitness = 9999999;
				tempo_inicio = clock();

				iniciarpopulacao();
				algoritmoGeneticoComBuscalocal();

				tempo_fim = clock();
				tempo_metaheuristica = tempo_fim - tempo_inicio;

				mediaMeta += melhorSolucao.fitness;
				mediaTimeMeta += tempo_metaheuristica;

				fprintf_s(out_file, "%f\n", melhorSolucao.fitness);
				msec = tempo_metaheuristica * 1000 / CLOCKS_PER_SEC;
				fprintf_s(out_file, "%d segundos %d milisegundos\n\n", msec / 1000, msec % 1000);
			}
			mediaMeta /= iteracoesBench;
			mediaTimeMeta /= iteracoesBench;

			fprintf_s(out_file, "Media: %f\n", mediaMeta);

			msec = mediaTimeMeta * 1000 / CLOCKS_PER_SEC;
			fprintf_s(out_file, "Tempo medio: %d segundos %d milisegundos\n\n", msec / 1000, msec % 1000);
		}
		

		fprintf_s(out_file, "//////////////////////////////////////////////////\n\n");

	}



	///////////////////////////////////////


	fclose(in_file);
	fclose(out_file);
	fclose(param);

	system("pause");
	return 0;
}
