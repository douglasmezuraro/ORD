#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

// Constantes uteis
#define C_NOME_FILE_DADOS     "dados-inline.txt"
#define C_NOME_FILE_REGISTROS "registros.txt"
#define C_PIPE                '|'
#define C_EMPTY_STRING        ""
#define C_TAMANHO_CAMPO       40
#define C_QTD_CAMPOS          4
// Constantes do menu de opções
#define C_PARAR_EXECUCAO 0
#define C_IMPORTAR       1
#define C_BUSCAR         2
#define C_INSERIR        3
#define C_REMOVER        4
// Constantes de posições dos campos de um registro
#define C_CAMPO_INSCRICAO 1
#define C_CAMPO_NOME      2
#define C_CAMPO_CURSO     3
#define C_CAMPO_SCORE     4

// Declarações de tipos
typedef struct {
    // campos auxiliares
    char tamanho[sizeof(int)];
    // campos necessários
    char inscricao[C_TAMANHO_CAMPO];
    char nome[C_TAMANHO_CAMPO];
    char curso[C_TAMANHO_CAMPO];
    char score[C_TAMANHO_CAMPO];
} Registro;

// REQUISITOS
void importar();
void buscar();
void inserir();
void inserir();

// MENU
void mostrarMenu();
int selecionarOpcao();
// METODOS UTEIS DE UM REGISTRO
void removerPipeRegistro(Registro * registro);
void printRegistro(Registro registro);
void setTamanho(Registro * reg);
Registro getRegistro(FILE * arquivo);
Registro getRegistroBusca(FILE * arquivo);
void lerCampo(char campo[], FILE * arquivo);
void registroToString(Registro reg, char str[]);
Registro newRegistro();
bool assigned(Registro reg);
// METODOS UTEIS
void removerPipeString(char str[]);
void limparBuffer();
void limparString(char str[]);
void getLED(FILE * arquivo, char led[]);
void posicinarNoPrimeiroRegistro(FILE * arquivo);
//
int main();
// REQUISITOS

void importar() {
    FILE * fDados = fopen(C_NOME_FILE_DADOS, "r");
    FILE * fRegistros = fopen(C_NOME_FILE_REGISTROS, "a+");

    char buffer[C_QTD_CAMPOS * C_TAMANHO_CAMPO],
         led[10];

    limparString(buffer);
    limparString(led);

    rewind(fDados);
    rewind(fRegistros);

    getLED(fRegistros, led);
    fputs(led, fRegistros);

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
    char inscricao[C_TAMANHO_CAMPO];

    limparBuffer();

    puts("Qual inscricao deseja buscar?");
    gets(inscricao);

    char tam[C_TAMANHO_CAMPO],
         key[C_TAMANHO_CAMPO],
         buffer[1000],
         campo[200];

    limparString(tam); limparString(key); limparString(buffer); limparString(campo);

    posicinarNoPrimeiroRegistro(fd);

    long p = -1;
    int iTam = 0;

    while(true) {
      lerCampo(tam, fd);
      removerPipeString(tam);
      iTam = atoi(tam);
      p = ftell(fd);

      lerCampo(key, fd);
      removerPipeString(key);

      fseek(fd, p, SEEK_SET);    // posiciona no campo lido

      if(strcasecmp(key, inscricao) == 0) {
        int j;
        for(j = 1; j <= C_QTD_CAMPOS; j++) {
          lerCampo(campo, fd);
          strcat(buffer, campo);
        }
        break;
      }
      else fseek(fd, iTam, SEEK_CUR); // posciona no proximo campo pulando o tamanho do tamanho lid
    }
    puts("OCORRENCIA: ");
    puts(buffer);
}

void inserir() {
    Registro reg = newRegistro();

    limparBuffer();

    printf("\nDigite a inscricao:\n  > ");
    gets(reg.inscricao);

    printf("\nDigite o nome:\n  > ");
    gets(reg.nome);

    printf("\nDigite o curso:\n  > ");
    gets(reg.curso);

    printf("\nDigite o score:\n  > ");
    gets(reg.score);

    char str[C_QTD_CAMPOS * C_TAMANHO_CAMPO];
    limparString(str);

    setTamanho(&reg);
    registroToString(reg, str);
    puts("STRING:");
    puts(str);

    printRegistro(reg);
}

void remover() {
    // TODO: Implementar o método de remoção
}

// MENU

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

// METODOS UTEIS DE UM REGISTRO

void removerPipeRegistro(Registro * registro) {
    removerPipeString(registro->inscricao);
    removerPipeString(registro->nome);
    removerPipeString(registro->curso);
    removerPipeString(registro->score);
}

void printRegistro(Registro registro) {
    printf("\nREGISTRO:");
    printf("\n  > INSCRICAO = %s", registro.inscricao);
    printf("\n  > NOME      = %s", registro.nome);
    printf("\n  > CURSO     = %s", registro.curso);
    printf("\n  > SCORE     = %s\n", registro.score);
}

void setTamanho(Registro * reg) {
    const C_BASE_DECIMAL = 10;

    int tam = strlen(reg->inscricao)
            + strlen(reg->nome)
            + strlen(reg->curso)
            + strlen(reg->score);

    itoa(tam, reg->tamanho, C_BASE_DECIMAL);
}

Registro getRegistro(FILE * arquivo) {
    Registro reg = newRegistro();

    lerCampo(reg.inscricao, arquivo);
    lerCampo(reg.nome, arquivo);
    lerCampo(reg.curso, arquivo);
    lerCampo(reg.score, arquivo);

    setTamanho(&reg);

    removerPipeRegistro(&reg);

    return reg;
}

Registro getRegistroBusca(FILE * arquivo) {
    Registro reg = newRegistro();

    lerCampo(reg.tamanho, arquivo);
    lerCampo(reg.inscricao, arquivo);
    lerCampo(reg.nome, arquivo);
    lerCampo(reg.curso, arquivo);
    lerCampo(reg.score, arquivo);

    removerPipeRegistro(&reg);

    return reg;
}


void lerCampo(char campo[], FILE * arquivo) {
    int i = 0;
    char flag;

    limparString(campo);

    if(flag = fgetc(arquivo) != EOF)
        fseek(arquivo, -1l, SEEK_CUR);

    if(feof(arquivo) == 0) {
      do {
          campo[i] = fgetc(arquivo);
          i++;
      } while(campo[i - 1] != C_PIPE);
    }

    campo[i] = '\0'; // finaliza a string para evitar lixo
}

void registroToString(Registro reg, char str[]) {
    strcat(str, reg.tamanho);
    strcat(str, "|");
    strcat(str, reg.inscricao);
    strcat(str, "|");
    strcat(str, reg.nome);
    strcat(str, "|");
    strcat(str, reg.curso);
    strcat(str, "|");
    strcat(str, reg.score);
    strcat(str, "|");
}

Registro newRegistro() {
    Registro reg;

    limparString(reg.inscricao);
    limparString(reg.nome);
    limparString(reg.curso);
    limparString(reg.score);

    strcpy(reg.tamanho, "0");

    return reg;
}

bool assigned(Registro reg) {
    return strcasecmp(reg.tamanho, "0") != 0;
}

// METODOS UTEIS

void removerPipeString(char str[]) {
    int i;

    for(i = 0; i < strlen(str); i++) {
        if(str[i] == C_PIPE)
            str[i] = '\0';
    }
}

void limparBuffer() {
    fflush(stdin);
}

void limparString(char str[]) {
    str[0] = '\0';
}

void getLED(FILE * arquivo, char led[])
// Esse método atribui ao parâmetro led uma string que contenha a led do arquivo onde
// caso o arquivo não exista é retornado a led apontando para -1
{
    int i = 0;

    limparString(led);

    led[i] = fgetc(arquivo);

    if(led[i] != EOF) {
        while(led[i] != ']') {
          i++;
          led[i] = fgetc(arquivo);
        }
        led[i] = '\0';
    }
    else strcpy(led, "[LED=*-1]");
}

void posicinarNoPrimeiroRegistro(FILE * arquivo)
// OBJETIVO : posicionar o ponteiro do arquivo depois da LED
{
  char aux;
  rewind(arquivo);
  do {
    aux = fgetc(arquivo);
  } while(aux != ']');
}

// METODO PRINCIPAL

int main() {
    mostrarMenu();
    exit(EXIT_SUCCESS);
}
