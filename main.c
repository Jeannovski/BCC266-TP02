#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "maquina.h"

int main(int argc, char *argv[]) {
    //verifica se o numero de arquivos esta correto
    if (argc != 2) {
        printf("Uso: %s <arquivo_de_instrucoes.txt>\n", argv[0]);
        return 1;
    }

    //inicializa as variaveis
    cacheL1 l1;
    cacheL2 l2;
    cacheL3 l3;
    memoriaRam ram;
    int aux;

    //inicializa as estruturas 
    inicializarRam(&ram);
    inicializarCaches(&l1, &l2, &l3);
    printf("Digite o numero de instrucoes a serem recebidas:\n");
    scanf("%d", &aux);

    //processa a quantidade de instrucoes solicitadas
    processarInstrucoes(argv[1], aux, &l1, &l2, &l3, &ram);

    //exibe os resultados da cache 
    printf("Cache L1 - Acessos: %d, Hits: %d, Misses: %d\n", l1.acessos, l1.hits, l1.misses);
    printf("Cache L2 - Acessos: %d, Hits: %d, Misses: %d\n", l2.acessos, l2.hits, l2.misses);
    printf("Cache L3 - Acessos: %d, Hits: %d, Misses: %d\n", l3.acessos, l3.hits, l3.misses);

    return 0;
}