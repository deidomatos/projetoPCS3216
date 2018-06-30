#include "Montador.h"

Montador::Montador()
{
}

Montador::~Montador()
{
}

Montador::Montador(char fileName, int memory)
{
    fileName = this.fileName;
    memory = this.memory;
}

/* PRIMEIRO PASSO DO MONTADOR */
void mounter1Passo(char *fileName, uint16_t *beginning, uint8_t *bytesNumber) {
    FILE *program;
    char reader[10];
    int aux = 0;
    uint16_t position;
    symbol_t* head = NULL;

    /* Abertura do programa de leitura  */
    program = fopen(fileName, "r");
    if (program == NULL) {
        printf("Erro na abertura do arquivo\n");
        return;
    }
    /* Loop para incrementar a tabela de símbolos */
    while (1) {
        aux = fscanf(program, "%s", reader);
        if (aux != 1) printf("Erro ao realizar a leitura do arquivo proposto\n");
        if(!strcmp(reader, "#")) break;
        if(!strcmp(reader, "@")) {
            aux = fscanf(program, "%04X", beginning);
            if (aux != 1) printf("Erro ao realizar a leitura do arquivo proposto\n");
            position = *beginning;
        } else if(!strcmp(reader, "LD")) { // Carrega o valor na memória do sistema
            aux = fscanf(program, "%s", reader);
            if (aux != 1) printf("Erro ao realizar a leitura do arquivo proposto\n");
            pushSymbol(&head, reader); // Coloca o símbolo referente na lista de símbolos
            position += 0x2;
        } else if(!strcmp(reader, "MM")) { // Guarda o valor na memória do sistema
            aux = fscanf(program, "%s", reader);
            if (aux != 1) printf("Erro ao realizar a leitura do arquivo proposto\n");
            pushSymbol(&head, reader); // Coloca o símbolo referente na lista de símbolos
            position += 0x2;
        } else if(!strcmp(reader, "JP")) { // Realiza o Jump Incondicional
            aux = fscanf(program, "%s", reader);
            if (aux != 1) printf("Erro ao realizar a leitura do arquivo proposto\n");
            pushSymbol(&head, reader); // Coloca o símbolo referente na lista de símbolos
            position += 0x2;
        } else if(!strcmp(reader, "JZ")) { // jump if zero
            aux = fscanf(program, "%s", reader);
            if (aux != 1) printf("Erro ao realizar a leitura do arquivo proposto\n");
            pushSymbol(&head, reader); // Coloca o símbolo referente na lista de símbolos
            position += 0x2;
        } else if(!strcmp(reader, "JN")) { // jump if negative
            aux = fscanf(program, "%s", reader);
            if (aux != 1) printf("Erro ao realizar a leitura do arquivo proposto\n");
            pushSymbol(&head, reader); // Coloca o símbolo referente na lista de símbolos
            position += 0x2;
        } else if(!strcmp(reader, "CN")) {
            aux = fscanf(program, "%s", reader);
            if (aux != 1) printf("Erro ao realizar a leitura do arquivo proposto\n");
            position += 0x1;
        } else if(!strcmp(reader, "+")) { // soma
            aux = fscanf(program, "%s", reader);
            if (aux != 1) printf("Erro ao realizar a leitura do arquivo proposto\n");
            pushSymbol(&head, reader); // Coloca o símbolo referente na lista de símbolos
            position += 0x2;
        } else if(!strcmp(reader, "-")) { // subtracao
            aux = fscanf(program, "%s", reader);
            if (aux != 1) printf("Erro ao realizar a leitura do arquivo proposto\n");
            pushSymbol(&head, reader); // Coloca o símbolo referente na lista de símbolos
            position += 0x2;
        } else if(!strcmp(reader, "*")) { // multiplicacao
            aux = fscanf(program, "%s", reader);
            if (aux != 1) printf("Erro ao realizar a leitura do arquivo proposto\n");
            pushSymbol(&head, reader); // Coloca o símbolo referente na lista de símbolos
            position += 0x2;
        } else if(!strcmp(reader, "/")) { // divisao
            aux = fscanf(program, "%s", reader);
            if (aux != 1) printf("Erro ao realizar a leitura do arquivo proposto\n");
            pushSymbol(&head, reader); // Coloca o símbolo referente na lista de símbolos
            position += 0x2;
        } else if(!strcmp(reader, "SC")) { // chamda de subrotina
            aux = fscanf(program, "%s", reader);
            if (aux != 1) printf("Erro ao realizar a leitura do arquivo proposto\n");
            pushSymbol(&head, reader); // Coloca o símbolo referente na lista de símbolos
            position += 0x2;
        } else if(!strcmp(reader, "OS")) { // chamada de sistema operacional
            // nao faz nada no primeiro passo, apenas incrementa position
            position += 0x1;
        } else if(!strcmp(reader, "IO")) { // input/output/interrupcao
            // nao faz nada no primeiro passo, apenas avanca na leitura e incrementa position
            aux = fscanf(program, "%s", reader);
            if (aux != 1) printf("Erro ao realizar a leitura do arquivo proposto\n");
            position += 0x1;
        } else if(!strcmp(reader, "K")) { // constante
            // nao faz nada no primeiro passo, apenas avanca na leitura e incrementa position
            aux = fscanf(program, "%s", reader);
            if (aux != 1) printf("Erro ao realizar a leitura do arquivo proposto\n");
            position += 0x1;
        } else { // symbols ou labels
            pushSymbol(&head, reader);
            // associa endereco ao symbol correto
            symbol_t* current = head;
            while(strcmp(current->name, reader)) current = current->next;
            current->value = position;
        }
    }
    *bytesNumber = position - *beginning;
    fclose(program);
    mounter2Passos(fileName, beginning, bytesNumber, &head);
}

/* SEGUNDO PASSO DO MONTADOR */
void mounter2Passos(char *fileName, uint16_t *beginning, uint8_t *bytesNumber, symbol_t* *head) {
    FILE *program;
    FILE *symbolProgram;
    char reader[10];
    char newName[150];
    int aux = 0;

    symbolProgram = fopen(fileName, "r");
    if (symbolProgram == NULL) {
        printf("Erro ao realizar a abertura do arquivo ecolhido\n");
        return;
    }

    strncpy(newName, fileName, strlen(fileName));
    program = fopen(newName, "w");
    fprintf(program, "%02X\n", (*beginning / 0x100));
    fprintf(program, "%02X\n", (*beginning % 0x100));
    fprintf(program, "%02X\n", *bytesNumber);
    aux = fscanf(symbolProgram, "%s", reader);
    if (aux != 1) printf("Erro ao realizar a leitura do arquivo proposto\n");

    while (1) {
        aux = fscanf(symbolProgram, "%s", reader);
        if (aux != 1) printf("Erro ao realizar a leitura do arquivo proposto\n");
        if(!strcmp(reader, "#")) break;
        if(!strcmp(reader, "MM")) { // jump incondicional
            // Realiza a leitura do próximo símbolo
            aux = fscanf(symbolProgram, "%s", reader);
            if (aux != 1) printf("Erro ao realizar a leitura do arquivo proposto\n");
            // Realiza a impressão dos 2 bytes da operação escolhida
            fprintf(program, "%02X\n", 0x00 + (retrieveValue(head, reader) / 0x100));
            fprintf(program, "%02X\n", (retrieveValue(head, reader) % 0x100));
         } else if(!strcmp(reader, "LD")) { // carrega value da memory
            // Realiza a leitura do próximo símbolo
            aux = fscanf(symbolProgram, "%s", reader);
            if (aux != 1) printf("Erro ao realizar a leitura do arquivo proposto\n");
            // Realiza a impressão dos 2 bytes da operação escolhida
            fprintf(program, "%02X\n", 0x80 + (retrieveValue(head, reader) / 0x100));
            fprintf(program, "%02X\n", (retrieveValue(head, reader) % 0x100));
        } else if(!strcmp(reader, "JP")) { // guarda value na memory
            // Realiza a leitura do próximo símbolo
            aux = fscanf(symbolProgram, "%s", reader);
            if (aux != 1) printf("Erro ao realizar a leitura do arquivo proposto\n");
            // Realiza a impressão dos 2 bytes da operação escolhida
            fprintf(program, "%02X\n", 0x90 + (retrieveValue(head, reader) / 0x100));
            fprintf(program, "%02X\n", (retrieveValue(head, reader) % 0x100));
        } else if(!strcmp(reader, "JZ")) { // jump if zero
            // Realiza a leitura do próximo símbolo
            aux = fscanf(symbolProgram, "%s", reader);
            if (aux != 1) printf("Erro ao realizar a leitura do arquivo proposto\n");
            // Realiza a impressão dos 2 bytes da operação escolhida
            fprintf(program, "%02X\n", 0x10 + (retrieveValue(head, reader) / 0x100));
            fprintf(program, "%02X\n", (retrieveValue(head, reader) % 0x100));
        } else if(!strcmp(reader, "JN")) { // jump if negative
            // Realiza a leitura do próximo símbolo
            aux = fscanf(symbolProgram, "%s", reader);
            if (aux != 1) printf("Erro ao realizar a leitura do arquivo proposto\n");
            // Realiza a impressão dos 2 bytes da operação escolhida
            fprintf(program, "%02X\n", 0x20 + (retrieveValue(head, reader) / 0x100));
            fprintf(program, "%02X\n", (retrieveValue(head, reader) % 0x100));
        } else if(!strcmp(reader, "CN")) { // instrucao de controle
            // Realiza a leitura do próximo símbolo
            aux = fscanf(symbolProgram, "%s", reader);
            if (aux != 1) printf("Erro ao realizar a leitura do arquivo proposto\n");
            // imprime o byte da operation
            if(!strcmp(reader, "HM")) fprintf(program, "%02X\n", 0x30);
            else if(!strcmp(reader, "IN")) fprintf(program, "%02X\n", 0x32);
        } else if(!strcmp(reader, "+")) { // soma
            // Realiza a leitura do próximo símbolo
            aux = fscanf(symbolProgram, "%s", reader);
            if (aux != 1) printf("Erro ao realizar a leitura do arquivo proposto\n");
            // Realiza a impressão dos 2 bytes da operação escolhida
            fprintf(program, "%02X\n", 0x40 + (retrieveValue(head, reader) / 0x100));
            fprintf(program, "%02X\n", (retrieveValue(head, reader) % 0x100));
        } else if(!strcmp(reader, "-")) { // subtracao
            // Realiza a leitura do próximo símbolo
            aux = fscanf(symbolProgram, "%s", reader);
            if (aux != 1) printf("Erro ao realizar a leitura do arquivo proposto\n");
            // Realiza a impressão dos 2 bytes da operação escolhida
            fprintf(program, "%02X\n", 0x50 + (retrieveValue(head, reader) / 0x100));
            fprintf(program, "%02X\n", (retrieveValue(head, reader) % 0x100));
        } else if(!strcmp(reader, "*")) { // multiplicacao
            // Realiza a leitura do próximo símbolo
            aux = fscanf(symbolProgram, "%s", reader);
            if (aux != 1) printf("Erro ao realizar a leitura do arquivo proposto\n");
            // Realiza a impressão dos 2 bytes da operação escolhida
            fprintf(program, "%02X\n", 0x60 + (retrieveValue(head, reader) / 0x100));
            fprintf(program, "%02X\n", (retrieveValue(head, reader) % 0x100));
        } else if(!strcmp(reader, "/")) { // divisao
            // Realiza a leitura do próximo símbolo
            aux = fscanf(symbolProgram, "%s", reader);
            if (aux != 1) printf("Erro ao realizar a leitura do arquivo proposto\n");
            // Realiza a impressão dos 2 bytes da operação escolhida
            fprintf(program, "%02X\n", 0x70 + (retrieveValue(head, reader) / 0x100));
            fprintf(program, "%02X\n", (retrieveValue(head, reader) % 0x100));
        } else if(!strcmp(reader, "SC")) { // chamada de subrotina
            // Realiza a leitura do próximo símbolo
            aux = fscanf(symbolProgram, "%s", reader);
            if (aux != 1) printf("Erro ao realizar a leitura do arquivo proposto\n");
            // Realiza a impressão dos 2 bytes da operação escolhida
            fprintf(program, "%02X\n", 0xA0 + (retrieveValue(head, reader) / 0x100));
            fprintf(program, "%02X\n", (retrieveValue(head, reader) % 0x100));
        } else if(!strcmp(reader, "OS")) { // chamada de sistema operacional
            // imprime o byte da operation
            fprintf(program, "%02X\n", 0xB0);
        } else if(!strcmp(reader, "IO")) { // input/output
            // Realiza a leitura do próximo símbolo
            aux = fscanf(symbolProgram, "%s", reader);
            if (aux != 1) printf("Erro ao realizar a leitura do arquivo proposto\n");
            // imprime o byte da operation
            if(!strcmp(reader, "GD")) fprintf(program, "%02X\n", 0xC0);
            else if(!strcmp(reader, "PD")) fprintf(program, "%02X\n", 0xC4);
        } else if(!strcmp(reader, "K")) { // constante
            // Realiza a leitura do próximo símbolo
            aux = fscanf(symbolProgram, "%s", reader);
            if (aux != 1) printf("Erro ao realizar a leitura do arquivo proposto\n");
            fprintf(program, "%s\n", reader);
        }
    }
    symbol_t* current = *head;
    symbol_t* next;
    while (current != NULL) {
        next = current->next;
        free(current);
        current = next;
    }
    *head = NULL;
    fclose(program);
    fclose(symbolProgram);
}
