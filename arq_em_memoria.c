/*
 * Mini Sistema de Arquivos em Memória - COMPLETO
 * Autor: Rafael Cunha (exemplo)
 * Compilar com: gcc -o mini_fs mini_fs.c
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

// Definições de limites
#define MAX_NOME 50
#define MAX_ARQUIVOS 100
#define MAX_CONTEUDO 1024
#define MAX_DIRETORIOS 100
#define MAX_BLOCOS 10000
#define TAM_BLOCO 64

// Tipos de arquivos simulados
typedef enum { NUMERICO, CARACTERE, BINARIO, PROGRAMA } TipoArquivo;

// Estrutura de permissões RWX usando bitfields
typedef struct {
    int read : 1;
    int write : 1;
    int execute : 1;
} Permissao;

// Estrutura do File Control Block (FCB) simulado
typedef struct {
    char nome[MAX_NOME];
    int tamanho;
    TipoArquivo tipo;
    time_t criado, modificado, acessado;
    int id;
    Permissao permissoes;
    char conteudo[MAX_CONTEUDO];
    int blocos[100];
    int num_blocos;
} Arquivo;

// Estrutura de diretório com suporte a subdiretórios e arquivos
typedef struct Diretorio {
    char nome[MAX_NOME];
    Arquivo* arquivos[MAX_ARQUIVOS];
    int qtd_arquivos;
    struct Diretorio* subdirs[MAX_DIRETORIOS];
    int qtd_subdirs;
    struct Diretorio* pai;    // Ponteiro para o diretório pai
} Diretorio;

Diretorio* raiz;
Diretorio* atual;
int contador_id = 1;
char disco[MAX_BLOCOS * TAM_BLOCO];
int blocos_livres[MAX_BLOCOS];
char usuario_atual[MAX_NOME] = "admin";

// Inicializa o sistema de arquivos em memória
void init_fs() {
    raiz = (Diretorio*)malloc(sizeof(Diretorio));
    strcpy(raiz->nome, "/");
    raiz->qtd_arquivos = 0;
    raiz->qtd_subdirs = 0;
    raiz->pai = NULL;
    atual = raiz;
    for (int i = 0; i < MAX_BLOCOS; i++) blocos_livres[i] = 1;
}

// Procura e aloca o primeiro bloco livre
int alocar_bloco() {
    for (int i = 0; i < MAX_BLOCOS; i++) {
        if (blocos_livres[i]) {
            blocos_livres[i] = 0;
            return i;
        }
    }
    return -1;
}

// Cria um novo diretório dentro do diretório atual
void mkdir_simples(char nome[]) {
    Diretorio* novo = malloc(sizeof(Diretorio));
    strcpy(novo->nome, nome);
    novo->qtd_arquivos = 0;
    novo->qtd_subdirs = 0;
    novo->pai = atual;
    atual->subdirs[atual->qtd_subdirs++] = novo;
    printf("Diretório '%s' criado.\n", nome);
}

// Altera o diretório atual
void cd(char nome[]) {
    if (strcmp(nome, "..") == 0 && atual->pai != NULL) {
        atual = atual->pai;
    } else {
        for (int i = 0; i < atual->qtd_subdirs; i++) {
            if (strcmp(atual->subdirs[i]->nome, nome) == 0) {
                atual = atual->subdirs[i];
                return;
            }
        }
        printf("Diretório não encontrado.\n");
    }
}

// Lista diretórios e arquivos no diretório atual
void ls() {
    printf("Diretório atual: %s\n", atual->nome);
    for (int i = 0; i < atual->qtd_subdirs; i++) {
        printf("[DIR] %s\n", atual->subdirs[i]->nome);
    }
    for (int i = 0; i < atual->qtd_arquivos; i++) {
        printf("[ARQ] %s\n", atual->arquivos[i]->nome);
    }
}

// Cria um arquivo vazio com metadados iniciais
void touch(char nome[]) {
    Arquivo* arq = malloc(sizeof(Arquivo));
    strcpy(arq->nome, nome);
    arq->tamanho = 0;
    arq->tipo = CARACTERE;
    arq->criado = arq->modificado = arq->acessado = time(NULL);
    arq->id = contador_id++;
    arq->permissoes.read = 1;
    arq->permissoes.write = 1;
    arq->permissoes.execute = 0;
    arq->num_blocos = 0;
    strcpy(arq->conteudo, "");
    atual->arquivos[atual->qtd_arquivos++] = arq;
    printf("Arquivo '%s' criado.\n", nome);
}

// Escreve conteúdo em um arquivo, simulando alocação de blocos
void echo(char nome[], char conteudo[]) {
    for (int i = 0; i < atual->qtd_arquivos; i++) {
        Arquivo* arq = atual->arquivos[i];
        if (strcmp(arq->nome, nome) == 0) {
            if (arq->permissoes.write) {
                strcpy(arq->conteudo, conteudo);
                arq->modificado = time(NULL);
                arq->tamanho = strlen(conteudo);
                arq->num_blocos = 0;
                int blocos_necessarios = (arq->tamanho + TAM_BLOCO - 1) / TAM_BLOCO;
                for (int j = 0; j < blocos_necessarios; j++) {
                    int bloco = alocar_bloco();
                    if (bloco == -1) {
                        printf("Sem espaço em disco.\n");
                        return;
                    }
                    arq->blocos[arq->num_blocos++] = bloco;
                    strncpy(&disco[bloco * TAM_BLOCO], conteudo + j * TAM_BLOCO, TAM_BLOCO);
                }
                printf("Conteúdo escrito em blocos.\n");
                return;
            } else {
                printf("Permissão negada.\n");
                return;
            }
        }
    }
    printf("Arquivo não encontrado.\n");
}

// Lê o conteúdo de um arquivo acessando os blocos alocados
void cat(char nome[]) {
    for (int i = 0; i < atual->qtd_arquivos; i++) {
        Arquivo* arq = atual->arquivos[i];
        if (strcmp(arq->nome, nome) == 0) {
            if (arq->permissoes.read) {
                for (int j = 0; j < arq->num_blocos; j++) {
                    int bloco = arq->blocos[j];
                    fwrite(&disco[bloco * TAM_BLOCO], 1, TAM_BLOCO, stdout);
                }
                printf("\n");
                return;
            } else {
                printf("Permissão negada.\n");
                return;
            }
        }
    }
    printf("Arquivo não encontrado.\n");
}

// Remove um arquivo, liberando seus blocos do "disco"
void rm(char nome[]) {
    for (int i = 0; i < atual->qtd_arquivos; i++) {
        if (strcmp(atual->arquivos[i]->nome, nome) == 0) {
            for (int j = 0; j < atual->arquivos[i]->num_blocos; j++) {
                blocos_livres[atual->arquivos[i]->blocos[j]] = 1;
            }
            free(atual->arquivos[i]);
            for (int k = i; k < atual->qtd_arquivos - 1; k++) {
                atual->arquivos[k] = atual->arquivos[k + 1];
            }
            atual->qtd_arquivos--;
            printf("Arquivo removido.\n");
            return;
        }
    }
    printf("Arquivo não encontrado.\n");
}

// Altera as permissões de um arquivo usando valores numéricos RWX
void chmod_simples(char nome[], int rwx) {
    for (int i = 0; i < atual->qtd_arquivos; i++) {
        if (strcmp(atual->arquivos[i]->nome, nome) == 0) {
            atual->arquivos[i]->permissoes.read = (rwx & 4) != 0;
            atual->arquivos[i]->permissoes.write = (rwx & 2) != 0;
            atual->arquivos[i]->permissoes.execute = (rwx & 1) != 0;
            printf("Permissões atualizadas.\n");
            return;
        }
    }
    printf("Arquivo não encontrado.\n");
}

// MAIN
int main() {
    init_fs();
    char comando[100], arg1[100], arg2[1024];

    while (1) {
        printf("%s$ ", atual->nome);
        fgets(comando, 100, stdin);
        comando[strcspn(comando, "\n")] = 0;

        if (sscanf(comando, "mkdir %s", arg1) == 1) mkdir_simples(arg1);
        else if (sscanf(comando, "cd %s", arg1) == 1) cd(arg1);
        else if (strcmp(comando, "ls") == 0) ls();
        else if (sscanf(comando, "touch %s", arg1) == 1) touch(arg1);
        else if (strncmp(comando, "echo ", 5) == 0) {
                sscanf(comando + 5, "%s %[^\n]", arg1, arg2);
                echo(arg1, arg2);
                }
        else if (sscanf(comando, "cat %s", arg1) == 1) cat(arg1);
        else if (sscanf(comando, "rm %s", arg1) == 1) rm(arg1);
        else if (sscanf(comando, "chmod %d %s", &contador_id, arg1) == 2) chmod_simples(arg1, contador_id);
        else if (strcmp(comando, "exit") == 0) break;
        else printf("Comando não reconhecido.\n");
    }

    return 0;
}
