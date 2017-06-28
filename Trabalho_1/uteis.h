#ifndef _UTEIS_H_
#define _UTEIS_H_

#define C_BASE_DECIMAL 10
#define C_EMPTY_STRING ""
#define C_STRING_END   '\0'

// Declaração
void limparBuffer();
bool stringsIguais(char a[], char b[]);
void removerCaractere(char str[], char caractere);
void substituiChar(char str[], char oldChar, char newChar);
void limparString(char str[]);

// Implementação
void limparBuffer() {
    fflush(stdin);
}

bool stringsIguais(char a[], char b[]) {
  return strcmp(a, b) == 0;
}

void removerCaractere(char str[], char caractere) {
    int i;

    for(i = 0; i < strlen(str); i++) {
        if(str[i] == caractere)
            str[i] = C_STRING_END;
    }
}

void substituiChar(char str[], char oldChar, char newChar) {
    int i;

    for(i = 0; i < strlen(str); i++) {
        if(str[i] == oldChar)
            str[i] = newChar;
    }
}

void limparString(char str[]) {
    str[0] = C_STRING_END;
}

#endif
