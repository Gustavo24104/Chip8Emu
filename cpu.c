#include "cpu.h"
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#define null NULL
#define PC_START 0x200
#define debbuging 1


unsigned char ram[4096];
bool video[64*32]; // 1: Branco; 0: Preto
uint16_t PC;
uint16_t indexRegister;
uint16_t stack[32];
uint16_t stackPointer;
uint8_t delayTimer;
uint8_t soundTimer;
uint8_t registers[16];
bool keys[16];


const static uint8_t fonte[80] = {
        0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
        0x20, 0x60, 0x20, 0x20, 0x70, // 1
        0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
        0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
        0x90, 0x90, 0xF0, 0x10, 0x10, // 4
        0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
        0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
        0xF0, 0x10, 0x20, 0x40, 0x40, // 7
        0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
        0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
        0xF0, 0x90, 0xF0, 0x90, 0x90, // A
        0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
        0xF0, 0x80, 0x80, 0x80, 0xF0, // C
        0xE0, 0x90, 0x90, 0x90, 0xE0, // D
        0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
        0xF0, 0x80, 0xF0, 0x80, 0x80  // F
};



void InitMemory() {
    memset(ram, 0, 4096);
    memset(video, 0, 64*32);
    PC = PC_START; //começa fora da fonte
    indexRegister = 0;
    memset(stack, 0, 32);
    stackPointer = 0;
    delayTimer = 0;
    soundTimer = 0;
    memset(registers, 0, 16);
    memset(keys, 0, 16);

    for(int i = 0; i < 80; ++i) {
        ram[i] = fonte[i];
    }
}

void LoadRom(char *filePath) {
    int romSize;
    FILE *rom = fopen(filePath, "rb");

    if(rom == null) {
        printf("ROM nao encontrada!\n");
        exit(-1);
    }

    fseek(rom, 0, SEEK_END);
    romSize = ftell(rom);
    rewind(rom);
    uint8_t buff[romSize];
    fread(buff, sizeof(uint8_t), romSize, rom);

    for(int i = 0; i < romSize; ++i) {
        ram[i + PC_START] = buff[i];
    }
}


void DumpRam() {
    for(int i = 0; i < 4096; ++i) {
        if(i % 16 == 0) printf("\n");
        printf("%02X ", ram[i]);
    }
}

void Log(char *msg) {
    if(debbuging) {
        printf("%s\n", msg);
    }
}


void CPULoop() {
    // Fetch
    //uint16_t i1, i2;
//    PC += 2;

//    i1 = ram[PC];
//    i2 = ram[PC+1];

    // Cada instrução eh composta de 2 bytes, cada posição da memória tem 1 byte, ent eh necessário ler
    // duas consecutivas para pegar a instrução
    uint16_t inst = (ram[PC] << 8) | ram[PC+1]; // Junta as duas movendo a primeira parte pra fente e fazendo um OR
    PC += 2;
    uint16_t nib1 = 0xF000 & inst; // extrai o primeiro nibble -> identifica instrução
    uint16_t X = 0x0F00 & inst; // segundo nibble -> indica algum registrador Vx
    uint8_t Y = 0x00F0 & inst; // terceiro nibble -> indica algum registrador Vy
    uint8_t N = 0x000F & inst; // quarto nibble -> numero de 4 bits
    uint8_t NN = 0x00FF & inst; // 2° byte -> numero imediato de 8 bits
    uint16_t NNN = 0x0FFF & inst; // endereço de memória imediato de 12 bits


//    printf("Ist separada: %02X%02X\n", i1, i2);
    printf("Ist: %04X\n", inst);
    printf("nib1: %X\n", nib1);
    printf("nib2: %X\n", X);
    printf("nib3: %X\n", Y);
    printf("nib4: %X\n", N);

    // Decode

    switch (nib1) {
        case 0x00: {
            if(Y == 0xE) { // opc: 00E0
                // clc
            }

            if(Y == 0xE && N == 0xE) { //opc 00EE
                // subrotina
            }

            break;
        }

        case 0x1: { // opc: 1NNN
            // jmp
            break;
        }

        case 0x2: { // opc: 2NNN
            // retorna de subrotina
            break;
        }

        case 0x3: { // opc: 3XNN
            // salto condicional
            break;
        }

        case 0x4: { // opc: 4XNN
            // salto condicional
            break;
        }

        case 0x5: { // opc: 5XY0
            // salto condicional
            break;
        }

        case 0x6: { // opc: 6XNN
            // SET
            break;
        }

        case 0x7: { // opc: 7XNN
            // ADD
            break;
        }

        case 0x8: {
            // Agora depende do último nibble da instrução
            switch (N) {
                case 0x0: { // opc 8XY0
                    break;
                }

                case 0x1: { // opc 8XY1
                    break;
                }

                case 0x2: { // opc 8XY2
                    break;
                }

                case 0x3: { // opc 8XY3
                    break;
                }

                case 0x4: { // opc 8XY4
                    break;
                }

                case 0x5: { // opc 8XY5
                    break;
                }

                case 0x7: { // opc 8XY7
                    break;
                }

                case 0x6: { // opc 8XY6
                    break;
                }

                case 0xE: { // opc 8XYE
                    break;
                }

            }
            break;
        }

        case 0x9: { // opc: 9XY0
            // salto condicional
            break;
        }

        case 0xA: { // opc: ANNN
            // set index
            break;
        }

        case 0xB: { // opc: BNNN
            // pulo com offset
            break;
        }

        case 0xC: { // opc: CXNN
            // rng
            break;
        }

        case 0xD: { // opc: DXYN
            // desenho
            break;
        }


        case 0xE: {
            if (NN == 0x9E) { // opc EX9E
                // salta instrução se tecla pressionada
            }

            if(NN == 0xA1) { // opc EXA1
                //salta instrução se tecla NAO pressionada
            }

            break;
        }

        case 0xF: {
            //Agora depende do segundo byte da instrução
            switch (NN) {
                case 0x07: { // opc FX07

                }

                case 0x15: { // opc FX15

                    break;
                }

                case 0x18: { // opc FX18

                    break;
                }

                case 0x1E: { // opc FX1E
                    break;
                }

                case 0x0A: { // opc FX0A
                    break;
                }

                case 0x29: { // opc FX29
                    break;
                }

                case 0x33: { // opc FX33
                    break;
                }

                case 0x55: { // opc FX55
                    break;
                }

                case 0x65: { // opc FX65
                    break;
                }

            }
            break;
        }

        default: {
            printf("opcode invalido!\n");
        }

    }

}