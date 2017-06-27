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

// REQUISITOS
void importar();
bool buscar();
void inserir();
bool remover();
//
void setChave(char chave[]);
// MENU
void mostrarMenu();
int selecionarOpcao();
//
int main();

void importar() {
    FILE * fDados = fopen(C_NOME_FILE_DADOS, "r");
    FILE * fRegistros = fopen(C_NOME_FILE_REGISTROS, "w");

    char buffer[C_QTD_CAMPOS * C_TAMANHO_CAMPO];

    // escreve a posição inicial da LED
    setLedHead(-1l, fRegistros);

    // lê o registro do arquivo de dados e popula o registro
    Registro reg = getRegistro(fDados);

    // enquanto o arquivo de dados tiver registros para ser lido faça
    while(assigned(reg)) {
        // converte o registro em string para que poss
        registroToString(reg, buffer, true);

        // escreve a string sequencialmente no arquivo
        fputs(buffer, fRegistros);

        // lê o próximo registro do arquivo de dados
        reg = getRegistro(fDados);
    }

    fclose(fDados);
    fclose(fRegistros);
}

bool buscar() {
    FILE * fd = fopen(C_NOME_FILE_REGISTROS, "r");

    char chave[C_TAMANHO_CAMPO];

    // popula a string chave com o dado digitado pelo usuário
    setChave(chave);

    // obtém o byteoffset do registro que tem a inscrição que foi anteriormente perguntada
    long byteOffset = buscarPorInscricao(chave, fd);

    if(byteOffset == -1)
        // se o byteoffset for igual a -1 o registro não existe no arquivo
        return false;
    else {
        // posiciona o ponteiro no arquivo no início do registro que está sendo procurado
        fseek(fd, byteOffset, SEEK_SET);

        // printa o registro na tela
        printRegistro(getRegistro(fd));

        // retorna sucesso
        return true;
    }

    fclose(fd);
}

void inserir() {
    FILE * fd = fopen(C_NOME_FILE_REGISTROS, "rw+");

    char buffer[C_QTD_CAMPOS * C_TAMANHO_CAMPO];

    // popula a string que será inserida no arquivo com os dados digitados pelo usuário
    Registro reg = popularRegistro();
    registroToString(reg, buffer, false);

   long byteOffset = getByteOffsetInsercao(atoi(reg.tamanho), getLedHead(fd), fd);
    fseek(fd, byteOffset, SEEK_SET);
    fputs(buffer, fd);

    fclose(fd);
}

bool remover() {
    FILE * fd = fopen(C_NOME_FILE_REGISTROS, "rw+");

    char chave[C_TAMANHO_CAMPO];

    // popula a string chave com o dado digitado pelo usuário
    setChave(chave);

    // obtém o byteoffset do registro a ser removido
    long byteOffset = buscarPorInscricao(chave, fd);


    if(byteOffset == -1)
        // retorna falha por não encontrar o registro que seria removido
        return false;
    else {
        // sinaliza que o registro foi removido escrevendo nele a cabeça da LED
        apontarPraLEDHead(byteOffset, fd);

        // muda a cabeça da LED com o byte offset do registro removido
        setLedHead(byteOffset, fd);

        // retorna que o registro foi removido do arquivo
        return true;
    }

    fclose(fd);
}

void setChave(char chave[]) {
    // inicializa a string
    limparString(chave);

    // remove os possíveis lixos no buffer
    limparBuffer();

    // obtem a insrição a ser procurada
    puts("Qual inscricao deseja buscar?");
    gets(chave);
}

void mostrarMenu() {
    limparBuffer();
    switch(selecionarOpcao()) {
        case C_PARAR_EXECUCAO:
            exit(EXIT_SUCCESS);
            break;
        case C_IMPORTAR:
            importar();
            puts("Dados importados com sucesso!");
            break;
        case C_BUSCAR:
            if(!buscar())
                puts("Registro nao encontrdo!");
            break;
        case C_INSERIR:
            inserir();
            puts("Dados inseridos com sucesso!");
            break;
        case C_REMOVER:
            if(remover())
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
