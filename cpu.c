#include "cpu.h"
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "screen.h"

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


void Cycle() {
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
            if(Y == 0xE0) { // opc: 00E0
                // clc
                Log("clc");
                memset(video, 0, 64*32);
                break;
            }

            if(Y == 0xE0 && N == 0xE0) { //opc 00EE
                Log("subroutine return");
                PC = stack[stackPointer--];
                break;
                // retorno de subrotina
            }

            break;
        }

        case 0x1000: { // opc: 1NNN -> j NNN
            // jmp
            Log("jmp");
            PC = NNN;
            break;
        }

        case 0x2000: { // opc: 2NNN -> jal NNN
            Log("subroutine call");
            stack[stackPointer++] = PC;
            PC = NNN;
            // chamada de subrotina
            break;
        }

        case 0x3000: { // opc: 3XNN -> beq Vx, NN
            Log("branch if equal immediate");
            if(registers[X] == NN) PC += 2;
            // salto condicional
            break;
        }

        case 0x4000: { // opc: 4XNN -> bne Vx, NN
            Log("branch if not equal immediate");
            if(registers[X] != NN) PC += 2;
            // salto condicional
            break;
        }

        case 0x5000: { // opc: 5XY0 -> beq Vx, Vy
            Log("branch if equal");
            if(registers[X] == registers[Y]) PC += 2;
            // salto condicional
            break;
        }

        case 0x6000: { // opc: 6XNN -> li Vx, NN
            // SET
            Log("set");
            registers[X] = NN;
            break;
        }

        case 0x7000: { // opc: 7XNN -> addiu Vx, Vx, NN
            Log("addiu");
            registers[X] += NN;
            // ADD sem overflow
            break;
        }

        case 0x8000: {
            // Agora depende do último nibble da instrução
            switch (N) {
                case 0x0: { // opc 8XY0 -> mov Vx, Vy
                    Log("mov");
                    registers[X] = registers[Y];
                    break;
                }

                case 0x1: { // opc 8XY1 -> OR Vx, Vy, Vx
                    Log("OR");
                    registers[X] |= registers[Y];
                    break;
                }

                case 0x2: { // opc 8XY2 -> AND Vx, Vx, Vy
                    Log("and");
                    registers[X] &= registers[Y];
                    break;
                }

                case 0x3: { // opc 8XY3 -> XOR Vx, Vx, Vy
                    Log("xor");
                    registers[X] ^= registers[Y];
                    break;
                }

                case 0x4: { // opc 8XY4 -> add Vx, Vx, Vy
                    Log("add overflow");
                    if(registers[X] + registers[Y] > 255) registers[0xF] = 1; // overflow
                    else registers[0xF] = 0;
                    registers[X] += registers[Y];
                    break;
                    // Soma com aviso de oferflow
                }

                case 0x5: { // opc 8XY5 -> sub Vx, Vx, Vy
                    Log("sub Vx, Vy");
                    if(registers[X] > registers[Y]) registers[0xF] = 1;
                    else registers[0xF] = 0;
                    registers[X] -= registers[Y];
                    break;
                }

                case 0x7: { // opc 8XY7 -> sub Vx, Vy, Vx
                    Log("sub Vy, Vx");
                    if(registers[Y] > registers[X]) registers[0xF] = 1;
                    else registers[0xF] = 0;
                    registers[X] = registers[Y] - registers[X];
                    break;
                }

                //Instrução ambigua!
                case 0x6: { // opc 8XY6
                    Log("rshift");
//                    registers[X] = registers[Y]; //Ambiguo!!!
                    registers[0xF] = registers[X] & 0x01;
                    registers[X] = registers[X] >> 1;

                    break;
                }

                //Instrução ambigua!
                case 0xE: { // opc 8XYE
                    Log("lsfhit");
//                    registers[X] = registers[Y]; //Ambiguo!
                    registers[0xF] = registers[X] & 0x80;
                    registers[X] = registers[X] << 1;
                    break;
                }

            }
            break;
        }

        case 0x9000: { // opc: 9XY0 -> bne Vx, Vy
            Log("branch if not equal");
            if(registers[X] != registers[Y]) PC += 2;
            // salto condicional
            break;
        }

        case 0xA000: { // opc: ANNN
            Log("set index");
            indexRegister = NNN;
            // set index
            break;
        }

        // Instrução ambigua!!
        case 0xB000: { // opc: BNNN
            Log("jump with offser");
            PC = NNN + registers[0];

            //PC = ((nib1 << 8) | NN) + registers[X];
            // pulo com offset
            break;
        }

        case 0xC000: { // opc: CXNN
            Log("rng");
            registers[X] = rand() & NN;
            // rng
            break;
        }

        case 0xD000: { // opc: DXYN
            // desenho
            Log("draw");
            break;
        }


        case 0xE000: {
            if (NN == 0x9E) { // opc EX9E
                Log("skip if key");
                // salta instrução se tecla pressionada
            }

            if(NN == 0xA1) { // opc EXA1
                Log("skip if not key");
                //salta instrução se tecla NAO pressionada
            }

            break;
        }

        case 0xF000: {
            //Agora depende do segundo byte da instrução
            switch (NN) {
                case 0x07: { // opc FX07
                    Log("Vx = delay timer");
                    registers[X] = delayTimer;
                    break;
                }

                case 0x15: { // opc FX15
                    Log("delay timer = Vx");
                    delayTimer = registers[X];
                    break;
                }

                case 0x18: { // opc FX18
                    Log("sound timer = Vx");
                    soundTimer = registers[X];
                    break;
                }

                case 0x1E: { // opc FX1E
                    Log("I += Vx");
                    indexRegister += registers[X];
                    break;
                }

                case 0x0A: { // opc FX0A
                    Log("get key");
                    break;
                }

                case 0x29: { // opc FX29
                    Log("set I to font");
                    indexRegister = registers[X] * 5;
                    break;
                }

                case 0x33: { // opc FX33
                    Log("binary coded decimal conversion");
                    ram[indexRegister] = (registers[X]/100) % 10;
                    ram[indexRegister+1] = (registers[X]/10) % 10;
                    ram[indexRegister+2] = registers[X] % 10;
                    break;
                }


                // Ambigua!
                case 0x55: { // opc FX55
                    Log("store");
                    for(int i = 0; i <= registers[X]; ++i) {
                        ram[indexRegister + i] = registers[i];
                    }

                    //indexRegister += registers[X] + 1;

                    break;
                }


                // Ambigua!
                case 0x65: { // opc FX65
                    Log("load");
                    for(int i = 0; i <= registers[X]; ++i) {
                        registers[i] = ram[indexRegister + i];
                    }

                    //indexRegister += registers[X] + 1;

                    break;
                }

                default: {
                    printf("Invalid opcode\n!");
                }

            }
            break;
        }

        default: {
            printf("Invalid opcode!\n");
        }

    }
}

void CPULoop() {
    while(1) {
        getchar();
        Cycle();
    }
}