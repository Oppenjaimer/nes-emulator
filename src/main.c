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
    bus.cpu.a = 0x05;

    uint8_t test_rom[] = {
        OP_ASL_IMP,
        OP_BCC_REL, 0x02,

        OP_ASL_IMP,
        OP_ASL_IMP,

        OP_ADC_IMM, 0xF6,
        OP_BCC_REL, 0x02,
        OP_AND_IMM, 0xAA,
    };

    for (size_t i = 0; i < sizeof(test_rom); i++) {
        bus_write(&bus, start_addr + i, test_rom[i]);
    }

    printf("--- Initial State ---\n");
    print_cpu_state(&bus.cpu);
    printf("\n--- Starting Execution ---\n");

    for (int i = 0; i < 15; i++) {
        cpu_clock(&bus.cpu);
        if (bus.cpu.cycles == 0) print_cpu_state(&bus.cpu);
    }

    return 0;
}
