#include <stdio.h>
#include <time.h>
#include <stdint.h>
#include "cpu.h"




int main() {
    InitMemory();
    LoadRom("testRom.ch8");
    CPULoop();
}
