#ifndef _REGISTRO_H_
#define _REGISTRO_H_

#define C_TAMANHO_CABECARIO 50
// TODO: o cabeçario por hora contem apenas a led, futuramente se
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

// Declarações
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
void popularBuffer(char buffer[]);
Registro popularRegistro();
// LED
long getLedHead(FILE * arquivo);
void setLedHead(long byteOffset, FILE * arquivo);
void atualizarLed(long byteOffset, FILE * arquivo);

// Implementações
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

void registroToString(Registro reg, char str[]) {
    limparString(str);

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

    campo[i] = C_STRING_END;
}

long getLedHead(FILE * arquivo) {
    char sLED[10];

    // posiciona o ponteiro do arquivo na posição em que esta escrito a cabeça da LED
    fseek(arquivo, strlen("LED=*"), SEEK_SET);

    // lê o campo até encontrar um '.'
    lerCampo(sLED, '.', arquivo);

    // retorna a cabeça da LED castando para um long
    return atol(sLED);
}

long buscarPorInscricao(char chave[], FILE * arquivo)
// Esse método busca uma incricao no arquivo e retorna seu byte offset,
// caso nao encontre o retorno é -1
{
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
    // lê o caractere da posição em que o ponteiro estava
    char aux = fgetc(arquivo);

    if(aux == EOF)
        return true;
    else {
        // volta o ponteiro um caractere
        fseek(arquivo, -1l, SEEK_CUR);
        return false;
    }
}

void setLedHead(long byteOffset, FILE * arquivo) {
    char sLed[C_TAMANHO_CABECARIO],
         sByteOffset[10];
    int i;

    limparString(sLed);
    limparString(sByteOffset);

    if(byteOffset > 0)
        byteOffset = byteOffset + 1; // por causa do 1 caractere do pipe

    // converte o byteOffset em string
    ltoa(byteOffset, sByteOffset, C_BASE_DECIMAL);

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
    long lByteOffset = getLedHead(arquivo);
    char sByteOffset[10];
    char sLed[10];

    limparString(sLed);

    // converte o byteOffset que é um long e atribui na string sByteOffset
    ltoa(lByteOffset, sByteOffset, C_BASE_DECIMAL);

    // ex: sLed = "*";
    strcat(sLed, "*");

    // ex: sLed = "*7218";
    strcat(sLed, sByteOffset);

    // ex: sLed = "*7218$";
    strcat(sLed, "$");

    // posiciona o ponteiro no byteOffset no registro que está sendo alterado
    fseek(arquivo, byteOffset, SEEK_SET);

    // escreve no arquivo
    fputs(sLed, arquivo);
}

void popularBuffer(char buffer[]) {
    // obtém o registro populado com os dados inseridos pelo usuário
    Registro reg = popularRegistro();

    // inicializa a string
    limparString(buffer);

    // converte o registro em uma string para poder ser escrita no arquivo
    registroToString(reg, buffer);
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

    setTamanhoRegistro(&reg);

    return reg;
}

#endif
