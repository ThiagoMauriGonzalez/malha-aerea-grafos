#include "meustipos.h"
#include "estrutura.h"
#include "aeroporto.h"
#include "voo.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

void limpar_buffer();
void exibir_menu();
void cadastrar_aeroporto_menu(MalhaAerea* malha);
void cadastrar_voo_menu(MalhaAerea* malha);
void remover_voo_menu(MalhaAerea* malha);
void listar_voos_menu(MalhaAerea* malha);
void listar_trajetos_menu(MalhaAerea* malha);

I32 main() {
    int opcao;
    MalhaAerea malha;

    inicializarMalha(&malha);

    // Cadastra aeroportos iniciais
    struct { char cod[4]; char cidade[100]; } aeroportos_iniciais[] = {
        {"BSB", "Brasilia"},
        {"CNF", "Belo Horizonte"},
        {"GIG", "Rio de Janeiro"},
        {"GRU", "Guarulhos"},
        {"SSA", "Salvador"}
    };
    for (int i = 0; i < 5; i++) {
        if (cadastrarAeroporto(&malha, aeroportos_iniciais[i].cod, aeroportos_iniciais[i].cidade)) {
            printf("Aeroporto %s (%s) cadastrado com sucesso.\n", aeroportos_iniciais[i].cod, aeroportos_iniciais[i].cidade);
        }
    }

    // Cadastra voos conforme a matriz da imagem
    struct { char origem[4]; char destino[4]; U32 numero; } voos_iniciais[] = {
        {"BSB", "SSA", 107},
        {"CNF", "GIG", 555},
        {"CNF", "GRU", 101},
        {"CNF", "SSA", 214},
        {"GIG", "BSB", 50},
        {"GIG", "CNF", 102},
        {"GIG", "GRU", 89},
        {"GRU", "CNF", 90},
        {"GRU", "SSA", 215},
        {"SSA", "CNF", 554},
        // Novo voo para permitir GIG -> CNF -> GRU -> BSB
        {"GRU", "BSB", 999}
    };
    for (int i = 0; i < sizeof(voos_iniciais)/sizeof(voos_iniciais[0]); i++) {
        if (cadastrarVoo(&malha, voos_iniciais[i].origem, voos_iniciais[i].destino, voos_iniciais[i].numero)) {
            printf("Voo %u cadastrado: %s -> %s\n", voos_iniciais[i].numero, voos_iniciais[i].origem, voos_iniciais[i].destino);
        }
    }

    do {
        exibir_menu();
        scanf("%d", &opcao);
        limpar_buffer();

        switch (opcao) {
            case 1:
                cadastrar_aeroporto_menu(&malha);
                break;
            case 2:
                cadastrar_voo_menu(&malha);
                break;
            case 3:
                remover_voo_menu(&malha);
                break;
            case 4:
                listar_voos_menu(&malha);
                break;
            case 5:
                listar_trajetos_menu(&malha);
                break; 
            case 0:
                printf("\nEncerrando o programa...\n");
                break;
            default:
                printf("\nOpcao invalida! Por favor, tente novamente.\n");
        }
    } while (opcao != 0);

    // Libera memoria alocada
    liberarMalha(&malha);
    return 0;
}

void limpar_buffer() {
    I32 c;
    while ((c = getchar()) != '\n' && c != EOF);
} 

void exibir_menu() {
    printf("\n=== SISTEMA DE MALHA AÉREA ===\n");
    printf("1. Cadastrar novo aeroporto\n");
    printf("2. Cadastrar voo entre aeroportos\n");
    printf("3. Remover voo\n");
    printf("4. Listar voos que saem de um aeroporto\n");
    printf("5. Listar trajetos possiveis entre dois aeroportos\n");
    printf("0. Sair\n");
    printf("Escolha uma opcao: ");
}

void cadastrar_aeroporto_menu(MalhaAerea* malha) {
    char codigo[4];
    char cidade[100];

    printf("\n=== Cadastro de Aeroporto ===\n");
    printf("Codigo (3 letras): ");
    scanf("%3s", codigo);
    limpar_buffer();

    printf("Cidade: ");
    scanf("%99[^\n]", cidade);
    limpar_buffer();

    if (cadastrarAeroporto(malha, codigo, cidade)) {
        printf("\nAeroporto cadastrado com sucesso.\n");
    } else {
        printf("\nErro: aeroporto com esse codigo ja existe.\n");
    }
}

void cadastrar_voo_menu(MalhaAerea* malha) {
    char origem[4];
    char destino[4];
    U32 numeroVoo;

    printf("\n=== Cadastro de Voo ===\n");
    printf("Codigo do aeroporto de origem: ");
    scanf("%3s", origem);
    limpar_buffer();

    printf("Codigo do aeroporto de destino: ");
    scanf("%3s", destino);
    limpar_buffer();

    printf("Numero do voo: ");
    scanf("%u", &numeroVoo);
    limpar_buffer();

    if (cadastrarVoo(malha, origem, destino, numeroVoo)) {
        printf("\nVoo cadastrado com sucesso.\n");
    } else {
        printf("\nErro ao cadastrar voo: verifique se os aeroportos existem e se o voo ja foi cadastrado.\n");
    }
}

void remover_voo_menu(MalhaAerea* malha) {
    U32 numeroVoo;
    printf("\n=== Remoção de Voo ===\n");
    printf("Número do voo a remover: ");
    scanf("%u", &numeroVoo);
    limpar_buffer();

    if (removerVoo(malha, numeroVoo)) {
        printf("\nVoo removido com sucesso.\n");
    } else {
        printf("\nErro: voo %u não encontrado.\n", numeroVoo);
    }
}

void listar_voos_menu(MalhaAerea* malha) {
    char origem[4];
    printf("\n=== Listar Voos ===\n");
    printf("Codigo do aeroporto de origem: ");
    scanf("%3s", origem);
    limpar_buffer();

    VooInfo* lista;
    U32 qtd;
    if (!obterVoosSaida(malha, origem, &lista, &qtd)) {
        printf("\nErro: aeroporto %s nao encontrado.\n", origem);
        return;
    }
    if (qtd == 0) {
        printf("\nNao ha voos saindo de %s.\n", origem);
        return;
    }
    printf("\nVoos saindo de %s:\n", origem);
    for (U32 i = 0; i < qtd; i++) {
        printf("Voo %u -> %s\n",
               lista[i].numero,
               malha->aeroportos[lista[i].destinoIdx].cidade);
    }
    free(lista);
}

void listar_trajetos_menu(MalhaAerea* malha) {
    char origem[4], destino[4];
    printf("\n=== Listar Trajetos ===\n");
    printf("Codigo do aeroporto de origem: ");
    scanf("%3s", origem);
    limpar_buffer();

    printf("Codigo do aeroporto de destino: ");
    scanf("%3s", destino);
    limpar_buffer();

    listarTodosCaminhos(malha, origem, destino);
}
