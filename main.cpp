#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <iostream>
#include <string>
#include <windows.h>
#include <fstream>
#include <Montador.h>

void chargeLoader(char *fileName, uint8_t *memory);
void loopInstruction (uint16_t CI, uint8_t *memory, char *fileName, char *userDirectory, uint16_t *bytesNumber);
void pushSymbol(symbol_t* *head, char *name);
int symbolSearch(symbol_t* *head, char *name);
uint16_t retrieveValue(symbol_t* *head, char *reader);

int main()
{
    uint8_t bytesNumber = 0x00;
    uint16_t beginning = 0x0000;
    char chosenCommand[150];
    char fileName[150];
    char userDirectory[150];
    uint8_t memory[1024] = {0x00};
    uint16_t CI = 0x0000;
    deletedOne_t* head = NULL;

    cout << " Projeto da disciplina PCS3216 - 2018" << endl;
    cout << " Docente: Prof. Dr. Joao Jose Neto "<< endl;
    cout << " Aluno: David Almeida Matos 9344998"<< endl;

    /** TELA DE LOGIN **/
    cout << endl <<  " ---- TELA DE LOGIN DE USUARIO ----" << endl << endl;
    cout << " Digite o nome de usuario desejado: ";
    cin >> user;
    cout << " Usuario " + user + " logado com sucesso!\n\n";

    fgets(userDirectory, 150, stdin);
    if(userDirectory[strlen(userDirectory) - 1] == '\n')
        userDirectory[strlen(userDirectory) - 1] = NULL;

    // Inicia carregando o loader na memória
    strcpy(fileName, userDirectory);
    strcat(fileName, "loader.txt");
    chargeLoader(fileName, memory);

    while (1) {

        cout << endl <<  " ---- Lista de comandos disponiveis no projeto ----" << endl << endl;
        cout << " $DIR - Exibe os programas do usuario que estao disponiveis" << endl;
        cout << " $DEL - Remove um programa escolhido da pasta do usuario logado;"<< endl;
        cout << " $RUN - Realiza a execucao de um programa escolhido disponivel;" << endl;
        cout << " $END - Encerra as operacoes e finaliza o sistema."<< endl << endl;
        cout << " Digite o comando desejado: ";

        fgets(chosenCommand, 150, stdin);
        if(chosenCommand[strlen(chosenCommand) - 1] == '\n')
            chosenCommand[strlen(chosenCommand) - 1] = NULL;


        if (!strncmp(chosenCommand, "$DIR", 4)) {
            printf(" ---- LISTA DE ARQUIVOS DISPONIVEIS PARA O USUARIO ----\n");
            struct dirent *de;
            DIR *dr = opendir(userDirectory);
            if (dr == NULL) {
                printf("Erro na busca de programas disponiveis!\n" );
                return 0;
            }
            while ((de = readdir(dr)) != NULL) {
                if ( !strcmp(de->d_name, ".") || !strcmp(de->d_name, "..")
                    || !strcmp(de->d_name, ".DS_Store")) {}
                else printf("%s\n", de->d_name);
            }
            closedir(dr);
        }

        else if (!strncmp(chosenCommand, "$DEL", 4)) {
            strcpy(fileName, userDirectory);
            if(!rename(fileName, userDirectory)) {
                remove(fineName);
                cout << "Arquivo removido com sucesso!\n";
            }
            else printf("\nErro ao deletar arquivo escolhido!\n");
        }

        else if (!strncmp(chosenCommand, "$RUN", 4)) {
            chargeLoader("loader.txt", memory);

            strcpy(fileName, userDirectory);
            strcat(fileName, chosenCommand + 5);
            loopInstruction (CI, memory, fileName, userDirectory, &bytesNumber);
            CI = memory[0x0028]*0x100 + memory[0x0029] - bytesNumber;

            Montador mounter = new Montador(fileName, memory);
            loopInstruction (CI, memory, fileName, userDirectory, &bytesNumber);
        }

        else if (!strncmp(chosenCommand, "$END", 4)) {
            return 0;
        }
    }
    return 0;
}

typedef struct symbol {
   char name[10];
   uint16_t value;
   struct symbol* next;
} symbol_t;

uint16_t retrieveValue(symbol_t* *head, char *reader) {
    symbol_t* current = *head;
    while(strcmp(current->name, reader)) current = current->next;
    return current->value;
}

void pushSymbol(symbol_t* *head, char *name) {
    if(*head == NULL) {
        symbol_t* new = (symbol_t*)malloc(sizeof(symbol_t));
        if(new == NULL){
            printf("Nao foi possivel alocar memory\n");
            exit(-1);
        }
        strcpy(new->name, name);
        new->next = NULL;
        *head = new;
        return;
    }

    if (symbolSearch(head, name)) return;

    symbol_t* new = (symbol_t*)malloc(sizeof(symbol_t));
    if(new == NULL){
        printf("Nao foi possivel alocar memory\n");
        exit(-1);
    }

    strcpy(new->name, name);
    new->next = NULL;
    symbol_t* current = *head;
    while (current->next != NULL) current = current->next;
    current->next = new;
}

int symbolSearch(symbol_t* *head, char *name) {
    symbol_t* current = *head;
    while (current != NULL) {
        if(!strcmp(current->name, name)) return 1;
        current = current->next;
    }
    return 0;
}

void loopInstruction (uint16_t CI, uint8_t *memory, char *fileName, char *userDirectory, uint16_t *bytesNumber) {
    FILE *program;
    FILE *exit;
    int8_t operation;
    int16_t operand;
    int8_t accumulator = 0x00;
    int aux;
    char auxVar[10];
    int HALT = 0;
    int indirectMode = 0;
    char arquivo_exit[150];

    program = fopen(fileName, "r");
    if (program == NULL) {
        printf("Erro na abertura do arquivo\n");
        return;
    }

    strcpy(arquivo_exit, userDirectory);
    strcat(arquivo_exit, "exit.txt");
    exit = fopen(arquivo_exit, "w");
    if (exit == NULL) {
        printf("Erro na abertura do arquivo\n");
        return;
    }

    aux = fscanf(program, "%02X", bytesNumber);
    aux = fscanf(program, "%02X", bytesNumber);
    aux = fscanf(program, "%02X", bytesNumber);
    if (aux != 1) printf("Erro ao realizara leitura do arquivo proposto\n");
    rewind(program);

    while(!HALT) {

        operation = (memory[CI] / 0x10); // Realiza a identificação do tipo de instrução solicitada
        if (operation == 0x3 || operation == 0xB || operation == 0xC)
            operand = (memory[CI] % 0x10);
        else operand = (memory[CI] % 0x10)*0x100 + memory[CI + 0x1];

        switch (operation) {

        case 0x0: // jump incondicional
            if(!indirectMode) CI = operand;
            else CI = memory[operand]*0x100 + memory[operand + 0x1];
            indirectMode = 0; // Retornará ao endereçamento direto em todos os casos
            break;

        case 0x1: // jump se accumulator for 0
            if (accumulator == 0 && !indirectMode) CI = operand;
            else if(accumulator == 0) CI = memory[operand]*0x100 + memory[operand + 0x1];
            else CI += 2;
            indirectMode = 0; // Retornará ao endereçamento direto em todos os casos
            break;

        case 0x2: // jump se accumulator for negativo
            if (accumulator < 0 && !indirectMode) CI = operand;
            else if(accumulator < 0) CI = memory[operand]*0x100 + memory[operand + 0x1];
            else CI += 2;
            indirectMode = 0; // Retornará ao endereçamento direto em todos os casos
            break;

        case 0x3: // instrucao de controle
            switch (operand) {

            case 0x0: // halt machine
                printf("Halt Machine\n\n");
                HALT = 1;
                break;

            case 0x2: // indirect
                indirectMode = 1;
                CI += 1;
                break;
            }
            break;

        case 0x4: // soma
            if(!indirectMode) accumulator += memory[operand];
            else accumulator += memory[memory[operand]*0x100 + memory[operand + 0x1]];
            CI += 2;
            indirectMode = 0; // Retornará ao endereçamento direto em todos os casos
            break;

        case 0x5: // subtracao
            if(!indirectMode) accumulator -= memory[operand];
            else accumulator -= memory[memory[operand]*0x100 + memory[operand + 0x1]];
            CI += 2;
            indirectMode = 0; // Retornará ao endereçamento direto em todos os casos
            break;

        case 0x6: // multiplicacao
            if(!indirectMode) accumulator *= memory[operand];
            else accumulator *= memory[memory[operand]*0x100 + memory[operand + 0x1]];
            CI += 2;
            indirectMode = 0; // Retornará ao endereçamento direto em todos os casos
            break;

        case 0x7: // divisao
            if(!indirectMode) accumulator /= memory[operand];
            else accumulator /= memory[memory[operand]*0x100 + memory[operand + 0x1]];
            CI += 2;
            indirectMode = 0; // Retornará ao endereçamento direto em todos os casos
            break;

        case 0x8: // load
            if(!indirectMode) accumulator = memory[operand];
            else accumulator = memory[memory[operand]*0x100 + memory[operand + 0x1]];
            CI += 2;
            indirectMode = 0; // Retornará ao endereçamento direto em todos os casos
            break;

        case 0x9: // store
            if(!indirectMode) memory[operand] = accumulator;
            else memory[memory[operand]*0x100 + memory[operand + 0x1]] = accumulator;
            CI += 2;
            indirectMode = 0; // Retornará ao endereçamento direto em todos os casos
            break;

        case 0xA: // chamada de sub rotina
            if(!indirectMode) {
                memory[operand] = (CI + 0x2) / 0x100;
                memory[operand + 0x1] = (CI + 0x2) % 0x100;
                CI = operand + 0x2;
            }
            else {
                memory[memory[operand]*0x100 + memory[operand + 0x1]] = (CI + 0x2) / 0x100;
                memory[(memory[operand]*0x100 + memory[operand + 0x1]) + 0x1] = (CI + 0x2) % 0x100;
                CI = (memory[operand]*0x100 + memory[operand + 0x1]) + 0x2;
            }
            indirectMode = 0; // Retornará ao endereçamento direto em todos os casos
            break;

        case 0xB: // chamada do sistema operacional
            HALT = 1;
            break;

        case 0xC: // entrada, exit e interrupcao
            switch (operand) {

            case 0x0:
                fgets(auxVar, sizeof(auxVar)-1, program);
                sscanf(auxVar, "%02X", &accumulator);
                CI += 1;
                break;

            case 0x4:
                fprintf(exit, "%02X\n", accumulator);
                CI += 1;
                break;
            }
            break;
        }
    }
    fclose(program);
    fclose(exit);
}

void chargeLoader(char *fileName, uint8_t *memory) {
    FILE *program;
    program = fopen(fileName, "r");
    if (program == NULL) {
        printf("Erro na abertura do arquivo\n");
        return;
    }
    // Lê arquivo e o coloca na memória
    for (int i = 0; i < 44; i++) fscanf(program, "%02X", &memory[0x0 + i]);
    fclose(program) ;
}
