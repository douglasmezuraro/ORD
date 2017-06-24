#ifndef _REGISTRO_H_
#define _REGISTRO_H_

#define C_TAMANHO_CABECARIO 50
// TODO: o cabe�ario por hora contem apenas a led, futuramente se
// der tempo implementar mais coisas, ex, qte de registros
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

// Declara��es
void printRegistro(Registro reg);
bool assigned(Registro reg);
Registro newRegistro();
Registro stringToRegistro(char str[]);
void removerPipeRegistro(Registro * reg);
void setTamanhoRegistro(Registro * reg);
Registro getRegistro(FILE * arquivo);
void lerCampo(char campo[], char delimitador, FILE * arquivo);
long buscarPorInscricao(char chave[], FILE * arquivo);
bool fimArquivo(FILE * arquivo);
// LED
long getLedByteOffset(FILE * arquivo);
void setLedByteOffset(long byteOffset, FILE * arquivo);
void atualizarLed(long byteOffset, FILE * arquivo);

// Implementa��es
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

    limparString(campo);

    if(!fimArquivo(arquivo)) {
        do {
            campo[i] = fgetc(arquivo);
            i++;
        } while(campo[i - 1] != delimitador);
    }

    campo[i] = '\0'; // finaliza a string para evitar lixo
}

long getLedByteOffset(FILE * arquivo) {
    char sLED[10];

    fseek(arquivo, strlen("LED=*"), SEEK_SET);
    lerCampo(sLED, '.', arquivo);

    return atol(sLED);
}

long buscarPorInscricao(char chave[], FILE * arquivo)
// Esse m�todo busca uma incricao no arquivo e retorna seu byte offset,
// caso nao encontre o retorno � -1
{
    char tamanho[C_TAMANHO_CAMPO],
         inscricao[C_TAMANHO_CAMPO];
    bool match = false;
    long byteOffset = -1;

    // inicializa as strings
    limparString(tamanho); limparString(inscricao);

    // ignora o cabe�ario movendo o ponteiro p/ o primeiro registro
    fseek(arquivo, C_TAMANHO_CABECARIO, SEEK_SET);

    // enquanto n�o encontrou fa�a:
    while(!match) {
      // l� o primeiro campo que � o tamanho do registro
      lerCampo(tamanho, '|', arquivo);
      // obtem o byte offset do inicio do registro sem c seu tamanho
      byteOffset = ftell(arquivo);
      // l� o segundo campo que � a inscri��o do registro
      lerCampo(inscricao, '|', arquivo);
      // retira a pipe da inscri��o lida para poder comparar com a incri��o passada por par�metro
      removerCaractere(inscricao, '|');
      // movimenta o ponteiro para o offset obtido logo acima
      fseek(arquivo, byteOffset, SEEK_SET);

      if(stringsIguais(chave, inscricao))
        // se as for igual o registro foi encontrado
        match = true;
      if(fimArquivo(arquivo)) {
        byteOffset = -1;
        break;
      }
      else {
        // sen�o pula para o proximo registro
        int iTam = atoi(tamanho);
        fseek(arquivo, iTam, SEEK_CUR);
      }
    }
    return byteOffset;
}

bool fimArquivo(FILE * arquivo) {
    // l� o caractere da posi��o em que o ponteiro estava
    char aux = fgetc(arquivo);

    if(aux == EOF)
        return true;
    else {
        // volta o ponteiro um caractere
        fseek(arquivo, -1l, SEEK_CUR);
        return false;
    }
}

void setLedByteOffset(long byteOffset, FILE * arquivo) {
    char sLed[C_TAMANHO_CABECARIO]; limparString(sLed);
    char sByteOffset[10]; limparString(sByteOffset);
    int i;

    if(byteOffset > 0)
        byteOffset = byteOffset + 1; // por causa do 1 caractere do pipe
    // converte o byteOffset em string
    ltoa(byteOffset, sByteOffset, 10);
    // concatena com a string sLed, ex: "LED="
    strcat(sLed, "LED=*");
    // concatena com a string sLed, ex: "LED=1782"
    strcat(sLed, sByteOffset);

    // preenche a string sLed com pontos, ex: "LED=1782.........."
    for(i = strlen(sLed); i < C_TAMANHO_CABECARIO; i++)
        sLed[i] = '.';

   sLed[C_TAMANHO_CABECARIO] = '\0';

    // volta pro inicio do arquivo
    rewind(arquivo);
    // escreve a string sLed no arquivo
    fputs(sLed, arquivo);
}

void atualizarLed(long byteOffset, FILE * arquivo) {
    long lByteOffset = getLedByteOffset(arquivo);
    char sByteOffset[10];

    char sLed[10];
    limparString(sLed);

    // converte o byteOffset que � um long e atribui na string sByteOffset
    ltoa(lByteOffset, sByteOffset, 10);

    // ex: sLed = "(*";
    strcat(sLed, "(*");
    // ex: sLed = "(*7218";
    strcat(sLed, sByteOffset);
    // ex: sLed = "(*7218)";
    strcat(sLed, ")");

    // posiciona o ponteiro no byteOffset no registro que est� sendo alterado
    fseek(arquivo, byteOffset, SEEK_SET);

    // escreve no arquivo
    fputs(sLed, arquivo);
}

#endif
