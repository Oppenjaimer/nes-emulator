#include <stdio.h>

#include "bus.h"
#include "cpu.h"

void print_cpu_state(const CPU *cpu) {
    printf("PC: %04X | A: %02X X: %02X Y: %02X | SP: %02X | Status: %02X | Cycles: %d\n",
           cpu->pc, cpu->a, cpu->x, cpu->y, cpu->sp, cpu->status, cpu->cycles);
}

int main() {
    Bus bus = {0};

    bus_init(&bus);
    cpu_init_table(&bus.cpu);

    uint16_t start_addr = 0x8000;
    bus.cpu.pc = start_addr;

    bus_write(&bus, start_addr + 0, NOP_IMP);
    bus_write(&bus, start_addr + 1, NOP_IMP);
    bus_write(&bus, start_addr + 2, BRK_IMP);

    printf("--- Initial State ---\n");
    print_cpu_state(&bus.cpu);
    printf("\n--- Starting Execution ---\n");

    for (int i = 1; i <= 12; i++) {
        cpu_clock(&bus.cpu);
        printf("(Clock %02d) ", i);
        print_cpu_state(&bus.cpu);
    }

    return 0;
}
