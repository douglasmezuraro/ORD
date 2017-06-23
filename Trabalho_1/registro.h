#ifndef _REGISTRO_H_
#define _REGISTRO_H_

#define C_TAMANHO_CAMPO 40
#define C_QTD_CAMPOS    4

#define C_CAMPO_INSCRICAO 1
#define C_CAMPO_NOME      2
#define C_CAMPO_CURSO     3
#define C_CAMPO_SCORE     4

typedef struct {
    char inscricao[C_TAMANHO_CAMPO];
    char nome[C_TAMANHO_CAMPO];
    char curso[C_TAMANHO_CAMPO];
    char score[C_TAMANHO_CAMPO];
    // campo auxiliar
    char tamanho[sizeof(int)];
} Registro;

// Declarações
void printRegistro(Registro reg);
bool assigned(Registro reg);
Registro newRegistro();
Registro stringToRegistro(char str[]);
void removerPipeRegistro(Registro * reg);
void setTamanhoRegistro(Registro * reg);
Registro getRegistro(FILE * arquivo);
void lerCampo(char campo[], char delimitador, FILE * arquivo);
void getLED(FILE * arquivo, char led[]);
void posicinarNoPrimeiroRegistro(FILE * arquivo);

// Implementações
void printRegistro(Registro reg) {
    printf("\nREGISTRO:");
    printf("\n  > INSCRICAO = %s",   reg.inscricao);
    printf("\n  > NOME      = %s",   reg.nome);
    printf("\n  > CURSO     = %s",   reg.curso);
    printf("\n  > SCORE     = %s\n", reg.score);
}

bool assigned(Registro reg) {
    return strcasecmp(reg.tamanho, "0") != 0;
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

void removerPipeRegistro(Registro * reg) {
    removerCaractere(reg->inscricao, '|');
    removerCaractere(reg->nome, '|');
    removerCaractere(reg->curso, '|');
    removerCaractere(reg->score, '|');
}

void setTamanhoRegistro(Registro * reg) {
    const C_BASE_DECIMAL = 10;

    int tam = strlen(reg->inscricao)
            + strlen(reg->nome)
            + strlen(reg->curso)
            + strlen(reg->score);

    itoa(tam, reg->tamanho, C_BASE_DECIMAL);
}

Registro getRegistro(FILE * arquivo) {
    Registro reg = newRegistro();

    lerCampo(reg.inscricao, '|', arquivo);
    lerCampo(reg.nome, '|', arquivo);
    lerCampo(reg.curso, '|', arquivo);
    lerCampo(reg.score, '|', arquivo);

    setTamanhoRegistro(&reg);

    removerPipeRegistro(&reg);

    return reg;
}

void lerCampo(char campo[], char delimitador, FILE * arquivo) {
    int i = 0;
    char flag;

    limparString(campo);

    if(flag = fgetc(arquivo) != EOF)
        fseek(arquivo, -1l, SEEK_CUR);

    if(feof(arquivo) == 0) {
      do {
          campo[i] = fgetc(arquivo);
          i++;
      } while(campo[i - 1] != delimitador);
    }

    campo[i] = '\0'; // finaliza a string para evitar lixo
}

void getLED(FILE * arquivo, char led[]) {
    lerCampo(led, ']', arquivo);

    if(stringsIguais(led, C_EMPTY_STRING))
      strcpy(led, "[LED=*-1]");
}

void posicinarNoPrimeiroRegistro(FILE * arquivo) {
  char aux;
  rewind(arquivo);
  do {
    aux = fgetc(arquivo);
  } while(aux != ']');
}

#endif
