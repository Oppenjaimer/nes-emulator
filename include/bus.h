#ifndef NES_BUS_H
#define NES_BUS_H

#include "cpu.h"

#include <stdint.h>

#define RAM_SIZE (1024 * 64) // 64 KB

typedef struct Bus {
    // Devices connected to bus
    CPU cpu;
    uint8_t ram[RAM_SIZE];
} Bus;

void bus_init(Bus *bus);
uint8_t bus_read(const Bus *bus, uint16_t addr);
void bus_write(Bus *bus, uint16_t addr, uint8_t value);

#endif
