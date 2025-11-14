/*
  ilha_mochila.c
  Projeto: Código da Ilha – Mochila (Nível Mestre)
  - Vetor de até 10 itens (struct Item)
  - Inserir, remover, listar
  - Ordenações: Bubble (nome), Insertion (tipo), Selection (prioridade)
  - Busca sequencial e Busca binária (após ordenação por nome)
  - Contagem de comparações e tempo de execução (clock)
  - Comentários e interface em Português
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>

#define MAX_ITENS 10
#define MAX_NOME 30
#define MAX_TIPO 20

/* Estrutura Item: nome, tipo, quantidade, prioridade */
typedef struct {
    char nome[MAX_NOME];
    char tipo[MAX_TIPO];
    int quantidade;
    int prioridade; /* 1 (menor) .. 10 (maior) */
} Item;

/* Variáveis globais do programa */
Item mochila[MAX_ITENS];
int numItens = 0;
bool ordenadaPorNome = false;

/* Contadores para análise de desempenho */
long comparacoesBuscaSequencial = 0;
long comparacoesBuscaBinaria = 0;
long comparacoesBubbleNome = 0;
long comparacoesInsertionTipo = 0;
long comparacoesSelectionPrioridade = 0;

/* ---------- Utilitários ---------- */

/* Remove o '\n' que fgets deixa no final */
void removerNovaLinha(char *s) {
    if (!s) return;
    size_t len = strlen(s);
    if (len == 0) return;
    if (s[len-1] == '\n') s[len-1] = '\0';
}

/* Limpa a tela de forma simples (imprime várias linhas) */
void limparTela(void) {
    for (int i = 0; i < 30; ++i) putchar('\n');
}

/* Mostra cabeçalho do menu principal com estado da ordenação */
void exibirMenu(void) {
    printf("=========================================\n");
    printf("  CÓDIGO DA ILHA - MOCHILA (Nível Mestre)\n");
    printf("=========================================\n");
    printf("Itens na mochila: %d / %d\n", numItens, MAX_ITENS);
    printf("Ordenada por nome: %s\n", ordenadaPorNome ? "SIM" : "NÃO");
    printf("-----------------------------------------\n");
    printf("1 - Adicionar um item\n");
    printf("2 - Remover um item (por nome)\n");
    printf("3 - Listar todos os itens\n");
    printf("4 - Ordenar itens (escolher critério e algoritmo)\n");
    printf("5 - Buscar item por nome (busca sequencial)\n");
    printf("6 - Buscar item por nome (busca binária - requer ordenação por nome)\n");
    printf("0 - Sair\n");
    printf("-----------------------------------------\n");
    printf("Escolha uma opção: ");
}

/* Lê uma string com fgets e remove o '\n' */
void lerString(const char *prompt, char *dest, int tamanho) {
    printf("%s", prompt);
    if (fgets(dest, tamanho, stdin) == NULL) {
        /* Em caso de EOF, garantir string vazia */
        dest[0] = '\0';
    }
    removerNovaLinha(dest);
}

/* Lê inteiro com validação; retorna o valor lido */
int lerInt(const char *prompt, int min, int max) {
    char buffer[64];
    int valor;
    while (1) {
        printf("%s", prompt);
        if (fgets(buffer, sizeof(buffer), stdin) == NULL) {
            /* EOF -> retorno min */
            return min;
        }
        if (sscanf(buffer, "%d", &valor) != 1) {
            printf("Entrada inválida. Digite um número inteiro.\n");
            continue;
        }
        if (valor < min || valor > max) {
            printf("Valor fora do intervalo [%d..%d]. Tente novamente.\n", min, max);
            continue;
        }
        return valor;
    }
}

/* ---------- Operações básicas (vetor) ---------- */

/* Inserir item na mochila (vetor) */
void inserirItem(void) {
    if (numItens >= MAX_ITENS) {
        printf("Mochila cheia. Remova um item antes de adicionar outro.\n");
        return;
    }
    Item it;
    lerString("Nome do item: ", it.nome, MAX_NOME);
    if (strlen(it.nome) == 0) {
        printf("Nome inválido. Operação cancelada.\n");
        return;
    }
    lerString("Tipo do item (ex: arma, munição, cura): ", it.tipo, MAX_TIPO);
    it.quantidade = lerInt("Quantidade (>=1): ", 1, 1000000);
    it.prioridade = lerInt("Prioridade (1 - menor .. 10 - maior): ", 1, 10);

    mochila[numItens++] = it;
    ordenadaPorNome = false; /* Após inserção, a ordenação por nome pode estar inválida */
    printf("Item inserido com sucesso.\n");
}

/* Remover item por nome (procura primeira ocorrência) */
void removerItem(void) {
    if (numItens == 0) {
        printf("Mochila vazia. Nada para remover.\n");
        return;
    }
    char nomeBusca[MAX_NOME];
    lerString("Informe o NOME do item a remover: ", nomeBusca, MAX_NOME);
    if (strlen(nomeBusca) == 0) {
        printf("Nome inválido. Operação cancelada.\n");
        return;
    }
    int idx = -1;
    for (int i = 0; i < numItens; ++i) {
        if (strcmp(mochila[i].nome, nomeBusca) == 0) {
            idx = i;
            break;
        }
    }
    if (idx == -1) {
        printf("Item \"%s\" não encontrado na mochila.\n", nomeBusca);
        return;
    }
    /* Desloca elementos para preencher lacuna */
    for (int i = idx; i < numItens - 1; ++i) {
        mochila[i] = mochila[i+1];
    }
    numItens--;
    printf("Item \"%s\" removido com sucesso.\n", nomeBusca);
    /* Após remoção, ordenação por nome pode permanecer válida se estava true; porém para simplicidade deixamos como estava */
}

/* Listar todos os itens */
void listarItens(void) {
    if (numItens == 0) {
        printf("Mochila vazia.\n");
        return;
    }
    printf("\n----------------- ITENS NA MOCHILA -----------------\n");
    printf("%-3s | %-28s | %-12s | %-9s | %-9s\n", "ID", "NOME", "TIPO", "QUANTIDADE", "PRIORIDADE");
    printf("---------------------------------------------------------------\n");
    for (int i = 0; i < numItens; ++i) {
        printf("%-3d | %-28s | %-12s | %-9d | %-9d\n", i, mochila[i].nome, mochila[i].tipo, mochila[i].quantidade, mochila[i].prioridade);
    }
    printf("---------------------------------------------------------------\n");
}

/* Busca sequencial por nome (retorna índice ou -1); incrementa comparacoesBuscaSequencial */
int buscarSequencial(const char *nome) {
    comparacoesBuscaSequencial = 0;
    for (int i = 0; i < numItens; ++i) {
        comparacoesBuscaSequencial++;
        if (strcmp(mochila[i].nome, nome) == 0) {
            return i;
        }
    }
    return -1;
}

/* Busca binária por nome (requer que mochila esteja ordenada por nome). Retorna índice ou -1.
   Conta comparações em comparacoesBuscaBinaria. */
int buscaBinariaPorNome(const char *nome) {
    comparacoesBuscaBinaria = 0;
    int low = 0, high = numItens - 1;
    while (low <= high) {
        int mid = (low + high) / 2;
        comparacoesBuscaBinaria++;
        int cmp = strcmp(mochila[mid].nome, nome);
        if (cmp == 0) return mid;
        if (cmp < 0) low = mid + 1;
        else high = mid - 1;
    }
    return -1;
}

/* ---------- Ordenações ---------- */

/* Bubble sort por NOME (alfabético ascendente).
   Conta comparações em comparacoesBubbleNome. Retorna tempo de execução em segundos. */
double bubbleSortPorNome(void) {
    comparacoesBubbleNome = 0;
    clock_t start = clock();
    if (numItens <= 1) {
        ordenadaPorNome = true;
        return 0.0;
    }
    for (int i = 0; i < numItens - 1; ++i) {
        bool trocou = false;
        for (int j = 0; j < numItens - 1 - i; ++j) {
            comparacoesBubbleNome++;
            if (strcmp(mochila[j].nome, mochila[j+1].nome) > 0) {
                Item tmp = mochila[j];
                mochila[j] = mochila[j+1];
                mochila[j+1] = tmp;
                trocou = true;
            }
        }
        if (!trocou) break;
    }
    ordenadaPorNome = true;
    clock_t end = clock();
    return (double)(end - start) / CLOCKS_PER_SEC;
}

/* Insertion sort por TIPO (alfabético ascendente).
   Conta comparações em comparacoesInsertionTipo. Retorna tempo em segundos. */
double insertionSortPorTipo(void) {
    comparacoesInsertionTipo = 0;
    clock_t start = clock();
    for (int i = 1; i < numItens; ++i) {
        Item chave = mochila[i];
        int j = i - 1;
        /* comparar mochila[j].tipo > chave.tipo */
        while (j >= 0) {
            comparacoesInsertionTipo++;
            if (strcmp(mochila[j].tipo, chave.tipo) > 0) {
                mochila[j+1] = mochila[j];
                j--;
            } else {
                break;
            }
        }
        mochila[j+1] = chave;
    }
    /* após ordenar por tipo, não garantimos ordenadaPorNome */
    ordenadaPorNome = false;
    clock_t end = clock();
    return (double)(end - start) / CLOCKS_PER_SEC;
}

/* Selection sort por PRIORIDADE (ordem decrescente: maior prioridade primeiro).
   Conta comparações em comparacoesSelectionPrioridade. Retorna tempo em segundos. */
double selectionSortPorPrioridade(void) {
    comparacoesSelectionPrioridade = 0;
    clock_t start = clock();
    for (int i = 0; i < numItens - 1; ++i) {
        int idxMax = i;
        for (int j = i + 1; j < numItens; ++j) {
            comparacoesSelectionPrioridade++;
            if (mochila[j].prioridade > mochila[idxMax].prioridade) {
                idxMax = j;
            }
        }
        if (idxMax != i) {
            Item tmp = mochila[i];
            mochila[i] = mochila[idxMax];
            mochila[idxMax] = tmp;
        }
    }
    ordenadaPorNome = false;
    clock_t end = clock();
    return (double)(end - start) / CLOCKS_PER_SEC;
}

/* Menu de ordenação: escolhe critério e executa algoritmo correspondente, mostrando comparações e tempo */
void menuOrdenacao(void) {
    if (numItens == 0) {
        printf("Mochila vazia. Não há itens para ordenar.\n");
        return;
    }
    printf("\n--- Menu de Ordenação ---\n");
    printf("1 - Ordenar por NOME (Bubble Sort)\n");
    printf("2 - Ordenar por TIPO (Insertion Sort)\n");
    printf("3 - Ordenar por PRIORIDADE (Selection Sort - decrescente)\n");
    printf("0 - Voltar\n");
    int op = lerInt("Escolha: ", 0, 3);
    if (op == 0) return;

    if (op == 1) {
        double t = bubbleSortPorNome();
        printf("Ordenação por NOME concluída (Bubble Sort).\n");
        printf("Comparações realizadas: %ld\n", comparacoesBubbleNome);
        printf("Tempo: %.6f s\n", t);
    } else if (op == 2) {
        double t = insertionSortPorTipo();
        printf("Ordenação por TIPO concluída (Insertion Sort).\n");
        printf("Comparações realizadas: %ld\n", comparacoesInsertionTipo);
        printf("Tempo: %.6f s\n", t);
    } else if (op == 3) {
        double t = selectionSortPorPrioridade();
        printf("Ordenação por PRIORIDADE concluída (Selection Sort - decrescente).\n");
        printf("Comparações realizadas: %ld\n", comparacoesSelectionPrioridade);
        printf("Tempo: %.6f s\n", t);
    } else {
        printf("Opção inválida.\n");
    }
}

/* ---------- Funções de busca e interface ---------- */

/* Executa busca sequencial com relatório */
void acaoBuscarSequencial(void) {
    if (numItens == 0) {
        printf("Mochila vazia.\n");
        return;
    }
    char nomeBusca[MAX_NOME];
    lerString("Informe o NOME do item para busca sequencial: ", nomeBusca, MAX_NOME);
    if (strlen(nomeBusca) == 0) {
        printf("Nome inválido.\n");
        return;
    }
    clock_t start = clock();
    int idx = buscarSequencial(nomeBusca);
    clock_t end = clock();
    double tempo = (double)(end - start) / CLOCKS_PER_SEC;
    printf("Comparações na busca sequencial: %ld\n", comparacoesBuscaSequencial);
    printf("Tempo gasto: %.6f s\n", tempo);
    if (idx >= 0) {
        printf("Item encontrado no índice %d:\n", idx);
        printf(" Nome: %s\n Tipo: %s\n Quantidade: %d\n Prioridade: %d\n",
               mochila[idx].nome, mochila[idx].tipo, mochila[idx].quantidade, mochila[idx].prioridade);
    } else {
        printf("Item \"%s\" não encontrado.\n", nomeBusca);
    }
}

/* Executa busca binária com relatório (requer ordenadaPorNome == true) */
void acaoBuscarBinaria(void) {
    if (numItens == 0) {
        printf("Mochila vazia.\n");
        return;
    }
    if (!ordenadaPorNome) {
        printf("A busca binária exige que os itens estejam ordenados por nome.\n");
        printf("Deseja ordenar agora por NOME (Bubble Sort)? 1-Sim / 0-Não: ");
        int resp = lerInt("", 0, 1);
        if (resp == 1) {
            double t = bubbleSortPorNome();
            printf("Ordenação por NOME concluída. Comparações: %ld Tempo: %.6f s\n", comparacoesBubbleNome, t);
        } else {
            printf("Operação cancelada. Ordene os itens por nome antes de usar busca binária.\n");
            return;
        }
    }
    char nomeBusca[MAX_NOME];
    lerString("Informe o NOME do item para busca binária: ", nomeBusca, MAX_NOME);
    if (strlen(nomeBusca) == 0) {
        printf("Nome inválido.\n");
        return;
    }
    clock_t start = clock();
    int idx = buscaBinariaPorNome(nomeBusca);
    clock_t end = clock();
    double tempo = (double)(end - start) / CLOCKS_PER_SEC;
    printf("Comparações na busca binária: %ld\n", comparacoesBuscaBinaria);
    printf("Tempo gasto: %.6f s\n", tempo);
    if (idx >= 0) {
        printf("Item encontrado no índice %d:\n", idx);
        printf(" Nome: %s\n Tipo: %s\n Quantidade: %d\n Prioridade: %d\n",
               mochila[idx].nome, mochila[idx].tipo, mochila[idx].quantidade, mochila[idx].prioridade);
    } else {
        printf("Item \"%s\" não encontrado.\n", nomeBusca);
    }
}

/* ---------- Função main: loop do programa ---------- */

int main(void) {
    /* Inicialização */
    numItens = 0;
    ordenadaPorNome = false;
    comparacoesBuscaSequencial = comparacoesBuscaBinaria = 0;
    comparacoesBubbleNome = comparacoesInsertionTipo = comparacoesSelectionPrioridade = 0;

    /* Menu principal */
    while (1) {
        exibirMenu();
        int opcao = lerInt("", 0, 6);
        /* Consome a linha restante (caso) - leitura já faz isso */
        switch (opcao) {
            case 1:
                inserirItem();
                break;
            case 2:
                removerItem();
                break;
            case 3:
                listarItens();
                break;
            case 4:
                menuOrdenacao();
                break;
            case 5:
                acaoBuscarSequencial();
                break;
            case 6:
                acaoBuscarBinaria();
                break;
            case 0:
                printf("Encerrando o programa. Até a próxima!\n");
                return 0;
            default:
                printf("Opção inválida. Tente novamente.\n");
        }
        printf("\nPressione Enter para continuar...");
        getchar(); /* pausa simples */
        limparTela();
    }

    return 0;
}
