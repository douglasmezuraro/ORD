#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

// bibliotecas personalizadas
#include "uteis.h"
#include "registro.h"

// Constantes uteis
#define C_NOME_FILE_DADOS     "dados-inline.txt"
#define C_NOME_FILE_REGISTROS "registros.txt"

// Constantes do menu de opções
#define C_PARAR_EXECUCAO 0
#define C_IMPORTAR       1
#define C_BUSCAR         2
#define C_INSERIR        3
#define C_REMOVER        4

/// Requisitos
void importar();
bool buscar(char inscricao[]);
void inserir();
bool remover(char inscricao[]);
/// Métodos utéis
void setInscricao(char inscricao[]);
void mostrarMenu();
int selecionarOpcao();
/// Método principal
int main();

void importar() {
    FILE * fDados = fopen(C_NOME_FILE_DADOS, "r");
    FILE * fRegistros = fopen(C_NOME_FILE_REGISTROS, "w");

    char buffer[C_QTD_CAMPOS * C_TAMANHO_CAMPO];

    setLedHead(-1l, fRegistros);
    Registro reg = getRegistro(fDados);

    while(assigned(reg)) {
        registroToString(reg, buffer, true);
        fputs(buffer, fRegistros);
        reg = getRegistro(fDados);
    }

    fclose(fDados);
    fclose(fRegistros);
}

bool buscar(char inscricao[]) {
    FILE * fd = fopen(C_NOME_FILE_REGISTROS, "r");

    long byteOffset = buscarPorInscricao(inscricao, fd);

    if(byteOffset == -1) {
        fclose(fd);
        return false;
    }
    else {
        fseek(fd, byteOffset, SEEK_SET);
        printRegistro(getRegistro(fd));
        fclose(fd);
        return true;
    }
}

void inserir() {
    FILE * fd = fopen(C_NOME_FILE_REGISTROS, "rw+");

    char buffer[C_QTD_CAMPOS * C_TAMANHO_CAMPO];

    Registro reg = popularRegistro();

    long byteOffset = getByteOffsetInsercao(atoi(reg.tamanho), getLedHead(fd), fd);
    registroToString(reg, buffer, byteOffset == -1);
    fseek(fd, byteOffset, SEEK_SET);
    fputs(buffer, fd);

    fclose(fd);
}

bool remover(char inscricao[]) {
    FILE * fd = fopen(C_NOME_FILE_REGISTROS, "rw+");

    long byteOffset = buscarPorInscricao(inscricao, fd);

    if(byteOffset == -1) {
        fclose(fd);
        return false;
    }
    else {
        apontarPraLEDHead(byteOffset, fd);
        setLedHead(byteOffset, fd);
        fclose(fd);
        return true;
    }
}

void setInscricao(char inscricao[]) {
    limparString(inscricao);
    limparBuffer();

    puts("Digite a chave:");
    gets(inscricao);
}

void mostrarMenu() {
    const int C_TAM_INSCRICAO = 10;
    char inscricao[C_TAM_INSCRICAO];

    switch(selecionarOpcao()) {
        case C_PARAR_EXECUCAO:
            exit(EXIT_SUCCESS);
            break;

        case C_IMPORTAR:
            importar();
            puts("Dados importados com sucesso!");
            break;

        case C_BUSCAR:
            setInscricao(inscricao);
            if(!buscar(inscricao))
                puts("Registro nao encontrdo!");
            break;

        case C_INSERIR:
            inserir();
            puts("Dados inseridos com sucesso!");
            break;

        case C_REMOVER:
            setInscricao(inscricao);
            if(remover(inscricao))
                puts("Registro removido com sucesso!");
            else
                puts("Registro nao encontrado!");
            break;

        default:
            puts("Opcao invalida!");
    }

    getchar();

    puts("Pressione qualquer tecla para continuar...");
    getchar();

    system("cls");
    mostrarMenu();
}

int selecionarOpcao() {
    puts("MENU:                  ");
    puts("  1 - Importar.        ");
    puts("  2 - Buscar.          ");
    puts("  3 - Inserir.         ");
    puts("  4 - Remover.         ");
    puts("                       ");
    puts("  0 - Sair.            ");
    puts("                       ");
    puts("> Selecione uma opcao: ");

    int aux = -1;

    scanf("%i", &aux);

    return aux;
}

int main() {
    mostrarMenu();
    exit(EXIT_SUCCESS);
}
