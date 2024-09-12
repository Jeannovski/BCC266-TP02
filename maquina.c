#include "maquina.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

void inicializarRam(memoriaRam *ram) {
    srand(time(NULL));
    for (int i = 0; i < TAMANHO_RAM; i++) {
        for (int j = 0; j < PALAVRAS_POR_BLOCO; j++) {
            ram->blocos[i].palavras[j] = rand() % 4; //preenche com valores aleatorios entre 0 e 3
        }
        ram->blocos[i].valido = 1; //valida o bloco da memoria ram
        ram->blocos[i].tag = i; //da o endereco ao bloco
    }
}

void inicializarCaches(cacheL1 *l1, cacheL2 *l2, cacheL3 *l3) {
    for (int i = 0; i < TAMANHO_L1; i++) {
        l1->blocos[i].valido = 0;//valida como 0 pois inicialmente esta vazia
        l1->blocos[i].tag = -1;//usamos -1 pra demonstrar que nao ha nenhum bloco aqui
        l1->blocos[i].contadorLRU = 0;//id de entrada do elemento
        for (int j = 0; j < PALAVRAS_POR_BLOCO; j++) {
            l1->blocos[i].palavras[j] = 0;
        }
    }

    l1->acessos = 0;
    l1->hits = 0;
    l1->misses = 0;
    l1->contadorGlobalLRU = 0;//contador pra controlar as entradas na cache

    for (int i = 0; i < TAMANHO_L2; i++) {
        l2->blocos[i].valido = 0;//valida como 0 pois inicialmente esta vazia
        l2->blocos[i].tag = -1;//usamos -1 pra demonstrar que nao ha nenhum bloco aqui
        l2->blocos[i].contadorLRU = 0;//id de entrada do elemento
        for (int j = 0; j < PALAVRAS_POR_BLOCO; j++) {
            l2->blocos[i].palavras[j] = 0;
        }
    }
    l2->acessos = 0;
    l2->hits = 0;
    l2->misses = 0;
    l2->contadorGlobalLRU = 0;//contador pra controlar as entradas na cache

    for (int i = 0; i < TAMANHO_L3; i++) {
        l3->blocos[i].valido = 0;//valida como 0 pois inicialmente esta vazia
        l3->blocos[i].tag = -1;//usamos -1 pra demonstrar que nao ha nenhum bloco aqui
        l3->blocos[i].contadorLRU = 0;//id de entrada do elemento
        for (int j = 0; j < PALAVRAS_POR_BLOCO; j++) {
            l3->blocos[i].palavras[j] = 0;
        }
    }
    l3->acessos = 0;
    l3->hits = 0;
    l3->misses = 0;
    l3->contadorGlobalLRU = 0;//contador pra controlar as entradas na cache
}

void imprimirRam(const memoriaRam *ram) {
    for (int i = 0; i < TAMANHO_RAM; i++) {
        printf("bloco %d: ", i);
        for (int j = 0; j < PALAVRAS_POR_BLOCO; j++) {
            printf("%d ", ram->blocos[i].palavras[j]);
        }
        printf("\n");
    }
}

void processarInstrucoes(const char* filename, int num_instrucoes, cacheL1 *l1, cacheL2 *l2, cacheL3 *l3, memoriaRam *ram) {

    //abertura do arquivo

    FILE* arquivo = fopen(filename, "r");
    if (!arquivo) {
        printf("erro ao abrir o arquivo de instrucoes.\n");
        return;
    }

    //inicia a maquina

    for (int i = 0; i < num_instrucoes; i++) {
        int opcode, bloco1, palavra1, bloco2, palavra2, bloco_res, palavra_res;
        fscanf(arquivo, "%d:%d:%d:%d:%d:%d:%d\n", &opcode, &bloco1, &palavra1, &bloco2, &palavra2, &bloco_res, &palavra_res);

        printf("Instrução %d: opcode: %d, bloco1: %d palavra1: %d, bloco2: %d palavra2: %d, bloco Res: %d palavra res: %d\n", i, opcode, bloco1, palavra1, bloco2, palavra2, bloco_res, palavra_res);

        //busca os valores desejados na memoria para realizar a operacao

        int valor1 = buscarNaCacheEOperar(bloco1, palavra1, opcode, l1, l2, l3, ram);
        int valor2 = buscarNaCacheEOperar(bloco2, palavra2, opcode, l1, l2, l3, ram);

        int resultado;
        if (opcode == 0) { //soma
            resultado = valor1 + valor2;
        } else if (opcode == 1) { //subtracao
            resultado = valor1 - valor2;
        }

        //armazena o valor da operacao no endereco desejado para
        ram->blocos[bloco_res].palavras[palavra_res] = resultado;

        printf("Resultado: %d\n", resultado);
    }

    //fecha o processamento

    fclose(arquivo);
}

int buscarNaCacheEOperar(int enderecoBloco, int palavra, int opcode, cacheL1 *l1, cacheL2 *l2, cacheL3 *l3, memoriaRam *ram) {

    //buscar na cache 1

    for (int i = 0; i < TAMANHO_L1; i++) {
        if (l1->blocos[i].valido && l1->blocos[i].tag == enderecoBloco) {
            l1->acessos++; //contabilizar acesso
            l1->hits++; //contabilizar hit caso aconteca
            l1->blocos[i].contadorLRU = l1->contadorGlobalLRU++; //atualiza o contador do elemento pois ele foi usado recentemente
            return l1->blocos[i].palavras[palavra]; //retorna o valor desejado pra operacao
        }
    }

    //busca na cache 2

    for (int i = 0; i < TAMANHO_L2; i++) {
        if (l2->blocos[i].valido && l2->blocos[i].tag == enderecoBloco) {
            l2->acessos++; //contabiliza o acesso a cache 2
            l2->hits++; //contabiliza o hit a cache 2
            l2->blocos[i].contadorLRU = l2->contadorGlobalLRU++;//atualiza o contador do elemento pois ele foi usado recentemente
            carregarParaCache(l1->blocos, TAMANHO_L1, enderecoBloco, l2->blocos[i].palavras, &l1->contadorGlobalLRU);//leva ele pra cache 1
            return l2->blocos[i].palavras[palavra];
        }
    }

    for (int i = 0; i < TAMANHO_L3; i++) {
        if (l3->blocos[i].valido && l3->blocos[i].tag == enderecoBloco) {
            l3->acessos++;//contabiliza o acesso a cache 3
            l3->hits++;//contabiliza o hit na cache 3
            l3->blocos[i].contadorLRU = l3->contadorGlobalLRU++;//atualiza o contador do elemento pois ele foi usado recentemente
            carregarParaCache(l2->blocos, TAMANHO_L2, enderecoBloco, l3->blocos[i].palavras, &l2->contadorGlobalLRU);//carrega pra cache 2
            carregarParaCache(l1->blocos, TAMANHO_L1, enderecoBloco, l3->blocos[i].palavras, &l1->contadorGlobalLRU);//carrega pra cache 1
            return l3->blocos[i].palavras[palavra];
        }
    }

    //se nao estiver em nenhuma cache, buscar na RAM
    int valor = ram->blocos[enderecoBloco].palavras[palavra];

    //copia o bloco pra l3
    carregarParaCache(l3->blocos, TAMANHO_L3, enderecoBloco, ram->blocos[enderecoBloco].palavras, &l3->contadorGlobalLRU);
    //copia o bloco pra l2
    carregarParaCache(l2->blocos, TAMANHO_L2, enderecoBloco, ram->blocos[enderecoBloco].palavras, &l2->contadorGlobalLRU);
    //copia o bloco pra l1
    carregarParaCache(l1->blocos, TAMANHO_L1, enderecoBloco, ram->blocos[enderecoBloco].palavras, &l1->contadorGlobalLRU);

    //contabiliza as percas visto que nenhum valor foi acessado nas caches

    l1->misses++;
    l2->misses++;
    l3->misses++;

    return valor;
}

void carregarParaCache(blocoMemoria *cache, int tamanhoCache, int enderecoBloco, int palavras[PALAVRAS_POR_BLOCO], int *contadorGlobalLRU) {
    //variaveis auxiliares para ajudar na busca do elemento mais antigo
    int menorLRU = *contadorGlobalLRU;
    int indiceSubstituicao = 0;

    for (int i = 0; i < tamanhoCache; i++) {
        if (cache[i].valido == 0) {

            //caso haja um espaco valido na memoria nao substituir

            indiceSubstituicao = i;
            break;
        }

        //caso a cache estiver cheia pegar o indice do elemento mais antigo e substituir

        if (cache[i].contadorLRU < menorLRU) {
            menorLRU = cache[i].contadorLRU;
            indiceSubstituicao = i;
        }
    }

    //substitui o bloco na cache
    cache[indiceSubstituicao].valido = 1;
    cache[indiceSubstituicao].tag = enderecoBloco;
    cache[indiceSubstituicao].contadorLRU = *contadorGlobalLRU;
    (*contadorGlobalLRU)++;
    memcpy(cache[indiceSubstituicao].palavras, palavras, sizeof(int) * PALAVRAS_POR_BLOCO);//copia os valores para o novo bloco
}
