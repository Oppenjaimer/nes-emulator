#include <assert.h>
#include <stdio.h>

#include "bus.h"
#include "cpu.h"

void print_cpu_state(const CPU *cpu) {
    printf("PC: %04X | A: %02X X: %02X Y: %02X | SP: %02X | Status: %02X | Cycles: %d\n",
           cpu->pc, cpu->a, cpu->x, cpu->y, cpu->sp, cpu->status, cpu->cycles);
}

int main() {
    // Initialize bus
    Bus bus = {0};
    bus_init(&bus);

    // Write starting address to reset vector
    uint16_t start_addr = 0x8000;
    bus_write(&bus, RESET_VECTOR + 0, start_addr & 0x00FF);
    bus_write(&bus, RESET_VECTOR + 1, start_addr >> 8);

    // Initialize CPU
    cpu_init_table(&bus.cpu);
    cpu_reset(&bus.cpu);

    // Set test initial values
    uint8_t initial_sp = bus.cpu.sp;
    bus.cpu.a = 0x00;

    uint8_t test_rom[] = {
        OP_JSR_ABS, 0x10, 0x80,
        OP_NOP_IMP
    };

    uint8_t subroutine_rom[] = {
        OP_LDA_IMM, 0x42,
        OP_RTS_IMP
    };

    // Load programs into memory
    for (size_t i = 0; i < sizeof(test_rom); i++) {
        bus_write(&bus, start_addr + i, test_rom[i]);
    }
    for (size_t i = 0; i < sizeof(subroutine_rom); i++) {
        bus_write(&bus, 0x8010 + i, subroutine_rom[i]);
    }

    // Run CPU cycles
    int cycles = 16;
    for (int i = 0; i < RESET_CYCLES + cycles; i++) {
        cpu_clock(&bus.cpu);
        if (bus.cpu.cycles == 0) print_cpu_state(&bus.cpu);
    }

    // Check test results
    assert(bus.cpu.pc == 0x8004);
    assert(bus.cpu.a == 0x42);
    assert(bus.cpu.sp == initial_sp);
    assert(bus.cpu.cycles == 0);

    return 0;
}
