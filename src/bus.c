#include "bus.h"

void bus_init(Bus *bus) {
    // Clear RAM
    for (int i = 0; i < RAM_SIZE; i++) {
        bus->ram[i] = 0;
    }

    // Connect CPU to bus
    cpu_connect_bus(&bus->cpu, bus);
}

uint8_t bus_read(const Bus *bus, uint16_t addr) {
    return bus->ram[addr];
}

void bus_write(Bus *bus, uint16_t addr, uint8_t value) {
    bus->ram[addr] = value;
}
