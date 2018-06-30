#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <iostream>
#include <string>
#include <windows.h>
#include <fstream>

class Montador
{
    public:
        Montador();
        virtual ~Montador();
        Montador(char fileName, int memory);
        void mounter1Passo(char *fileName, uint16_t *beginning, uint8_t *bytesNumber);
        void mounter2Passos(char *fileName, uint16_t *beginning, uint8_t *bytesNumber, symbol_t* *head);
};
