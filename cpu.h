#ifndef CHIP8_CPU_H
#define CHIP8_CPU_H

void LoadRom(char *filePath);
void InitMemory();
void DumpRam();
void Cycle();
void CPULoop();

#endif //CHIP8_CPU_H
