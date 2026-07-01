#ifndef NES_CPU_H
#define NES_CPU_H

#include <stdbool.h>
#include <stdint.h>

#define TABLE_SIZE 256  // Instruction table size (16x16)

// Forward declarations
typedef struct Bus Bus;
typedef struct CPU CPU;

typedef enum Flag {
    C = 1 << 0, // Carry flag
    Z = 1 << 1, // Zero flag
    I = 1 << 2, // Interrupt disable
    D = 1 << 3, // Decimal mode (not implemented)
    B = 1 << 4, // Break command
    U = 1 << 5, // Unused
    V = 1 << 6, // Overflow flag
    N = 1 << 7, // Negative flag
} Flag;

typedef enum Opcode {
    // ADC - Add with carry
    ADC_IMM = 0x69, ADC_ZPG = 0x65, ADC_ZPX = 0x75, ADC_ABS = 0x6D, ADC_ABX = 0x7D,
    ADC_ABY = 0x79, ADC_IDX = 0x61, ADC_IDY = 0x71,

    // AND - Logical AND
    AND_IMM = 0x29, AND_ZPG = 0x25, AND_ZPX = 0x35, AND_ABS = 0x2D, AND_ABX = 0x3D,
    AND_ABY = 0x39, AND_IDX = 0x21, AND_IDY = 0x31,

    // ASL - Arithmetic shift left
    ASL_IMP = 0x0A, ASL_ZPG = 0x06, ASL_ZPX = 0x16, ASL_ABS = 0x0E, ASL_ABX = 0x1E,

    // BCC - Branch if carry clear
    BCC_REL = 0x90,

    // BCS - Branch if carry set
    BCS_REL = 0xB0,

    // BEQ - Branch if zero set
    BEQ_REL = 0xF0,

    // BIT - Bit test
    BIT_ZPG = 0x24, BIT_ABS = 0x2C,

    // BMI - Branch if negative set
    BMI_REL = 0x30,

    // BNE - Branch if zero clear
    BNE_REL = 0xD0,

    // BPL - Branch if negative clear
    BPL_REL = 0x10,

    // BRK - Force interrupt
    BRK_IMP = 0x00,

    // BVC - Branch if overflow clear
    BVC_REL = 0x50,

    // BVS - Branch if overflow set
    BVS_REL = 0x70,

    // CLC - Clear carry flag
    CLC_IMP = 0x18,

    // CLD - Clear decimal mode
    CLD_IMP = 0xD8,

    // CLI - Clear interrupt disable
    CLI_IMP = 0x58,

    // CLV - Clear overflow flag
    CLV_IMP = 0xB8,

    // CMP - Compare accumulator
    CMP_IMM = 0xC9, CMP_ZPG = 0xC5, CMP_ZPX = 0xD5, CMP_ABS = 0xCD, CMP_ABX = 0xDD,
    CMP_ABY = 0xD9, CMP_IDX = 0xC1, CMP_IDY = 0xD1,

    // CPX - Compare X register
    CPX_IMM = 0xE0, CPX_ZPG = 0xE4, CPX_ABS = 0xEC,

    // CPY - Compare Y register
    CPY_IMM = 0xC0, CPY_ZPG = 0xC4, CPY_ABS = 0xCC,

    // DEC - Decrement memory
    DEC_ZPG = 0xC6, DEC_ZPX = 0xD6, DEC_ABS = 0xCE, DEC_ABX = 0xDE,

    // DEX - Decrement X register
    DEX_IMP = 0xCA,

    // DEY - Decrement Y register
    DEY_IMP = 0x88,

    // EOR - Exclusive OR
    EOR_IMM = 0x49, EOR_ZPG = 0x45, EOR_ZPX = 0x55, EOR_ABS = 0x4D, EOR_ABX = 0x5D,
    EOR_ABY = 0x59, EOR_IDX = 0x41, EOR_IDY = 0x51,

    // INC - Increment memory
    INC_ZPG = 0xE6, INC_ZPX = 0xF6, INC_ABS = 0xEE, INC_ABX = 0xFE,

    // INX - Increment X register
    INX_IMP = 0xE8,

    // INY - Increment Y register
    INY_IMP = 0xC8,

    // JMP - Jump
    JMP_ABS = 0x4C, JMP_IND = 0x6C,

    // JSR - Jump to subroutine
    JSR_ABS = 0x20,

    // LDA - Load accumulator
    LDA_IMM = 0xA9, LDA_ZPG = 0xA5, LDA_ZPX = 0xB5, LDA_ABS = 0xAD, LDA_ABX = 0xBD,
    LDA_ABY = 0xB9, LDA_IDX = 0xA1, LDA_IDY = 0xB1,

    // LDX - Load X register
    LDX_IMM = 0xA2, LDX_ZPG = 0xA6, LDX_ZPY = 0xB6, LDX_ABS = 0xAE, LDX_ABY = 0xBE,

    // LDY - Load Y register
    LDY_IMM = 0xA0, LDY_ZPG = 0xA4, LDY_ZPX = 0xB4, LDY_ABS = 0xAC, LDY_ABX = 0xBC,

    // LSR - Logical shift right
    LSR_IMP = 0x4A, LSR_ZPG = 0x46, LSR_ZPX = 0x56, LSR_ABS = 0x4E, LSR_ABX = 0x5E,

    // NOP - No operation
    NOP_IMP = 0xEA,

    // ORA - Logical OR
    ORA_IMM = 0x09, ORA_ZPG = 0x05, ORA_ZPX = 0x15, ORA_ABS = 0x0D, ORA_ABX = 0x1D,
    ORA_ABY = 0x19, ORA_IDX = 0x01, ORA_IDY = 0x11,

    // PHA - Push accumulator onto stack
    PHA_IMP = 0x48,

    // PHP - Push processor status onto stack
    PHP_IMP = 0x08,

    // PLA - Pull accumulator from stack
    PLA_IMP = 0x68,

    // PLP - Pull processor status from stack
    PLP_IMP = 0x28,

    // ROL - Rotate left
    ROL_IMP = 0x2A, ROL_ZPG = 0x26, ROL_ZPX = 0x36, ROL_ABS = 0x2E, ROL_ABX = 0x3E,

    // ROR - Rotate right
    ROR_IMP = 0x6A, ROR_ZPG = 0x66, ROR_ZPX = 0x76, ROR_ABS = 0x6E, ROR_ABX = 0x7E,

    // RTI - Return from interrupt
    RTI_IMP = 0x40,

    // RTS - Return from subroutine
    RTS_IMP = 0x60,

    // SBC - Subtract with carry
    SBC_IMM = 0xE9, SBC_ZPG = 0xE5, SBC_ZPX = 0xF5, SBC_ABS = 0xED, SBC_ABX = 0xFD,
    SBC_ABY = 0xF9, SBC_IDX = 0xE1, SBC_IDY = 0xF1,

    // SEC - Set carry flag
    SEC_IMP = 0x38,

    // SED - Set decimal mode
    SED_IMP = 0xF8,

    // SEI - Set interrupt disable
    SEI_IMP = 0x78,

    // STA - Store accumulator
    STA_ZPG = 0x85, STA_ZPX = 0x95, STA_ABS = 0x8D, STA_ABX = 0x9D, STA_ABY = 0x99,
    STA_IDX = 0x81, STA_IDY = 0x91,

    // STX - Store X register
    STX_ZPG = 0x86, STX_ZPY = 0x96, STX_ABS = 0x8E,

    // STY - Store Y register
    STY_ZPG = 0x84, STY_ZPX = 0x94, STY_ABS = 0x8C,

    // TAX - Transfer accumulator to X
    TAX_IMP = 0xAA,

    // TAY - Transfer accumulator to Y
    TAY_IMP = 0xA8,

    // TSX - Transfer SP to X
    TSX_IMP = 0xBA,

    // TXA - Transfer X to accumulator
    TXA_IMP = 0x8A,

    // TXS - Transfer X to SP
    TXS_IMP = 0x9A,

    // TYA - Transfer Y to accumulator
    TYA_IMP = 0x98,
} Opcode;

typedef struct Instruction {
    const char *name;           // Pneumonic
    uint8_t (*address)(CPU *);  // Addressing mode handler
    uint8_t (*execute)(CPU *);  // Instruction implementation handler
    uint8_t cycles;             // Cycles required
} Instruction;

typedef struct CPU {
    Instruction table[TABLE_SIZE];  // Instruction table (bottom nibble = column, top nibble = row)
    Bus *bus;                       // Devices bus
    uint16_t pc;                    // Program counter
    uint8_t sp;                     // Stack pointer
    uint8_t a;                      // Accumulator
    uint8_t x;                      // Index register X
    uint8_t y;                      // Index register Y
    uint8_t status;                 // Processor status flags
    uint8_t cycles;                 // Cycles remaining for current instruction
} CPU;

// Initialization
void cpu_init_table(CPU *cpu);
void cpu_connect_bus(CPU *cpu, Bus *bus);

// Core
uint8_t cpu_read(const CPU *cpu, uint16_t addr);
void cpu_write(CPU *cpu, uint16_t addr, uint8_t value);
uint8_t cpu_fetch(CPU *cpu, uint16_t addr);
uint8_t cpu_get_flag(const CPU *cpu, Flag flag);
void cpu_set_flag(CPU *cpu, Flag flag, bool value);

// Signals
void cpu_clock(CPU *cpu);   // Clock cycle
void cpu_reset(CPU *cpu);   // Reset
void cpu_irq(CPU *cpu);     // Maskable interrupt
void cpu_nmi(CPU *cpu);     // Non-maskable interrupt

// Addressing modes
uint8_t cpu_imp(CPU *cpu);  // Implied
uint8_t cpu_imm(CPU *cpu);  // Immediate
uint8_t cpu_zpg(CPU *cpu);  // Zero Page
uint8_t cpu_zpx(CPU *cpu);  // Zero Page,X
uint8_t cpu_zpy(CPU *cpu);  // Zero Page,Y
uint8_t cpu_rel(CPU *cpu);  // Relative
uint8_t cpu_abs(CPU *cpu);  // Absolute
uint8_t cpu_abx(CPU *cpu);  // Absolute,X
uint8_t cpu_aby(CPU *cpu);  // Absolute,Y
uint8_t cpu_ind(CPU *cpu);  // Indirect
uint8_t cpu_idx(CPU *cpu);  // (Indirect,X)
uint8_t cpu_idy(CPU *cpu);  // (Indirect),Y

// Opcodes
uint8_t cpu_adc(CPU *cpu);  // Add with carry
uint8_t cpu_and(CPU *cpu);  // Logical AND
uint8_t cpu_asl(CPU *cpu);  // Arithmetic shift left
uint8_t cpu_bcc(CPU *cpu);  // Branch if carry clear
uint8_t cpu_bcs(CPU *cpu);  // Branch if carry set
uint8_t cpu_beq(CPU *cpu);  // Branch if zero set
uint8_t cpu_bit(CPU *cpu);  // Bit test
uint8_t cpu_bmi(CPU *cpu);  // Branch if negative set
uint8_t cpu_bne(CPU *cpu);  // Branch if zero clear
uint8_t cpu_bpl(CPU *cpu);  // Branch if negative clear
uint8_t cpu_brk(CPU *cpu);  // Force interrupt
uint8_t cpu_bvc(CPU *cpu);  // Branch if overflow clear
uint8_t cpu_bvs(CPU *cpu);  // Branch if overflow set
uint8_t cpu_clc(CPU *cpu);  // Clear carry flag
uint8_t cpu_cld(CPU *cpu);  // Clear decimal mode
uint8_t cpu_cli(CPU *cpu);  // Clear interrupt disable
uint8_t cpu_clv(CPU *cpu);  // Clear overflow flag
uint8_t cpu_cmp(CPU *cpu);  // Compare accumulator
uint8_t cpu_cpx(CPU *cpu);  // Compare X register
uint8_t cpu_cpy(CPU *cpu);  // Compare Y register
uint8_t cpu_dec(CPU *cpu);  // Decrement memory
uint8_t cpu_dex(CPU *cpu);  // Decrement X register
uint8_t cpu_dey(CPU *cpu);  // Decrement Y register
uint8_t cpu_eor(CPU *cpu);  // Exclusive OR
uint8_t cpu_inc(CPU *cpu);  // Increment memory
uint8_t cpu_inx(CPU *cpu);  // Increment X register
uint8_t cpu_iny(CPU *cpu);  // Increment Y register
uint8_t cpu_jmp(CPU *cpu);  // Jump
uint8_t cpu_jsr(CPU *cpu);  // Jump to subroutine
uint8_t cpu_lda(CPU *cpu);  // Load accumulator
uint8_t cpu_ldx(CPU *cpu);  // Load X register
uint8_t cpu_ldy(CPU *cpu);  // Load Y register
uint8_t cpu_lsr(CPU *cpu);  // Logical shift right
uint8_t cpu_nop(CPU *cpu);  // No operation
uint8_t cpu_ora(CPU *cpu);  // Logical OR
uint8_t cpu_pha(CPU *cpu);  // Push accumulator onto stack
uint8_t cpu_php(CPU *cpu);  // Push processor status onto stack
uint8_t cpu_pla(CPU *cpu);  // Pull accumulator from stack
uint8_t cpu_plp(CPU *cpu);  // Pull processor status from stack
uint8_t cpu_rol(CPU *cpu);  // Rotate left
uint8_t cpu_ror(CPU *cpu);  // Rotate right
uint8_t cpu_rti(CPU *cpu);  // Return from interrupt
uint8_t cpu_rts(CPU *cpu);  // Return from subroutine
uint8_t cpu_sbc(CPU *cpu);  // Subtract with carry
uint8_t cpu_sec(CPU *cpu);  // Set carry flag
uint8_t cpu_sed(CPU *cpu);  // Set decimal mode
uint8_t cpu_sei(CPU *cpu);  // Set interrupt disable
uint8_t cpu_sta(CPU *cpu);  // Store accumulator
uint8_t cpu_stx(CPU *cpu);  // Store X register
uint8_t cpu_sty(CPU *cpu);  // Store Y register
uint8_t cpu_tax(CPU *cpu);  // Transfer accumulator to X
uint8_t cpu_tay(CPU *cpu);  // Transfer accumulator to Y
uint8_t cpu_tsx(CPU *cpu);  // Transfer SP to X
uint8_t cpu_txa(CPU *cpu);  // Transfer X to accumulator
uint8_t cpu_txs(CPU *cpu);  // Transfer X to SP
uint8_t cpu_tya(CPU *cpu);  // Transfer Y to accumulator
uint8_t cpu_xxx(CPU *cpu);  // Illegal opcode

#endif
