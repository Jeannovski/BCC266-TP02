#ifndef MAQUINA_H
#define MAQUINA_H

//definindo o tamanho das estruturas em base 2

#define PALAVRAS_POR_BLOCO 4
#define TAMANHO_L1 32
#define TAMANHO_L2 64
#define TAMANHO_L3 128
#define TAMANHO_RAM 1024


//estrutura de um bloco de memoria

typedef struct {
    int palavras[PALAVRAS_POR_BLOCO];
    int valido;
    int tag;
    int contadorLRU;
} blocoMemoria;

//estrutura da cache 1

typedef struct {
    blocoMemoria blocos[TAMANHO_L1];
    int acessos;
    int hits;
    int misses;
    int contadorGlobalLRU;
} cacheL1;

//estrutura da cache 2

typedef struct {
    blocoMemoria blocos[TAMANHO_L2];
    int acessos;
    int hits;
    int misses;
    int contadorGlobalLRU;
} cacheL2;

//estrutura da cache 3

typedef struct {
    blocoMemoria blocos[TAMANHO_L3];
    int acessos;
    int hits;
    int misses;
    int contadorGlobalLRU;
} cacheL3;

//estrutura da ram 

typedef struct {
    blocoMemoria blocos[TAMANHO_RAM];
} memoriaRam;

//inicializar ram

void inicializarRam(memoriaRam *ram);

//inicializar as caches

void inicializarCaches(cacheL1 *l1, cacheL2 *l2, cacheL3 *l3);

//imprimir a ram

void imprimirRam(const memoriaRam *ram);

//funcao que simula a maquina

void processarInstrucoes(const char* filename, int num_instrucoes, cacheL1 *l1, cacheL2 *l2, cacheL3 *l3, memoriaRam *ram);

//funcao para processar os dados

int buscarNaCacheEOperar(int enderecoBloco, int palavra, int opcode, cacheL1 *l1, cacheL2 *l2, cacheL3 *l3, memoriaRam *ram);

//funcao auxiliar para levar as copias as memorias

void carregarParaCache(blocoMemoria *cache, int tamanhoCache, int enderecoBloco, int palavras[PALAVRAS_POR_BLOCO], int *contadorGlobalLRU);

#endif