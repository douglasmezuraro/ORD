#ifndef _REGISTRO_H_
#define _REGISTRO_H_

#define C_TAMANHO_CABECARIO 50
#define C_TAMANHO_CAMPO     40
#define C_QTD_CAMPOS        4

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

/// Declarações
Registro newRegistro();
void registroToString(Registro reg, char str[], bool concatenarTamanho);
int getTamanhoRegistro(Registro reg);
void setTamanhoRegistro(Registro * reg, int tam);
Registro getRegistro(FILE * arquivo);
void lerCampo(char campo[], char delimitador, FILE * arquivo);
long buscarPorInscricao(char chave[], FILE * arquivo);
bool fimArquivo(FILE * arquivo);
void popularBuffer(char buffer[]);
Registro popularRegistro();

/// Métodos utéis
bool assigned(Registro reg);
void printRegistro(Registro reg);
void removerPipeRegistro(Registro * reg);
long getByteOffsetFinal(FILE * arquivo);
void verificarArquivo(FILE * arquivo);

/// LED
long getLedHead(FILE * arquivo);
void setLedHead(long byteOffset, FILE * arquivo);
void apontarPraLEDHead(long byteOffset, FILE * arquivo);
long index(FILE * arquivo);
long getByteOffsetInsercao(int tamanhoRegistro, long pCandidato, FILE * arquivo);

/// Implementação

void printRegistro(Registro reg) {
    printf("\nREGISTRO:");
    printf("\n  > INSCRICAO = %s", reg.inscricao);
    printf("\n  > NOME      = %s", reg.nome);
    printf("\n  > CURSO     = %s", reg.curso);
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

void registroToString(Registro reg, char str[], bool concatenarTamanho) {
    limparString(str);

    if(concatenarTamanho) {
      strcat(str, reg.tamanho);
      strcat(str, "|");
    }

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

int getTamanhoRegistro(Registro reg) {
  return strlen(reg.inscricao)
       + strlen(reg.nome)
       + strlen(reg.curso)
       + strlen(reg.score);
}

void setTamanhoRegistro(Registro * reg, int tam) {
    itoa(tam, reg->tamanho, C_BASE_DECIMAL);
}

Registro getRegistro(FILE * arquivo) {
    Registro reg = newRegistro();

    lerCampo(reg.inscricao, '|', arquivo);
    lerCampo(reg.nome, '|', arquivo);
    lerCampo(reg.curso, '|', arquivo);
    lerCampo(reg.score, '|', arquivo);

    setTamanhoRegistro(&reg, getTamanhoRegistro(reg));

    removerPipeRegistro(&reg);

    return reg;
}

void lerCampo(char campo[], char delimitador, FILE * arquivo) {
    int i = 0;

    limparString(campo);

    if(!fimArquivo(arquivo)) {
        do {
            campo[i] = fgetc(arquivo);
            i++;
        } while(campo[i - 1] != delimitador);
    }

    campo[i] = C_CHAR_END;
}

long getLedHead(FILE * arquivo) {
    char sLED[10];

    fseek(arquivo, strlen("LED=*"), SEEK_SET);
    lerCampo(sLED, '.', arquivo);

    return atol(sLED);
}

long buscarPorInscricao(char chave[], FILE * arquivo) {
    char tamanho[C_TAMANHO_CAMPO],
         inscricao[C_TAMANHO_CAMPO];

    bool match = false;
    long byteOffset = -1;

    limparString(tamanho);
    limparString(inscricao);

    // ignora o cabeçario movendo o ponteiro p/ o primeiro registro
    fseek(arquivo, C_TAMANHO_CABECARIO, SEEK_SET);

    while(!match) {
        // lê o primeiro campo que é o tamanho do registro
        lerCampo(tamanho, '|', arquivo);

        // obtem o byte offset do inicio do registro
        byteOffset = ftell(arquivo);

        // lê o segundo campo que é a inscrição do registro
        lerCampo(inscricao, '|', arquivo);
        removerCaractere(inscricao, '|');

        if(stringsIguais(chave, inscricao))
            match = true;
        else {
            if(fimArquivo(arquivo)) {
                byteOffset = -1;
                break;
            }
            else {
                fseek(arquivo, byteOffset, SEEK_SET);
                long lTam = atol(tamanho);
                fseek(arquivo, lTam, SEEK_CUR);
            }
        }
    }
    return byteOffset;
}

bool fimArquivo(FILE * arquivo) {
    char aux = fgetc(arquivo);

    if(aux == EOF)
        return true;
    else {
        fseek(arquivo, -1l, SEEK_CUR);
        return false;
    }
}

void setLedHead(long byteOffset, FILE * arquivo) {
    int i;
    char sLed[C_TAMANHO_CABECARIO],
         sByteOffset[10];


    limparString(sLed);
    limparString(sByteOffset);

    ltoa(byteOffset, sByteOffset, C_BASE_DECIMAL);

    strcat(sLed, "LED=*");
    strcat(sLed, sByteOffset);

    for(i = strlen(sLed); i < C_TAMANHO_CABECARIO; i++)
        sLed[i] = '.';

    sLed[C_TAMANHO_CABECARIO -1] = '|';
    sLed[C_TAMANHO_CABECARIO] = '\0';

    rewind(arquivo);
    fputs(sLed, arquivo);
}

void apontarPraLEDHead(long byteOffset, FILE * arquivo) {
    long lByteOffset = getLedHead(arquivo);
    char sByteOffset[10];
    char sLed[10];

    limparString(sLed);

    ltoa(lByteOffset, sByteOffset, C_BASE_DECIMAL);

    strcat(sLed, "*");
    strcat(sLed, sByteOffset);
    strcat(sLed, "$");

    fseek(arquivo, byteOffset, SEEK_SET);
    fputs(sLed, arquivo);
}

void popularBuffer(char buffer[]) {
    Registro reg = popularRegistro();
    limparString(buffer);
    registroToString(reg, buffer, true);
}

Registro popularRegistro() {
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

    setTamanhoRegistro(&reg, getTamanhoRegistro(reg) + C_QTD_CAMPOS);

    return reg;
}

int getTamanhoCandidato(long pCandidato, FILE * arquivo) {
    if(pCandidato <= 0)
        return -1;
    else {
      int len = 0,
          pipe = 0;

      char flag;

      fseek(arquivo, pCandidato, SEEK_SET);

      while(pipe < 2) {
          len++;
          long passo = pCandidato - len;
          fseek(arquivo, passo, SEEK_SET);

          if(flag = fgetc(arquivo) == '|')
            pipe++;
      }

      char sTam[10];
      limparString(sTam);

      lerCampo(sTam, '|', arquivo);
      return atoi(sTam);
    }
}

long getByteOffsetInsercao(int tamanhoRegistro, long pCandidato, FILE * arquivo) {
    int tCandidato = getTamanhoCandidato(pCandidato, arquivo);
    char sPonteiro[10];

    if(pCandidato != -1) {
      fseek(arquivo, pCandidato, SEEK_SET);
      lerCampo(sPonteiro, '$', arquivo);

      substituiChar(sPonteiro, '*', ' ');
      substituiChar(sPonteiro, '$', '\0');
    }

    if(tCandidato >= tamanhoRegistro) {
        setLedHead(atol(sPonteiro), arquivo);
        return pCandidato;
    }
    else if(tCandidato == -1)
        return getByteOffsetFinal(arquivo);
    else {
        pCandidato = atol(sPonteiro);
        getByteOffsetInsercao(tamanhoRegistro, pCandidato, arquivo);
    }
}

long getByteOffsetFinal(FILE * arquivo) {
    fseek(arquivo, 0l, SEEK_END);
    return ftell(arquivo);
}

void verificarArquivo(FILE * arquivo) {
    if(arquivo == NULL) {
        puts("Ocorreu um problema ao tentar abrir o arquivo. A aplicacao sera finalizada.");
        exit(EXIT_FAILURE);
    }
}

#endif
