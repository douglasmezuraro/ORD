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
// Constantes do menu de op��es
#define C_PARAR_EXECUCAO 0
#define C_IMPORTAR       1
#define C_BUSCAR         2
#define C_INSERIR        3
#define C_REMOVER        4

// REQUISITOS
void importar();
void buscar();
void inserir();
void inserir();
// MENU
void mostrarMenu();
int selecionarOpcao();
//
int main();

void importar() {
    FILE * fDados = fopen(C_NOME_FILE_DADOS, "r");
    FILE * fRegistros = fopen(C_NOME_FILE_REGISTROS, "w");

    char buffer[C_QTD_CAMPOS * C_TAMANHO_CAMPO],
         cabecario[C_TAMANHO_CABECARIO];

    limparString(buffer);
    limparString(cabecario);

    rewind(fDados);
    rewind(fRegistros);

    strcpy(cabecario, "LED=-1");
    setCabecario(cabecario, fRegistros);
    fputs(cabecario, fRegistros);

    Registro reg = getRegistro(fDados);

    while(assigned(reg)) {
        registroToString(reg, buffer);
        fputs(buffer, fRegistros);
        limparString(buffer);
        reg = getRegistro(fDados);
    }
}

void buscar() {
    FILE * fd = fopen(C_NOME_FILE_REGISTROS, "r");
    Registro reg;

    bool match = false;
    char chave[C_TAMANHO_CAMPO],
         tamanho[C_TAMANHO_CAMPO],
         inscricao[C_TAMANHO_CAMPO];

    limparString(tamanho); limparString(inscricao); limparBuffer();

    puts("Qual inscricao deseja buscar?");
    gets(chave);

    fseek(fd, C_TAMANHO_CABECARIO, SEEK_SET);

    long byteOffset = -1;
    while(!match) {
      lerCampo(tamanho, '|', fd);

      byteOffset = ftell(fd);

      lerCampo(inscricao, '|', fd);
      removerCaractere(inscricao, '|');

      fseek(fd, byteOffset, SEEK_SET);

      if(stringsIguais(chave, inscricao)) {
        reg = getRegistro(fd);
        match = true;
      }
      else {
        int iTam = atoi(tamanho);
        fseek(fd, iTam, SEEK_CUR);
      }
    }
    printRegistro(reg);
}

void inserir() {
    FILE * fd = fopen(C_NOME_FILE_REGISTROS, "a+");
    Registro reg = newRegistro();

    limparBuffer();

    // Popula o registro
    printf("\nDigite a inscricao:\n  > ");
    gets(reg.inscricao);
    printf("\nDigite o nome:\n  > ");
    gets(reg.nome);
    printf("\nDigite o curso:\n  > ");
    gets(reg.curso);
    printf("\nDigite o score:\n  > ");
    gets(reg.score);

    // Popula o buffer
    char buffer[C_QTD_CAMPOS * C_TAMANHO_CAMPO]; limparString(buffer);

    setTamanhoRegistro(&reg);
    registroToString(reg, buffer);

    long byteOffset = getLED(fd);

    if(byteOffset == -1) {
        // inserir no final do arquivo
        fseek(fd, 0, SEEK_END);
        fputs(buffer, fd);
    }
    else {
        // TODO: PEGAR O PRIMEIRO ESPA�O DISPONIVEL DA LED QUE CAIBA O REGISTRO
    }

    //puts("STRING:");
    //puts(buffer);
    //printRegistro(reg);
}

void remover() {
    // TODO: Implementar o m�todo de remo��o
}

void mostrarMenu() {
    switch(selecionarOpcao()) {
        case C_PARAR_EXECUCAO:
            exit(EXIT_SUCCESS);
            break;
        case C_IMPORTAR:
            importar();
            puts("Dados importados com sucesso!");
            break;
        case C_BUSCAR:
            buscar();
            //puts("Dados buscados com sucesso!");
            break;
        case C_INSERIR:
            inserir();
            //puts("Dados inseridos com sucesso!");
            break;
        case C_REMOVER:
            remover();
            //puts("Dados removidos com sucesso!");
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
    puts("MENU:");
    puts("  1 - Importar.");
    puts("  2 - Buscar.");
    puts("  3 - Inserir.");
    puts("  4 - Remover.");
    puts("");
    puts("  0 - Sair.");
    puts("");
    puts("> Selecione uma opcao: ");

    int aux = -1;

    scanf("%i", &aux);

    return aux;
}

int main() {
    mostrarMenu();
    exit(EXIT_SUCCESS);
}
