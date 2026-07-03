#ifndef NES_CPU_H
#define NES_CPU_H

#include <stdbool.h>
#include <stdint.h>

#define TABLE_SIZE 256  // Instruction table size (16x16)

// Forward declarations
typedef struct Bus Bus;
typedef struct CPU CPU;

typedef enum Flag {
    FLAG_C = 1 << 0, // Carry flag
    FLAG_Z = 1 << 1, // Zero flag
    FLAG_I = 1 << 2, // Interrupt disable
    FLAG_D = 1 << 3, // Decimal mode (not implemented)
    FLAG_B = 1 << 4, // Break command
    FLAG_U = 1 << 5, // Unused
    FLAG_V = 1 << 6, // Overflow flag
    FLAG_N = 1 << 7, // Negative flag
} Flag;

typedef enum Opcode {
    // ADC - Add with carry
    OP_ADC_IMM = 0x69, OP_ADC_ZPG = 0x65, OP_ADC_ZPX = 0x75, OP_ADC_ABS = 0x6D, OP_ADC_ABX = 0x7D,
    OP_ADC_ABY = 0x79, OP_ADC_IDX = 0x61, OP_ADC_IDY = 0x71,

    // AND - Logical AND
    OP_AND_IMM = 0x29, OP_AND_ZPG = 0x25, OP_AND_ZPX = 0x35, OP_AND_ABS = 0x2D, OP_AND_ABX = 0x3D,
    OP_AND_ABY = 0x39, OP_AND_IDX = 0x21, OP_AND_IDY = 0x31,

    // ASL - Arithmetic shift left
    OP_ASL_IMP = 0x0A, OP_ASL_ZPG = 0x06, OP_ASL_ZPX = 0x16, OP_ASL_ABS = 0x0E, OP_ASL_ABX = 0x1E,

    // BCC - Branch if carry clear
    OP_BCC_REL = 0x90,

    // BCS - Branch if carry set
    OP_BCS_REL = 0xB0,

    // BEQ - Branch if zero set
    OP_BEQ_REL = 0xF0,

    // BIT - Bit test
    OP_BIT_ZPG = 0x24, OP_BIT_ABS = 0x2C,

    // BMI - Branch if negative set
    OP_BMI_REL = 0x30,

    // BNE - Branch if zero clear
    OP_BNE_REL = 0xD0,

    // BPL - Branch if negative clear
    OP_BPL_REL = 0x10,

    // BRK - Force interrupt
    OP_BRK_IMP = 0x00,

    // BVC - Branch if overflow clear
    OP_BVC_REL = 0x50,

    // BVS - Branch if overflow set
    OP_BVS_REL = 0x70,

    // CLC - Clear carry flag
    OP_CLC_IMP = 0x18,

    // CLD - Clear decimal mode
    OP_CLD_IMP = 0xD8,

    // CLI - Clear interrupt disable
    OP_CLI_IMP = 0x58,

    // CLV - Clear overflow flag
    OP_CLV_IMP = 0xB8,

    // CMP - Compare accumulator
    OP_CMP_IMM = 0xC9, OP_CMP_ZPG = 0xC5, OP_CMP_ZPX = 0xD5, OP_CMP_ABS = 0xCD, OP_CMP_ABX = 0xDD,
    OP_CMP_ABY = 0xD9, OP_CMP_IDX = 0xC1, OP_CMP_IDY = 0xD1,

    // CPX - Compare X register
    OP_CPX_IMM = 0xE0, OP_CPX_ZPG = 0xE4, OP_CPX_ABS = 0xEC,

    // CPY - Compare Y register
    OP_CPY_IMM = 0xC0, OP_CPY_ZPG = 0xC4, OP_CPY_ABS = 0xCC,

    // DEC - Decrement memory
    OP_DEC_ZPG = 0xC6, OP_DEC_ZPX = 0xD6, OP_DEC_ABS = 0xCE, OP_DEC_ABX = 0xDE,

    // DEX - Decrement X register
    OP_DEX_IMP = 0xCA,

    // DEY - Decrement Y register
    OP_DEY_IMP = 0x88,

    // EOR - Exclusive OR
    OP_EOR_IMM = 0x49, OP_EOR_ZPG = 0x45, OP_EOR_ZPX = 0x55, OP_EOR_ABS = 0x4D, OP_EOR_ABX = 0x5D,
    OP_EOR_ABY = 0x59, OP_EOR_IDX = 0x41, OP_EOR_IDY = 0x51,

    // INC - Increment memory
    OP_INC_ZPG = 0xE6, OP_INC_ZPX = 0xF6, OP_INC_ABS = 0xEE, OP_INC_ABX = 0xFE,

    // INX - Increment X register
    OP_INX_IMP = 0xE8,

    // INY - Increment Y register
    OP_INY_IMP = 0xC8,

    // JMP - Jump
    OP_JMP_ABS = 0x4C, OP_JMP_IND = 0x6C,

    // JSR - Jump to subroutine
    OP_JSR_ABS = 0x20,

    // LDA - Load accumulator
    OP_LDA_IMM = 0xA9, OP_LDA_ZPG = 0xA5, OP_LDA_ZPX = 0xB5, OP_LDA_ABS = 0xAD, OP_LDA_ABX = 0xBD,
    OP_LDA_ABY = 0xB9, OP_LDA_IDX = 0xA1, OP_LDA_IDY = 0xB1,

    // LDX - Load X register
    OP_LDX_IMM = 0xA2, OP_LDX_ZPG = 0xA6, OP_LDX_ZPY = 0xB6, OP_LDX_ABS = 0xAE, OP_LDX_ABY = 0xBE,

    // LDY - Load Y register
    OP_LDY_IMM = 0xA0, OP_LDY_ZPG = 0xA4, OP_LDY_ZPX = 0xB4, OP_LDY_ABS = 0xAC, OP_LDY_ABX = 0xBC,

    // LSR - Logical shift right
    OP_LSR_IMP = 0x4A, OP_LSR_ZPG = 0x46, OP_LSR_ZPX = 0x56, OP_LSR_ABS = 0x4E, OP_LSR_ABX = 0x5E,

    // NOP - No operation
    OP_NOP_IMP = 0xEA,

    // ORA - Logical OR
    OP_ORA_IMM = 0x09, OP_ORA_ZPG = 0x05, OP_ORA_ZPX = 0x15, OP_ORA_ABS = 0x0D, OP_ORA_ABX = 0x1D,
    OP_ORA_ABY = 0x19, OP_ORA_IDX = 0x01, OP_ORA_IDY = 0x11,

    // PHA - Push accumulator onto stack
    OP_PHA_IMP = 0x48,

    // PHP - Push processor status onto stack
    OP_PHP_IMP = 0x08,

    // PLA - Pull accumulator from stack
    OP_PLA_IMP = 0x68,

    // PLP - Pull processor status from stack
    OP_PLP_IMP = 0x28,

    // ROL - Rotate left
    OP_ROL_IMP = 0x2A, OP_ROL_ZPG = 0x26, OP_ROL_ZPX = 0x36, OP_ROL_ABS = 0x2E, OP_ROL_ABX = 0x3E,

    // ROR - Rotate right
    OP_ROR_IMP = 0x6A, OP_ROR_ZPG = 0x66, OP_ROR_ZPX = 0x76, OP_ROR_ABS = 0x6E, OP_ROR_ABX = 0x7E,

    // RTI - Return from interrupt
    OP_RTI_IMP = 0x40,

    // RTS - Return from subroutine
    OP_RTS_IMP = 0x60,

    // SBC - Subtract with carry
    OP_SBC_IMM = 0xE9, OP_SBC_ZPG = 0xE5, OP_SBC_ZPX = 0xF5, OP_SBC_ABS = 0xED, OP_SBC_ABX = 0xFD,
    OP_SBC_ABY = 0xF9, OP_SBC_IDX = 0xE1, OP_SBC_IDY = 0xF1,

    // SEC - Set carry flag
    OP_SEC_IMP = 0x38,

    // SED - Set decimal mode
    OP_SED_IMP = 0xF8,

    // SEI - Set interrupt disable
    OP_SEI_IMP = 0x78,

    // STA - Store accumulator
    OP_STA_ZPG = 0x85, OP_STA_ZPX = 0x95, OP_STA_ABS = 0x8D, OP_STA_ABX = 0x9D, OP_STA_ABY = 0x99,
    OP_STA_IDX = 0x81, OP_STA_IDY = 0x91,

    // STX - Store X register
    OP_STX_ZPG = 0x86, OP_STX_ZPY = 0x96, OP_STX_ABS = 0x8E,

    // STY - Store Y register
    OP_STY_ZPG = 0x84, OP_STY_ZPX = 0x94, OP_STY_ABS = 0x8C,

    // TAX - Transfer accumulator to X
    OP_TAX_IMP = 0xAA,

    // TAY - Transfer accumulator to Y
    OP_TAY_IMP = 0xA8,

    // TSX - Transfer SP to X
    OP_TSX_IMP = 0xBA,

    // TXA - Transfer X to accumulator
    OP_TXA_IMP = 0x8A,

    // TXS - Transfer X to SP
    OP_TXS_IMP = 0x9A,

    // TYA - Transfer Y to accumulator
    OP_TYA_IMP = 0x98,
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

    uint16_t addr;                  // Absolute address resulting from addressing mode
    uint8_t opcode;                 // Currently executing opcode
    uint8_t fetched;                // Working memory value
    uint8_t cycles;                 // Cycles remaining for current instruction
} CPU;

// Initialization
void cpu_init_table(CPU *cpu);
void cpu_connect_bus(CPU *cpu, Bus *bus);

// Core
uint8_t cpu_read_byte(const CPU *cpu, uint16_t addr);
uint16_t cpu_read_word(const CPU *cpu, uint16_t addr);
void cpu_write_byte(CPU *cpu, uint16_t addr, uint8_t value);
void cpu_write_word(CPU *cpu, uint16_t addr, uint16_t value);
uint8_t cpu_fetch_byte(CPU *cpu);
uint16_t cpu_fetch_word(CPU *cpu);
void cpu_fetch_value(CPU *cpu);
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

// Instructions
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
