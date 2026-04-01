/******************************************************************************
Universidade de São Paulo - ICMC/EESC - Curso de Eng. de Computação
SSC0903 - Computação de Alto Desempenho (2026/1) – Trab01

Integrantes do Grupo 5:

- Diogo Barboza de Souza - 12745657
- Gabriela Passos de Andrade - 12625142
- João Marcelo Moreira Trovão Filho - 13676332
- Lucas Fernando Witzel Camara Ferreira - 11800650
- Matheus dos Santos Ines - 12546784
- Rafael Cunha Bejes Learth - 13676367

Codigo Paralelo: studentspar.c
*******************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <omp.h>

// Numero de repeticoes para o calculo do tempo medio
#define NUM_REP 100

// Optamos por tratar os arrays contendo os dados dos estudantes como estruturas 
// unidimensionais para melhor desempenho. Por isso, foram criadas macros que facilitam 
// a leitura do codigo no momento da indexacao de posicoes.

// Macros para indexacao de arrays multidimensionais
#define INDEX_1DIM(r) ((size_t)(r))
#define INDEX_2DIM(r, c) ((size_t)(r)*(C) + (c))
#define INDEX_3DIM(r,c,a) ((size_t)(r)*(C)*(A) + (size_t)(c)*(A) + (a))
#define INDEX_4DIM(r,c,a,n) ((size_t)(r)*(C)*(A)*(N) + (size_t)(c)*(A)*(N) + (size_t)(a)*(N) + (n))

// Estrutura para armazenar os dados estatisticos
typedef struct {
    double min_nota;
    double max_nota;
    double mediana;
    double media;
    double desvio_padrao;
} Dados;

/*
 * Funcao para gerar a tabela com todas as notas de cada estudante
 */
void gerarTabela(int R, int C, int A, int N, double* estudantes) {
    for (int r = 0; r < R; r++) {
        for (int c = 0; c < C; c++) {
            for (int a = 0; a < A; a++) {
                for (int n = 0; n < N; n++) {
                    estudantes[INDEX_4DIM(r, c, a, n)] = (double)(rand() % 1001) / 10.0;
                }
            }
        }
    }
}

/*
 * Funcao para comparar dois valores do tipo double
 */
int compararDouble(const void *a, const void *b) {
    double da = *(const double*) a;
    double db = *(const double*) b;
    if (da < db) return -1;
    if (da > db) return 1;
    return 0;
}

/*
 * Funcao para calcular os dados estatisticos: min, max, mediana, media e desvio padrao
*/
Dados calcularDados(const double *valores, double *aux_vetor, int lim) {
    Dados d = {0};
    double soma = 0.0;
    double soma_quadrados = 0.0, atual_valor, variancia;
    double min_valor = valores[0];
    double max_valor = valores[0];

    // Calculo das notas minimas e maximas
    #pragma omp parallel for reduction(+:soma, soma_quadrados) reduction(min:min_valor) reduction(max:max_valor)
    for (int i = 0; i < lim; i++) {
        atual_valor = valores[i];
        if (atual_valor < min_valor) min_valor = atual_valor;
        if (atual_valor > max_valor) max_valor = atual_valor;
    
        soma += atual_valor;
        soma_quadrados += (atual_valor * atual_valor);
        aux_vetor[i] = atual_valor; 
    }

    d.min_nota = min_valor;
    d.max_nota = max_valor;

    // Calculo da media e do desvio padrao
    d.media = soma / lim;
    variancia = (soma_quadrados / lim) - (d.media * d.media);
    d.desvio_padrao = sqrt(variancia < 0 ? 0 : variancia);

    // Calculo da mediana
    qsort(aux_vetor, (size_t)lim, sizeof(double), compararDouble);
    if (lim % 2 == 0) 
        d.mediana = (aux_vetor[lim / 2 - 1] + aux_vetor[lim / 2]) / 2.0;
    else 
        d.mediana = aux_vetor[lim / 2];

    return d;
}

/*
 * Funcao para formatar um valor double no formato brasileiro (com virgula)
 */
void formatarPT(double valor_original, char *saida, size_t tam_saida) {
    snprintf(saida, tam_saida, "%.1f", valor_original);
    for (char *c = saida; *c != '\0'; c++) {
        if (*c == '.') *c = ',';
    }
}

/*
 * Funcao para imprimir uma linha da tabela com os dados estatisticos
 */
void printLinha(const char *texto, Dados d) {
    char min_pt[32], max_pt[32], mediana_pt[32], media_pt[32], desvio_pt[32];
    formatarPT(d.min_nota, min_pt, sizeof(min_pt));
    formatarPT(d.max_nota, max_pt, sizeof(max_pt));
    formatarPT(d.mediana, mediana_pt, sizeof(mediana_pt));
    formatarPT(d.media, media_pt, sizeof(media_pt));
    formatarPT(d.desvio_padrao, desvio_pt, sizeof(desvio_pt));

    printf("| %-15s | %8s | %8s | %8s | %8s | %8s |\n", texto, min_pt, max_pt, mediana_pt, media_pt, desvio_pt);
}

/**
 * Funcao para imprimir as tabelas com os resultado dos dados estatisticos
 */
void printTabelas(int R, int C, Dados *cidade_Dados, Dados *regiao_Dados, Dados brasil_Dados, int melhor_regiao, int melhor_cidade_regiao, int melhor_cidade, double tempo_total) {
    printf("Tabelas para a saida: consideram as notas medias dos alunos.\n\n");

    printf("+-----------------+----------+----------+----------+----------+----------+\n");
    printf("| %-15s | %-8s | %-8s | %-8s | %-8s | %-8s |\n", "Cidades", "Min Nota", "Max Nota", "Mediana", "Media", "DsvPad");
    printf("+-----------------+----------+----------+----------+----------+----------+\n");
    for (int r = 0; r < R; r++) {
        for (int c = 0; c < C; c++) {
            char label[32];
            snprintf(label, sizeof(label), "R=%d, C=%d", r, c);
            printLinha(label, cidade_Dados[INDEX_2DIM(r, c)]);
        }
    }
    printf("+-----------------+----------+----------+----------+----------+----------+\n\n");

    printf("+-----------------+----------+----------+----------+----------+----------+\n");
    printf("| %-15s | %-8s | %-8s | %-8s | %-8s | %-8s |\n", "Regioes", "Min Nota", "Max Nota", "Mediana", "Media", "DsvPad");
    printf("+-----------------+----------+----------+----------+----------+----------+\n");
    for (int r = 0; r < R; r++) {
        char label[32];
        snprintf(label, sizeof(label), "R=%d", r);
        printLinha(label, regiao_Dados[INDEX_1DIM(r)]);
    }
    printf("+-----------------+----------+----------+----------+----------+----------+\n\n");

    printf("+-----------------+----------+----------+----------+----------+----------+\n");
    printf("| %-15s | %-8s | %-8s | %-8s | %-8s | %-8s |\n", "Brasil", "Min Nota", "Max Nota", "Mediana", "Media", "DsvPad");
    printf("+-----------------+----------+----------+----------+----------+----------+\n");
    printLinha("", brasil_Dados);
    printf("+-----------------+----------+----------+----------+----------+----------+\n\n");

    printf("+----------------------+----------------------+------------------+\n");
    printf("| %-20s | %-20s | %-16s |\n", "Premiacao", "Regicao/Cidade", "Media Aritmetica");
    printf("+----------------------+----------------------+------------------+\n");

    char media_regiao[32], media_cidade[32];
    formatarPT(regiao_Dados[INDEX_1DIM(melhor_regiao)].media, media_regiao, sizeof(media_regiao));
    formatarPT(cidade_Dados[INDEX_2DIM(melhor_cidade_regiao, melhor_cidade)].media, media_cidade, sizeof(media_cidade));

    char id_regiao[32], id_cidade[32];
    snprintf(id_regiao, sizeof(id_regiao), "R=%d", melhor_regiao);
    snprintf(id_cidade, sizeof(id_cidade), "R=%d, C=%d", melhor_cidade_regiao, melhor_cidade);

    printf("| %-20s | %-20s | %16s |\n", "Melhor Regiao", id_regiao, media_regiao);
    printf("| %-20s | %-20s | %16s |\n", "Melhor Cidade", id_cidade, media_cidade);
    printf("+----------------------+----------------------+------------------+\n");

    printf("\nTempo medio de execucao: %.6f (s)\n\n", tempo_total / NUM_REP);
}

/*
 * Funcao principal
 */
int main(void) {
    // ===================================================
    // Lendo os parametros de entrada do arquivo input.txt
    // ===================================================
    int R, C, A, N, T, seed;

    FILE* fp = fopen("input.txt", "r");
    if (fp == NULL) {
        fprintf(stderr, "Erro: nao foi possivel abrir o arquivo de entrada.\n");
        return 1;
    }
    if (fscanf(fp, "%d %d %d %d %d %d", &R, &C, &A, &N, &T, &seed) != 6) {
        fprintf(stderr, "Entrada invalida.\n");
        fclose(fp);
        return 1;
    }
    fclose(fp);

    if (R <= 0 || C <= 0 || A <= 0 || N <= 0 || T <= 0 || seed <= 0) {
        fprintf(stderr, "R, C, A, N, T e seed devem ser positivos.\n");
        return 1;
    }

    srand((unsigned int)seed);

    // ============================================
    // Alocacao de memoria para as tabelas de dados
    // ============================================
    Dados brasil_Dados;

    Dados *cidade_Dados = malloc((size_t) R * C * sizeof(*cidade_Dados));
    if (cidade_Dados == NULL) {
        fprintf(stderr, "Erro: memoria insuficiente para criar a tabela de dados das cidades.\n");
        return 1;
    }

    Dados *regiao_Dados = malloc((size_t) R * sizeof(*regiao_Dados));
    if (regiao_Dados == NULL) {
        fprintf(stderr, "Erro: memoria insuficiente para criar a tabela de dados das regioes.\n");
        free(cidade_Dados);
        return 1;
    }

    double* estudantes = malloc((size_t) R * C * A * N * sizeof(*estudantes));
    if (estudantes == NULL) {
        fprintf(stderr, "Erro: memoria insuficiente para criar a tabela de estudantes.\n");
        free(cidade_Dados);
        free(regiao_Dados);
        return 1;
    }
    
    double *media = malloc((size_t) R * C * A * sizeof(*media));
    if (media == NULL) {
        fprintf(stderr, "Erro: memoria insuficiente para criar a tabela de medias.\n");
        free(cidade_Dados);
        free(regiao_Dados);
        free(estudantes);
        return 1;
    }

    /*
    double *aux_vetor = malloc((size_t) R * C * A * sizeof(*aux_vetor));
    if (aux_vetor == NULL) {
        fprintf(stderr, "Erro: memoria insuficiente para vetor auxiliar.\n");
        free(cidade_Dados);
        free(regiao_Dados);
        free(estudantes);
        free(media);
        return 1;
    }
    */

    double inicio_tempo, final_tempo, tempo_total = 0.0;
    int melhor_regiao, melhor_cidade, melhor_cidade_regiao;

    // Preenchendo a tabela de estudantes com notas pseudo-aleatorias
    gerarTabela(R, C, A, N, estudantes);

    // ===================================================================================
    // Loop principal para o calculo dos dados estatisticos e medicao do tempo de execucao
    // ===================================================================================
    for (int rep = 0; rep < NUM_REP; rep++) {

        // Iniciando medicao de tempo
        inicio_tempo = omp_get_wtime();

        // Calculo das medias por aluno
        double soma;
        #pragma omp parallel for collapse(3) private(soma)
        for (int r=0; r<R; r++) {
            for (int c=0; c<C; c++) {
                for (int a=0; a<A; a++) {
                    // double soma = 0;
                    soma = 0;
                    for (int n=0; n<N; n++) {
                        soma += estudantes[INDEX_4DIM(r, c, a, n)];
                    }
                    media[INDEX_3DIM(r, c, a)] = (double) soma / N;
                }
            }
        }

        // Calculo das estatisticas por cidade
        #pragma omp parallel
        {
            double aux_vetor_cidade[A];

            #pragma omp for collapse(2)
            for (int r = 0; r < R; r++) {
                for (int c = 0; c < C; c++) {
                    cidade_Dados[INDEX_2DIM(r, c)] = calcularDados(&media[INDEX_3DIM(r, c, 0)], aux_vetor_cidade, A);
                }
            }
        }
        /*
        #pragma omp parallel for collapse(2)
        for (int r=0; r<R; r++) {
            for (int c=0; c<C; c++) {
                double aux_vector_cidade[A];
                cidade_Dados[INDEX_2DIM(r, c)] = calcularDados(&media[INDEX_3DIM(r, c, 0)], aux_vector_cidade, A); 
            }
        }
        */
        
        // Calculo das estatisticas por regiao
        #pragma omp parallel 
        {
            double aux_vetor_regiao[C * A];

            #pragma omp for
            for (int r = 0; r < R; r++) {
                regiao_Dados[INDEX_1DIM(r)] = calcularDados(&media[(size_t) r * (C * A)], aux_vetor_regiao, C * A);
            }
        }
        
        /*
        #pragma omp parallel for
        for (int r = 0; r < R; r++) {
            double aux_vetor_regiao[C * A];
            regiao_Dados[INDEX_1DIM(r)] = calcularDados(&media[(size_t) r * (C * A)], aux_vetor, C * A);
        }
        */

        // Calculo das estatisticas do Brasil
        double aux_vetor_brasil[R * C * A];
        brasil_Dados = calcularDados(media, aux_vetor_brasil, R * C * A);

        melhor_regiao = 0;
        melhor_cidade_regiao = 0;
        melhor_cidade = 0;

        // Definicao da Melhor Regiao
        for (int r=0; r<R; r++) {
            if (regiao_Dados[r].media > regiao_Dados[melhor_regiao].media) {
                melhor_regiao = r;
            }
        }

        // Definicao da Melhor Cidade dentro da Melhor Regiao
        for (int r=0; r<R; r++) {
            for (int c=0; c<C; c++) {
                if (cidade_Dados[INDEX_2DIM(r, c)].media > cidade_Dados[INDEX_2DIM(melhor_cidade_regiao, melhor_cidade)].media) {
                    melhor_cidade_regiao = r;
                    melhor_cidade = c;
                }
            }
        }
        
        // Finalizando medicao de tempo e calculando tempo total
        final_tempo = omp_get_wtime();
        tempo_total += final_tempo - inicio_tempo;
    }

    // ===============================================================================
    // Impressao dos resultados e liberacao da memoria alocada. Finalizacao do codigo.
    // ===============================================================================
    printTabelas(R, C, cidade_Dados, regiao_Dados, brasil_Dados, melhor_regiao, melhor_cidade_regiao, melhor_cidade, tempo_total);
    
    free(cidade_Dados);
    free(regiao_Dados);
    free(estudantes);
    free(media);
    //free(aux_vetor);

    return 0;
}
