#include "cpu.h"

// Forward declarations
uint8_t bus_read(const Bus *bus, uint16_t addr);
void bus_write(Bus *bus, uint16_t addr, uint8_t value);

// ======================================================================================================
// HELPERS
// ======================================================================================================

bool is_page_crossed(uint16_t base_addr, uint16_t eff_addr) {
    return (base_addr & 0xFF00) != (eff_addr & 0xFF00);
}

bool is_bit_set(uint16_t value, uint8_t bit) {
    return value & (1 << bit);
}

uint8_t get_bit(uint8_t byte, uint8_t bit) {
    return (byte >> bit) & 1;
}

// ======================================================================================================
// CORE
// ======================================================================================================

void cpu_init_table(CPU *cpu) {
    // Initialize instruction table with illegal opcodes
    for (int i = 0; i < TABLE_SIZE; i++) {
        cpu->table[i] = (Instruction){"???", &cpu_imp, &cpu_xxx, 2};
    }

    // Set legal opcodes
    cpu->table[OP_ADC_ABS] = (Instruction){"ADC_ABS", &cpu_abs, &cpu_adc, 4};
    cpu->table[OP_ADC_ABX] = (Instruction){"ADC_ABX", &cpu_abx, &cpu_adc, 4};
    cpu->table[OP_ADC_ABY] = (Instruction){"ADC_ABY", &cpu_aby, &cpu_adc, 4};
    cpu->table[OP_ADC_IDX] = (Instruction){"ADC_IDX", &cpu_idx, &cpu_adc, 6};
    cpu->table[OP_ADC_IDY] = (Instruction){"ADC_IDY", &cpu_idy, &cpu_adc, 5};
    cpu->table[OP_ADC_IMM] = (Instruction){"ADC_IMM", &cpu_imm, &cpu_adc, 2};
    cpu->table[OP_ADC_ZPG] = (Instruction){"ADC_ZPG", &cpu_zpg, &cpu_adc, 3};
    cpu->table[OP_ADC_ZPX] = (Instruction){"ADC_ZPX", &cpu_zpx, &cpu_adc, 4};

    cpu->table[OP_AND_ABS] = (Instruction){"AND_ABS", &cpu_abs, &cpu_and, 4};
    cpu->table[OP_AND_ABX] = (Instruction){"AND_ABX", &cpu_abx, &cpu_and, 4};
    cpu->table[OP_AND_ABY] = (Instruction){"AND_ABY", &cpu_aby, &cpu_and, 4};
    cpu->table[OP_AND_IDX] = (Instruction){"AND_IDX", &cpu_idx, &cpu_and, 6};
    cpu->table[OP_AND_IDY] = (Instruction){"AND_IDY", &cpu_idy, &cpu_and, 5};
    cpu->table[OP_AND_IMM] = (Instruction){"AND_IMM", &cpu_imm, &cpu_and, 2};
    cpu->table[OP_AND_ZPG] = (Instruction){"AND_ZPG", &cpu_zpg, &cpu_and, 3};
    cpu->table[OP_AND_ZPX] = (Instruction){"AND_ZPX", &cpu_zpx, &cpu_and, 4};

    cpu->table[OP_ASL_ABS] = (Instruction){"ASL_ABS", &cpu_abs, &cpu_asl, 6};
    cpu->table[OP_ASL_ABX] = (Instruction){"ASL_ABX", &cpu_abx, &cpu_asl, 7};
    cpu->table[OP_ASL_IMP] = (Instruction){"ASL_IMP", &cpu_imp, &cpu_asl, 2};
    cpu->table[OP_ASL_ZPG] = (Instruction){"ASL_ZPG", &cpu_zpg, &cpu_asl, 5};
    cpu->table[OP_ASL_ZPX] = (Instruction){"ASL_ZPX", &cpu_zpx, &cpu_asl, 6};

    cpu->table[OP_BCC_REL] = (Instruction){"BCC_REL", &cpu_rel, &cpu_bcc, 2};
    cpu->table[OP_BCS_REL] = (Instruction){"BCS_REL", &cpu_rel, &cpu_bcs, 2};
    cpu->table[OP_BEQ_REL] = (Instruction){"BEQ_REL", &cpu_rel, &cpu_beq, 2};

    cpu->table[OP_BIT_ABS] = (Instruction){"BIT_ABS", &cpu_abs, &cpu_bit, 4};
    cpu->table[OP_BIT_ZPG] = (Instruction){"BIT_ZPG", &cpu_zpg, &cpu_bit, 3};

    cpu->table[OP_BMI_REL] = (Instruction){"BMI_REL", &cpu_rel, &cpu_bmi, 2};
    cpu->table[OP_BNE_REL] = (Instruction){"BNE_REL", &cpu_rel, &cpu_bne, 2};
    cpu->table[OP_BPL_REL] = (Instruction){"BPL_REL", &cpu_rel, &cpu_bpl, 2};

    cpu->table[OP_BRK_IMP] = (Instruction){"BRK_IMP", &cpu_imp, &cpu_brk, 7};

    cpu->table[OP_BVC_REL] = (Instruction){"BVC_REL", &cpu_rel, &cpu_bvc, 2};
    cpu->table[OP_BVS_REL] = (Instruction){"BVS_REL", &cpu_rel, &cpu_bvs, 2};

    cpu->table[OP_CLC_IMP] = (Instruction){"CLC_IMP", &cpu_imp, &cpu_clc, 2};
    cpu->table[OP_CLD_IMP] = (Instruction){"CLD_IMP", &cpu_imp, &cpu_cld, 2};
    cpu->table[OP_CLI_IMP] = (Instruction){"CLI_IMP", &cpu_imp, &cpu_cli, 2};
    cpu->table[OP_CLV_IMP] = (Instruction){"CLV_IMP", &cpu_imp, &cpu_clv, 2};

    cpu->table[OP_CMP_ABS] = (Instruction){"CMP_ABS", &cpu_abs, &cpu_cmp, 4};
    cpu->table[OP_CMP_ABX] = (Instruction){"CMP_ABX", &cpu_abx, &cpu_cmp, 4};
    cpu->table[OP_CMP_ABY] = (Instruction){"CMP_ABY", &cpu_aby, &cpu_cmp, 4};
    cpu->table[OP_CMP_IDX] = (Instruction){"CMP_IDX", &cpu_idx, &cpu_cmp, 6};
    cpu->table[OP_CMP_IDY] = (Instruction){"CMP_IDY", &cpu_idy, &cpu_cmp, 5};
    cpu->table[OP_CMP_IMM] = (Instruction){"CMP_IMM", &cpu_imm, &cpu_cmp, 2};
    cpu->table[OP_CMP_ZPG] = (Instruction){"CMP_ZPG", &cpu_zpg, &cpu_cmp, 3};
    cpu->table[OP_CMP_ZPX] = (Instruction){"CMP_ZPX", &cpu_zpx, &cpu_cmp, 4};

    cpu->table[OP_CPX_ABS] = (Instruction){"CPX_ABS", &cpu_abs, &cpu_cpx, 4};
    cpu->table[OP_CPX_IMM] = (Instruction){"CPX_IMM", &cpu_imm, &cpu_cpx, 2};
    cpu->table[OP_CPX_ZPG] = (Instruction){"CPX_ZPG", &cpu_zpg, &cpu_cpx, 3};

    cpu->table[OP_CPY_ABS] = (Instruction){"CPY_ABS", &cpu_abs, &cpu_cpy, 4};
    cpu->table[OP_CPY_IMM] = (Instruction){"CPY_IMM", &cpu_imm, &cpu_cpy, 2};
    cpu->table[OP_CPY_ZPG] = (Instruction){"CPY_ZPG", &cpu_zpg, &cpu_cpy, 3};

    cpu->table[OP_DEC_ABS] = (Instruction){"DEC_ABS", &cpu_abs, &cpu_dec, 6};
    cpu->table[OP_DEC_ABX] = (Instruction){"DEC_ABX", &cpu_abx, &cpu_dec, 7};
    cpu->table[OP_DEC_ZPG] = (Instruction){"DEC_ZPG", &cpu_zpg, &cpu_dec, 5};
    cpu->table[OP_DEC_ZPX] = (Instruction){"DEC_ZPX", &cpu_zpx, &cpu_dec, 6};

    cpu->table[OP_DEX_IMP] = (Instruction){"DEX_IMP", &cpu_imp, &cpu_dex, 2};
    cpu->table[OP_DEY_IMP] = (Instruction){"DEY_IMP", &cpu_imp, &cpu_dey, 2};

    cpu->table[OP_EOR_ABS] = (Instruction){"EOR_ABS", &cpu_abs, &cpu_eor, 4};
    cpu->table[OP_EOR_ABX] = (Instruction){"EOR_ABX", &cpu_abx, &cpu_eor, 4};
    cpu->table[OP_EOR_ABY] = (Instruction){"EOR_ABY", &cpu_aby, &cpu_eor, 4};
    cpu->table[OP_EOR_IDX] = (Instruction){"EOR_IDX", &cpu_idx, &cpu_eor, 6};
    cpu->table[OP_EOR_IDY] = (Instruction){"EOR_IDY", &cpu_idy, &cpu_eor, 5};
    cpu->table[OP_EOR_IMM] = (Instruction){"EOR_IMM", &cpu_imm, &cpu_eor, 2};
    cpu->table[OP_EOR_ZPG] = (Instruction){"EOR_ZPG", &cpu_zpg, &cpu_eor, 3};
    cpu->table[OP_EOR_ZPX] = (Instruction){"EOR_ZPX", &cpu_zpx, &cpu_eor, 4};

    cpu->table[OP_INC_ABS] = (Instruction){"INC_ABS", &cpu_abs, &cpu_inc, 6};
    cpu->table[OP_INC_ABX] = (Instruction){"INC_ABX", &cpu_abx, &cpu_inc, 7};
    cpu->table[OP_INC_ZPG] = (Instruction){"INC_ZPG", &cpu_zpg, &cpu_inc, 5};
    cpu->table[OP_INC_ZPX] = (Instruction){"INC_ZPX", &cpu_zpx, &cpu_inc, 6};

    cpu->table[OP_INX_IMP] = (Instruction){"INX_IMP", &cpu_imp, &cpu_inx, 2};
    cpu->table[OP_INY_IMP] = (Instruction){"INY_IMP", &cpu_imp, &cpu_iny, 2};

    cpu->table[OP_JMP_ABS] = (Instruction){"JMP_ABS", &cpu_abs, &cpu_jmp, 3};
    cpu->table[OP_JMP_IND] = (Instruction){"JMP_IND", &cpu_ind, &cpu_jmp, 5};

    cpu->table[OP_JSR_ABS] = (Instruction){"JSR_ABS", &cpu_abs, &cpu_jsr, 6};

    cpu->table[OP_LDA_ABS] = (Instruction){"LDA_ABS", &cpu_abs, &cpu_lda, 4};
    cpu->table[OP_LDA_ABX] = (Instruction){"LDA_ABX", &cpu_abx, &cpu_lda, 4};
    cpu->table[OP_LDA_ABY] = (Instruction){"LDA_ABY", &cpu_aby, &cpu_lda, 4};
    cpu->table[OP_LDA_IDX] = (Instruction){"LDA_IDX", &cpu_idx, &cpu_lda, 6};
    cpu->table[OP_LDA_IDY] = (Instruction){"LDA_IDY", &cpu_idy, &cpu_lda, 5};
    cpu->table[OP_LDA_IMM] = (Instruction){"LDA_IMM", &cpu_imm, &cpu_lda, 2};
    cpu->table[OP_LDA_ZPG] = (Instruction){"LDA_ZPG", &cpu_zpg, &cpu_lda, 3};
    cpu->table[OP_LDA_ZPX] = (Instruction){"LDA_ZPX", &cpu_zpx, &cpu_lda, 4};

    cpu->table[OP_LDX_ABS] = (Instruction){"LDX_ABS", &cpu_abs, &cpu_ldx, 4};
    cpu->table[OP_LDX_ABY] = (Instruction){"LDX_ABY", &cpu_aby, &cpu_ldx, 4};
    cpu->table[OP_LDX_IMM] = (Instruction){"LDX_IMM", &cpu_imm, &cpu_ldx, 2};
    cpu->table[OP_LDX_ZPG] = (Instruction){"LDX_ZPG", &cpu_zpg, &cpu_ldx, 3};
    cpu->table[OP_LDX_ZPY] = (Instruction){"LDX_ZPY", &cpu_zpy, &cpu_ldx, 4};

    cpu->table[OP_LDY_ABS] = (Instruction){"LDY_ABS", &cpu_abs, &cpu_ldy, 4};
    cpu->table[OP_LDY_ABX] = (Instruction){"LDY_ABX", &cpu_abx, &cpu_ldy, 4};
    cpu->table[OP_LDY_IMM] = (Instruction){"LDY_IMM", &cpu_imm, &cpu_ldy, 2};
    cpu->table[OP_LDY_ZPG] = (Instruction){"LDY_ZPG", &cpu_zpg, &cpu_ldy, 3};
    cpu->table[OP_LDY_ZPX] = (Instruction){"LDY_ZPX", &cpu_zpx, &cpu_ldy, 4};

    cpu->table[OP_LSR_ABS] = (Instruction){"LSR_ABS", &cpu_abs, &cpu_lsr, 6};
    cpu->table[OP_LSR_ABX] = (Instruction){"LSR_ABX", &cpu_abx, &cpu_lsr, 7};
    cpu->table[OP_LSR_IMP] = (Instruction){"LSR_IMP", &cpu_imp, &cpu_lsr, 2};
    cpu->table[OP_LSR_ZPG] = (Instruction){"LSR_ZPG", &cpu_zpg, &cpu_lsr, 5};
    cpu->table[OP_LSR_ZPX] = (Instruction){"LSR_ZPX", &cpu_zpx, &cpu_lsr, 6};

    cpu->table[OP_NOP_IMP] = (Instruction){"NOP_IMP", &cpu_imp, &cpu_nop, 2};

    cpu->table[OP_ORA_ABS] = (Instruction){"ORA_ABS", &cpu_abs, &cpu_ora, 4};
    cpu->table[OP_ORA_ABX] = (Instruction){"ORA_ABX", &cpu_abx, &cpu_ora, 4};
    cpu->table[OP_ORA_ABY] = (Instruction){"ORA_ABY", &cpu_aby, &cpu_ora, 4};
    cpu->table[OP_ORA_IDX] = (Instruction){"ORA_IDX", &cpu_idx, &cpu_ora, 6};
    cpu->table[OP_ORA_IDY] = (Instruction){"ORA_IDY", &cpu_idy, &cpu_ora, 5};
    cpu->table[OP_ORA_IMM] = (Instruction){"ORA_IMM", &cpu_imm, &cpu_ora, 2};
    cpu->table[OP_ORA_ZPG] = (Instruction){"ORA_ZPG", &cpu_zpg, &cpu_ora, 3};
    cpu->table[OP_ORA_ZPX] = (Instruction){"ORA_ZPX", &cpu_zpx, &cpu_ora, 4};

    cpu->table[OP_PHA_IMP] = (Instruction){"PHA_IMP", &cpu_imp, &cpu_pha, 3};
    cpu->table[OP_PHP_IMP] = (Instruction){"PHP_IMP", &cpu_imp, &cpu_php, 3};
    cpu->table[OP_PLA_IMP] = (Instruction){"PLA_IMP", &cpu_imp, &cpu_pla, 4};
    cpu->table[OP_PLP_IMP] = (Instruction){"PLP_IMP", &cpu_imp, &cpu_plp, 4};

    cpu->table[OP_ROL_ABS] = (Instruction){"ROL_ABS", &cpu_abs, &cpu_rol, 6};
    cpu->table[OP_ROL_ABX] = (Instruction){"ROL_ABX", &cpu_abx, &cpu_rol, 7};
    cpu->table[OP_ROL_IMP] = (Instruction){"ROL_IMP", &cpu_imp, &cpu_rol, 2};
    cpu->table[OP_ROL_ZPG] = (Instruction){"ROL_ZPG", &cpu_zpg, &cpu_rol, 5};
    cpu->table[OP_ROL_ZPX] = (Instruction){"ROL_ZPX", &cpu_zpx, &cpu_rol, 6};

    cpu->table[OP_ROR_ABS] = (Instruction){"ROR_ABS", &cpu_abs, &cpu_ror, 6};
    cpu->table[OP_ROR_ABX] = (Instruction){"ROR_ABX", &cpu_abx, &cpu_ror, 7};
    cpu->table[OP_ROR_IMP] = (Instruction){"ROR_IMP", &cpu_imp, &cpu_ror, 2};
    cpu->table[OP_ROR_ZPG] = (Instruction){"ROR_ZPG", &cpu_zpg, &cpu_ror, 5};
    cpu->table[OP_ROR_ZPX] = (Instruction){"ROR_ZPX", &cpu_zpx, &cpu_ror, 6};

    cpu->table[OP_RTI_IMP] = (Instruction){"RTI_IMP", &cpu_imp, &cpu_rti, 6};
    cpu->table[OP_RTS_IMP] = (Instruction){"RTS_IMP", &cpu_imp, &cpu_rts, 6};

    cpu->table[OP_SBC_ABS] = (Instruction){"SBC_ABS", &cpu_abs, &cpu_sbc, 4};
    cpu->table[OP_SBC_ABX] = (Instruction){"SBC_ABX", &cpu_abx, &cpu_sbc, 4};
    cpu->table[OP_SBC_ABY] = (Instruction){"SBC_ABY", &cpu_aby, &cpu_sbc, 4};
    cpu->table[OP_SBC_IDX] = (Instruction){"SBC_IDX", &cpu_idx, &cpu_sbc, 6};
    cpu->table[OP_SBC_IDY] = (Instruction){"SBC_IDY", &cpu_idy, &cpu_sbc, 5};
    cpu->table[OP_SBC_IMM] = (Instruction){"SBC_IMM", &cpu_imm, &cpu_sbc, 2};
    cpu->table[OP_SBC_ZPG] = (Instruction){"SBC_ZPG", &cpu_zpg, &cpu_sbc, 3};
    cpu->table[OP_SBC_ZPX] = (Instruction){"SBC_ZPX", &cpu_zpx, &cpu_sbc, 4};

    cpu->table[OP_SEC_IMP] = (Instruction){"SEC_IMP", &cpu_imp, &cpu_sec, 2};
    cpu->table[OP_SED_IMP] = (Instruction){"SED_IMP", &cpu_imp, &cpu_sed, 2};
    cpu->table[OP_SEI_IMP] = (Instruction){"SEI_IMP", &cpu_imp, &cpu_sei, 2};

    cpu->table[OP_STA_ABS] = (Instruction){"STA_ABS", &cpu_abs, &cpu_sta, 4};
    cpu->table[OP_STA_ABX] = (Instruction){"STA_ABX", &cpu_abx, &cpu_sta, 5};
    cpu->table[OP_STA_ABY] = (Instruction){"STA_ABY", &cpu_aby, &cpu_sta, 5};
    cpu->table[OP_STA_IDX] = (Instruction){"STA_IDX", &cpu_idx, &cpu_sta, 6};
    cpu->table[OP_STA_IDY] = (Instruction){"STA_IDY", &cpu_idy, &cpu_sta, 6};
    cpu->table[OP_STA_ZPG] = (Instruction){"STA_ZPG", &cpu_zpg, &cpu_sta, 3};
    cpu->table[OP_STA_ZPX] = (Instruction){"STA_ZPX", &cpu_zpx, &cpu_sta, 4};

    cpu->table[OP_STX_ABS] = (Instruction){"STX_ABS", &cpu_abs, &cpu_stx, 4};
    cpu->table[OP_STX_ZPG] = (Instruction){"STX_ZPG", &cpu_zpg, &cpu_stx, 3};
    cpu->table[OP_STX_ZPY] = (Instruction){"STX_ZPY", &cpu_zpy, &cpu_stx, 4};

    cpu->table[OP_STY_ABS] = (Instruction){"STY_ABS", &cpu_abs, &cpu_sty, 4};
    cpu->table[OP_STY_ZPG] = (Instruction){"STY_ZPG", &cpu_zpg, &cpu_sty, 3};
    cpu->table[OP_STY_ZPX] = (Instruction){"STY_ZPX", &cpu_zpx, &cpu_sty, 4};

    cpu->table[OP_TAX_IMP] = (Instruction){"TAX_IMP", &cpu_imp, &cpu_tax, 2};
    cpu->table[OP_TAY_IMP] = (Instruction){"TAY_IMP", &cpu_imp, &cpu_tay, 2};

    cpu->table[OP_TSX_IMP] = (Instruction){"TSX_IMP", &cpu_imp, &cpu_tsx, 2};
    cpu->table[OP_TXA_IMP] = (Instruction){"TXA_IMP", &cpu_imp, &cpu_txa, 2};
    cpu->table[OP_TXS_IMP] = (Instruction){"TXS_IMP", &cpu_imp, &cpu_txs, 2};
    cpu->table[OP_TYA_IMP] = (Instruction){"TYA_IMP", &cpu_imp, &cpu_tya, 2};
}

void cpu_connect_bus(CPU *cpu, Bus *bus) {
    cpu->bus = bus;
}

uint8_t cpu_read_byte(const CPU *cpu, uint16_t addr) {
    return bus_read(cpu->bus, addr);
}

uint16_t cpu_read_word(const CPU *cpu, uint16_t addr) {
    uint16_t low = cpu_read_byte(cpu, addr + 0);
    uint16_t high = cpu_read_byte(cpu, addr + 1);

    return (high << 8) | low;
}

void cpu_write_byte(CPU *cpu, uint16_t addr, uint8_t value) {
    bus_write(cpu->bus, addr, value);
}

void cpu_write_word(CPU *cpu, uint16_t addr, uint16_t value) {
    cpu_write_byte(cpu, addr + 0, value & 0x00FF);
    cpu_write_byte(cpu, addr + 1, value >> 8);
}

uint8_t cpu_fetch_byte(CPU *cpu) {
    uint8_t byte = cpu_read_byte(cpu, cpu->pc);
    cpu->pc++;

    return byte;
}

uint16_t cpu_fetch_word(CPU *cpu) {
    uint16_t low = cpu_fetch_byte(cpu);
    uint16_t high = cpu_fetch_byte(cpu);

    return (high << 8) | low;
}

void cpu_fetch_value(CPU *cpu) {
    // Read from resolved address when mode is not implied
    if (cpu->table[cpu->opcode].address != &cpu_imp)
        cpu->fetched = cpu_read_byte(cpu, cpu->addr);
}

uint8_t cpu_get_flag(const CPU *cpu, Flag flag) {
    return (cpu->status & flag);
}

void cpu_set_flag(CPU *cpu, Flag flag, bool value) {
    if (value) cpu->status |= flag;
    else cpu->status &= ~flag;
}

uint8_t cpu_branch_if(CPU *cpu, Flag flag, bool expected) {
    if (cpu_get_flag(cpu, flag) != expected) return 0;

    // Add extra cycle if branch succeeds
    cpu->cycles++;

    // Add extra cycle if a page is crossed
    if (is_page_crossed(cpu->pc, cpu->addr))
        cpu->cycles++;

    cpu->pc = cpu->addr;

    return 0;
}

// ======================================================================================================
// STACK
// ======================================================================================================

uint16_t cpu_get_stack_address(const CPU *cpu) {
    return STACK_BASE | cpu->sp;
}

void cpu_stack_push_byte(CPU *cpu, uint8_t value) {
    cpu_write_byte(cpu, cpu_get_stack_address(cpu), value);
    cpu->sp--;
}

void cpu_stack_push_word(CPU *cpu, uint16_t value) {
    cpu_stack_push_byte(cpu, value >> 8);
    cpu_stack_push_byte(cpu, value & 0x00FF);
}

// ======================================================================================================
// SIGNALS
// ======================================================================================================

void cpu_clock(CPU *cpu) {
    if (cpu->cycles == 0) {
        cpu->opcode = cpu_fetch_byte(cpu);
        Instruction instruction = cpu->table[cpu->opcode];

        cpu->cycles = instruction.cycles;

        uint8_t extra_cycle1 = cpu->table[cpu->opcode].address(cpu);
        uint8_t extra_cycle2 = cpu->table[cpu->opcode].execute(cpu);

        cpu->cycles += (extra_cycle1 & extra_cycle2);
    }

    if (cpu->cycles > 0) cpu->cycles--;
}

void cpu_reset(CPU *cpu) {
    cpu->pc      = cpu_read_word(cpu, RESET_VECTOR);
    cpu->sp      = RESET_SP;
    cpu->a       = RESET_REG;
    cpu->x       = RESET_REG;
    cpu->y       = RESET_REG;
    cpu->status  = RESET_STATUS;

    cpu->addr    = 0x0000;
    cpu->opcode  = 0x00;
    cpu->fetched = 0x00;
    cpu->cycles  = RESET_CYCLES;
}

void cpu_irq(CPU *cpu) {
    if (cpu_get_flag(cpu, FLAG_I)) return;

    // Clear break flag to distinguish hardware IRQs from software BRKs
    cpu_set_flag(cpu, FLAG_B, false);

    cpu_stack_push_word(cpu, cpu->pc);
    cpu_stack_push_byte(cpu, cpu->status);

    // Disable interrupts while IRQ is running
    cpu_set_flag(cpu, FLAG_I, true);

    cpu->pc = cpu_read_word(cpu, INTER_VECTOR);
    cpu->cycles = 7;
}

void cpu_nmi(CPU *cpu) {
    // No NMI/BRK distinction strictly needed, but break flag still needs to be cleared
    cpu_set_flag(cpu, FLAG_B, false);

    cpu_stack_push_word(cpu, cpu->pc);
    cpu_stack_push_byte(cpu, cpu->status);

    // Disable interrupts while NMI is running
    cpu_set_flag(cpu, FLAG_I, true);

    cpu->pc = cpu_read_word(cpu, NMI_VECTOR);
    cpu->cycles = 8;
}

// ======================================================================================================
// ADDRESSING MODES
// ======================================================================================================

uint8_t cpu_imp(CPU *cpu) {
    // Handle accumulator addressing mode as implied
    cpu->fetched = cpu->a;

    return 0;
}

uint8_t cpu_imm(CPU *cpu) {
    cpu->addr = cpu->pc++;

    return 0;
}

uint8_t cpu_zpg(CPU *cpu) {
    cpu->addr = cpu_fetch_byte(cpu);

    return 0;
}

uint8_t cpu_zpx(CPU *cpu) {
    cpu->addr = cpu_fetch_byte(cpu) + cpu->x;
    cpu->addr &= 0x00FF;

    return 0;
}

uint8_t cpu_zpy(CPU *cpu) {
    cpu->addr = cpu_fetch_byte(cpu) + cpu->y;
    cpu->addr &= 0x00FF;

    return 0;
}

uint8_t cpu_rel(CPU *cpu) {
    uint16_t rel_addr = cpu_fetch_byte(cpu);

    // Handle signed relative address
    if (is_bit_set(rel_addr, 7))
        rel_addr |= 0xFF00;

    cpu->addr = cpu->pc + rel_addr;

    // Extra cycles handled by branching implementation
    return 0;
}

uint8_t cpu_abs(CPU *cpu) {
    cpu->addr = cpu_fetch_word(cpu);
    return 0;
}

uint8_t cpu_abx(CPU *cpu) {
    uint16_t base_addr = cpu_fetch_word(cpu);
    cpu->addr = base_addr + cpu->x;

    return is_page_crossed(base_addr, cpu->addr);
}

uint8_t cpu_aby(CPU *cpu) {
    uint16_t base_addr = cpu_fetch_word(cpu);
    cpu->addr = base_addr + cpu->y;

    return is_page_crossed(base_addr, cpu->addr);
}

uint8_t cpu_ind(CPU *cpu) {
    uint16_t ptr_addr = cpu_fetch_word(cpu);

    if ((ptr_addr & 0x00FF) == 0xFF) {
        // Simulate 6502 page boundary bug
        uint16_t low = cpu_read_byte(cpu, ptr_addr);
        uint16_t high = cpu_read_byte(cpu, ptr_addr & 0xFF00);
        cpu->addr = (high << 8) | low;
    } else {
        // Normal behavior
        cpu->addr = cpu_read_word(cpu, ptr_addr);
    }

    return 0;
}

uint8_t cpu_idx(CPU *cpu) {
    uint16_t zpg_addr = cpu_fetch_byte(cpu) + cpu->x;

    // Wrap within zero page
    uint16_t low = cpu_read_byte(cpu, (zpg_addr + 0) & 0x00FF);
    uint16_t high = cpu_read_byte(cpu, (zpg_addr + 1) & 0x00FF);
    cpu->addr = (high << 8) | low;

    return 0;
}

uint8_t cpu_idy(CPU *cpu) {
    uint16_t zpg_addr = cpu_fetch_byte(cpu);

    // Wrap within zero page
    uint16_t low = cpu_read_byte(cpu, (zpg_addr + 0) & 0x00FF);
    uint16_t high = cpu_read_byte(cpu, (zpg_addr + 1) & 0x00FF);
    uint16_t base_addr = (high << 8) | low;
    cpu->addr = base_addr + cpu->y;

    return is_page_crossed(base_addr, cpu->addr);
}

// ======================================================================================================
// INSTRUCTIONS
// ======================================================================================================

// NOTE: Functions return 1 if extra cycles may be needed, and 0 otherwise. Branching instructions
// handle extra cycles directly inside their implementation, so they just return 0.

uint8_t cpu_adc(CPU *cpu) {
    cpu_fetch_value(cpu);

    uint16_t result_word = (uint16_t)cpu->a + (uint16_t)cpu->fetched + (uint16_t)cpu_get_flag(cpu, FLAG_C);
    uint8_t result_byte = result_word & 0x00FF;

    bool same_add_msb = (get_bit(cpu->a, 7) ^ get_bit(cpu->fetched, 7)) == 0;
    bool diff_acc_msb = get_bit(cpu->a, 7) != get_bit(result_byte, 7);

    cpu->a = result_byte;

    cpu_set_flag(cpu, FLAG_C, result_word & 0xFF00);
    cpu_set_flag(cpu, FLAG_Z, result_byte == 0x00);
    cpu_set_flag(cpu, FLAG_V, same_add_msb && diff_acc_msb);
    cpu_set_flag(cpu, FLAG_N, is_bit_set(result_byte, 7));

    return 1;
}

uint8_t cpu_and(CPU *cpu) {
    cpu_fetch_value(cpu);

    cpu->a &= cpu->fetched;

    cpu_set_flag(cpu, FLAG_Z, cpu->a == 0x00);
    cpu_set_flag(cpu, FLAG_N, is_bit_set(cpu->a, 7));

    return 1;
}

uint8_t cpu_asl(CPU *cpu) {
    cpu_fetch_value(cpu);

    uint16_t result_word = (uint16_t)cpu->fetched << 1;
    uint8_t result_byte = result_word & 0x00FF;

    cpu_set_flag(cpu, FLAG_C, result_word & 0xFF00);
    cpu_set_flag(cpu, FLAG_Z, result_byte == 0x00);
    cpu_set_flag(cpu, FLAG_N, is_bit_set(result_byte, 7));

    if (cpu->table[cpu->opcode].address == &cpu_imp) {
        // Accumulator addressing mode
        cpu->a = result_byte;
    } else {
        // Other addressing modes
        cpu_write_byte(cpu, cpu->addr, result_byte);
    }

    return 0;
}

uint8_t cpu_bcc(CPU *cpu) {
    return cpu_branch_if(cpu, FLAG_C, false);
}

uint8_t cpu_bcs(CPU *cpu) {
    return cpu_branch_if(cpu, FLAG_C, true);
}

uint8_t cpu_beq(CPU *cpu) {
    return cpu_branch_if(cpu, FLAG_Z, true);
}

uint8_t cpu_bit(CPU *cpu) {
    cpu_fetch_value(cpu);

    cpu_set_flag(cpu, FLAG_Z, (cpu->a & cpu->fetched) == 0x00);
    cpu_set_flag(cpu, FLAG_V, is_bit_set(cpu->fetched, 6));
    cpu_set_flag(cpu, FLAG_N, is_bit_set(cpu->fetched, 7));

    return 0;
}

uint8_t cpu_bmi(CPU *cpu) {
    return cpu_branch_if(cpu, FLAG_N, true);
}

uint8_t cpu_bne(CPU *cpu) {
    return cpu_branch_if(cpu, FLAG_Z, false);
}

uint8_t cpu_bpl(CPU *cpu) {
    return cpu_branch_if(cpu, FLAG_N, false);
}

uint8_t cpu_brk(CPU *cpu) {
    // BRK skips the next byte
    cpu->pc++;

    cpu_set_flag(cpu, FLAG_B, true);

    cpu_stack_push_word(cpu, cpu->pc);
    cpu_stack_push_byte(cpu, cpu->status);

    cpu_set_flag(cpu, FLAG_B, false);
    cpu_set_flag(cpu, FLAG_I, true);

    cpu->pc = cpu_read_word(cpu, INTER_VECTOR);

    return 0;
}

uint8_t cpu_bvc(CPU *cpu) {
    return cpu_branch_if(cpu, FLAG_V, false);
}

uint8_t cpu_bvs(CPU *cpu) {
    return cpu_branch_if(cpu, FLAG_V, true);
}

uint8_t cpu_clc(CPU *cpu) {
    cpu_set_flag(cpu, FLAG_C, false);

    return 0;
}

uint8_t cpu_cld(CPU *cpu) {
    cpu_set_flag(cpu, FLAG_D, false);

    return 0;
}

uint8_t cpu_cli(CPU *cpu) {
    cpu_set_flag(cpu, FLAG_I, false);

    return 0;
}

uint8_t cpu_clv(CPU *cpu) {
    cpu_set_flag(cpu, FLAG_V, false);

    return 0;
}

uint8_t cpu_cmp(CPU *cpu) {
    cpu_fetch_value(cpu);

    int16_t result = (uint16_t)cpu->a - (uint16_t)cpu->fetched;

    cpu_set_flag(cpu, FLAG_C, result >= 0);
    cpu_set_flag(cpu, FLAG_Z, result == 0);
    cpu_set_flag(cpu, FLAG_N, is_bit_set(result, 7));

    return 1;
}

uint8_t cpu_cpx(CPU *cpu) {
    cpu_fetch_value(cpu);

    int16_t result = (uint16_t)cpu->x - (uint16_t)cpu->fetched;

    cpu_set_flag(cpu, FLAG_C, result >= 0);
    cpu_set_flag(cpu, FLAG_Z, result == 0);
    cpu_set_flag(cpu, FLAG_N, is_bit_set(result, 7));

    return 0;
}

uint8_t cpu_cpy(CPU *cpu) {
    cpu_fetch_value(cpu);

    int16_t result = (uint16_t)cpu->y - (uint16_t)cpu->fetched;

    cpu_set_flag(cpu, FLAG_C, result >= 0);
    cpu_set_flag(cpu, FLAG_Z, result == 0);
    cpu_set_flag(cpu, FLAG_N, is_bit_set(result, 7));

    return 0;
}

uint8_t cpu_dec(CPU *cpu) {
    cpu_fetch_value(cpu);

    uint8_t result = cpu->fetched - 1;

    cpu_write_byte(cpu, cpu->addr, result);

    cpu_set_flag(cpu, FLAG_Z, result == 0x00);
    cpu_set_flag(cpu, FLAG_N, is_bit_set(result, 7));

    return 0;
}

uint8_t cpu_dex(CPU *cpu) {
    cpu->x--;

    cpu_set_flag(cpu, FLAG_Z, cpu->x == 0x00);
    cpu_set_flag(cpu, FLAG_N, is_bit_set(cpu->x, 7));

    return 0;
}

uint8_t cpu_dey(CPU *cpu) {
    cpu->y--;

    cpu_set_flag(cpu, FLAG_Z, cpu->y == 0x00);
    cpu_set_flag(cpu, FLAG_N, is_bit_set(cpu->y, 7));

    return 0;
}

uint8_t cpu_eor(CPU *cpu) {
    cpu_fetch_value(cpu);

    cpu->a ^= cpu->fetched;

    cpu_set_flag(cpu, FLAG_Z, cpu->a == 0x00);
    cpu_set_flag(cpu, FLAG_N, is_bit_set(cpu->a, 7));

    return 1;
}

uint8_t cpu_inc(CPU *cpu) {
    cpu_fetch_value(cpu);

    uint8_t result = cpu->fetched + 1;

    cpu_write_byte(cpu, cpu->addr, result);

    cpu_set_flag(cpu, FLAG_Z, result == 0x00);
    cpu_set_flag(cpu, FLAG_N, is_bit_set(result, 7));

    return 0;
}

uint8_t cpu_inx(CPU *cpu) {
    cpu->x++;

    cpu_set_flag(cpu, FLAG_Z, cpu->x == 0x00);
    cpu_set_flag(cpu, FLAG_N, is_bit_set(cpu->x, 7));

    return 0;
}

uint8_t cpu_iny(CPU *cpu) {
    cpu->y++;

    cpu_set_flag(cpu, FLAG_Z, cpu->y == 0x00);
    cpu_set_flag(cpu, FLAG_N, is_bit_set(cpu->y, 7));

    return 0;
}

uint8_t cpu_jmp(CPU *cpu) {
    (void)cpu;
    return 0;
}

uint8_t cpu_jsr(CPU *cpu) {
    (void)cpu;
    return 0;
}

uint8_t cpu_lda(CPU *cpu) {
    (void)cpu;
    return 0;
}

uint8_t cpu_ldx(CPU *cpu) {
    (void)cpu;
    return 0;
}

uint8_t cpu_ldy(CPU *cpu) {
    (void)cpu;
    return 0;
}

uint8_t cpu_lsr(CPU *cpu) {
    (void)cpu;
    return 0;
}

uint8_t cpu_nop(CPU *cpu) {
    (void)cpu;
    return 0;
}

uint8_t cpu_ora(CPU *cpu) {
    (void)cpu;
    return 0;
}

uint8_t cpu_pha(CPU *cpu) {
    (void)cpu;
    return 0;
}

uint8_t cpu_php(CPU *cpu) {
    (void)cpu;
    return 0;
}

uint8_t cpu_pla(CPU *cpu) {
    (void)cpu;
    return 0;
}

uint8_t cpu_plp(CPU *cpu) {
    (void)cpu;
    return 0;
}

uint8_t cpu_rol(CPU *cpu) {
    (void)cpu;
    return 0;
}

uint8_t cpu_ror(CPU *cpu) {
    (void)cpu;
    return 0;
}

uint8_t cpu_rti(CPU *cpu) {
    (void)cpu;
    return 0;
}

uint8_t cpu_rts(CPU *cpu) {
    (void)cpu;
    return 0;
}

uint8_t cpu_sbc(CPU *cpu) {
    (void)cpu;
    return 0;
}

uint8_t cpu_sec(CPU *cpu) {
    (void)cpu;
    return 0;
}

uint8_t cpu_sed(CPU *cpu) {
    (void)cpu;
    return 0;
}

uint8_t cpu_sei(CPU *cpu) {
    (void)cpu;
    return 0;
}

uint8_t cpu_sta(CPU *cpu) {
    (void)cpu;
    return 0;
}

uint8_t cpu_stx(CPU *cpu) {
    (void)cpu;
    return 0;
}

uint8_t cpu_sty(CPU *cpu) {
    (void)cpu;
    return 0;
}

uint8_t cpu_tax(CPU *cpu) {
    (void)cpu;
    return 0;
}

uint8_t cpu_tay(CPU *cpu) {
    (void)cpu;
    return 0;
}

uint8_t cpu_tsx(CPU *cpu) {
    (void)cpu;
    return 0;
}

uint8_t cpu_txa(CPU *cpu) {
    (void)cpu;
    return 0;
}

uint8_t cpu_txs(CPU *cpu) {
    (void)cpu;
    return 0;
}

uint8_t cpu_tya(CPU *cpu) {
    (void)cpu;
    return 0;
}

uint8_t cpu_xxx(CPU *cpu) {
    (void)cpu;
    return 0;
}
