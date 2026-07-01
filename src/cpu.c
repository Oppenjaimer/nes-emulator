#include "cpu.h"

// Forward declarations
uint8_t bus_read(const Bus *bus, uint16_t addr);
void bus_write(Bus *bus, uint16_t addr, uint8_t value);

// ======================================================================================================
// CORE
// ======================================================================================================

void cpu_init_table(CPU *cpu) {
    // Initialize instruction table with illegal opcodes
    for (int i = 0; i < TABLE_SIZE; i++) {
        cpu->table[i] = (Instruction){"???", &cpu_imp, &cpu_xxx, 2};
    }

    // Set legal opcodes
    cpu->table[ADC_ABS] = (Instruction){"ADC_ABS", &cpu_abs, &cpu_adc, 4};
    cpu->table[ADC_ABX] = (Instruction){"ADC_ABX", &cpu_abx, &cpu_adc, 4};
    cpu->table[ADC_ABY] = (Instruction){"ADC_ABY", &cpu_aby, &cpu_adc, 4};
    cpu->table[ADC_IDX] = (Instruction){"ADC_IDX", &cpu_idx, &cpu_adc, 6};
    cpu->table[ADC_IDY] = (Instruction){"ADC_IDY", &cpu_idy, &cpu_adc, 5};
    cpu->table[ADC_IMM] = (Instruction){"ADC_IMM", &cpu_imm, &cpu_adc, 2};
    cpu->table[ADC_ZPG] = (Instruction){"ADC_ZPG", &cpu_zpg, &cpu_adc, 3};
    cpu->table[ADC_ZPX] = (Instruction){"ADC_ZPX", &cpu_zpx, &cpu_adc, 4};

    cpu->table[AND_ABS] = (Instruction){"AND_ABS", &cpu_abs, &cpu_and, 4};
    cpu->table[AND_ABX] = (Instruction){"AND_ABX", &cpu_abx, &cpu_and, 4};
    cpu->table[AND_ABY] = (Instruction){"AND_ABY", &cpu_aby, &cpu_and, 4};
    cpu->table[AND_IDX] = (Instruction){"AND_IDX", &cpu_idx, &cpu_and, 6};
    cpu->table[AND_IDY] = (Instruction){"AND_IDY", &cpu_idy, &cpu_and, 5};
    cpu->table[AND_IMM] = (Instruction){"AND_IMM", &cpu_imm, &cpu_and, 2};
    cpu->table[AND_ZPG] = (Instruction){"AND_ZPG", &cpu_zpg, &cpu_and, 3};
    cpu->table[AND_ZPX] = (Instruction){"AND_ZPX", &cpu_zpx, &cpu_and, 4};

    cpu->table[ASL_ABS] = (Instruction){"ASL_ABS", &cpu_abs, &cpu_asl, 6};
    cpu->table[ASL_ABX] = (Instruction){"ASL_ABX", &cpu_abx, &cpu_asl, 7};
    cpu->table[ASL_IMP] = (Instruction){"ASL_IMP", &cpu_imp, &cpu_asl, 2};
    cpu->table[ASL_ZPG] = (Instruction){"ASL_ZPG", &cpu_zpg, &cpu_asl, 5};
    cpu->table[ASL_ZPX] = (Instruction){"ASL_ZPX", &cpu_zpx, &cpu_asl, 6};

    cpu->table[BCC_REL] = (Instruction){"BCC_REL", &cpu_rel, &cpu_bcc, 2};
    cpu->table[BCS_REL] = (Instruction){"BCS_REL", &cpu_rel, &cpu_bcs, 2};
    cpu->table[BEQ_REL] = (Instruction){"BEQ_REL", &cpu_rel, &cpu_beq, 2};

    cpu->table[BIT_ABS] = (Instruction){"BIT_ABS", &cpu_abs, &cpu_bit, 4};
    cpu->table[BIT_ZPG] = (Instruction){"BIT_ZPG", &cpu_zpg, &cpu_bit, 3};

    cpu->table[BMI_REL] = (Instruction){"BMI_REL", &cpu_rel, &cpu_bmi, 2};
    cpu->table[BNE_REL] = (Instruction){"BNE_REL", &cpu_rel, &cpu_bne, 2};
    cpu->table[BPL_REL] = (Instruction){"BPL_REL", &cpu_rel, &cpu_bpl, 2};

    cpu->table[BRK_IMP] = (Instruction){"BRK_IMP", &cpu_imp, &cpu_brk, 7};

    cpu->table[BVC_REL] = (Instruction){"BVC_REL", &cpu_rel, &cpu_bvc, 2};
    cpu->table[BVS_REL] = (Instruction){"BVS_REL", &cpu_rel, &cpu_bvs, 2};

    cpu->table[CLC_IMP] = (Instruction){"CLC_IMP", &cpu_imp, &cpu_clc, 2};
    cpu->table[CLD_IMP] = (Instruction){"CLD_IMP", &cpu_imp, &cpu_cld, 2};
    cpu->table[CLI_IMP] = (Instruction){"CLI_IMP", &cpu_imp, &cpu_cli, 2};
    cpu->table[CLV_IMP] = (Instruction){"CLV_IMP", &cpu_imp, &cpu_clv, 2};

    cpu->table[CMP_ABS] = (Instruction){"CMP_ABS", &cpu_abs, &cpu_cmp, 4};
    cpu->table[CMP_ABX] = (Instruction){"CMP_ABX", &cpu_abx, &cpu_cmp, 4};
    cpu->table[CMP_ABY] = (Instruction){"CMP_ABY", &cpu_aby, &cpu_cmp, 4};
    cpu->table[CMP_IDX] = (Instruction){"CMP_IDX", &cpu_idx, &cpu_cmp, 6};
    cpu->table[CMP_IDY] = (Instruction){"CMP_IDY", &cpu_idy, &cpu_cmp, 5};
    cpu->table[CMP_IMM] = (Instruction){"CMP_IMM", &cpu_imm, &cpu_cmp, 2};
    cpu->table[CMP_ZPG] = (Instruction){"CMP_ZPG", &cpu_zpg, &cpu_cmp, 3};
    cpu->table[CMP_ZPX] = (Instruction){"CMP_ZPX", &cpu_zpx, &cpu_cmp, 4};

    cpu->table[CPX_ABS] = (Instruction){"CPX_ABS", &cpu_abs, &cpu_cpx, 4};
    cpu->table[CPX_IMM] = (Instruction){"CPX_IMM", &cpu_imm, &cpu_cpx, 2};
    cpu->table[CPX_ZPG] = (Instruction){"CPX_ZPG", &cpu_zpg, &cpu_cpx, 3};

    cpu->table[CPY_ABS] = (Instruction){"CPY_ABS", &cpu_abs, &cpu_cpy, 4};
    cpu->table[CPY_IMM] = (Instruction){"CPY_IMM", &cpu_imm, &cpu_cpy, 2};
    cpu->table[CPY_ZPG] = (Instruction){"CPY_ZPG", &cpu_zpg, &cpu_cpy, 3};

    cpu->table[DEC_ABS] = (Instruction){"DEC_ABS", &cpu_abs, &cpu_dec, 6};
    cpu->table[DEC_ABX] = (Instruction){"DEC_ABX", &cpu_abx, &cpu_dec, 7};
    cpu->table[DEC_ZPG] = (Instruction){"DEC_ZPG", &cpu_zpg, &cpu_dec, 5};
    cpu->table[DEC_ZPX] = (Instruction){"DEC_ZPX", &cpu_zpx, &cpu_dec, 6};

    cpu->table[DEX_IMP] = (Instruction){"DEX_IMP", &cpu_imp, &cpu_dex, 2};
    cpu->table[DEY_IMP] = (Instruction){"DEY_IMP", &cpu_imp, &cpu_dey, 2};

    cpu->table[EOR_ABS] = (Instruction){"EOR_ABS", &cpu_abs, &cpu_eor, 4};
    cpu->table[EOR_ABX] = (Instruction){"EOR_ABX", &cpu_abx, &cpu_eor, 4};
    cpu->table[EOR_ABY] = (Instruction){"EOR_ABY", &cpu_aby, &cpu_eor, 4};
    cpu->table[EOR_IDX] = (Instruction){"EOR_IDX", &cpu_idx, &cpu_eor, 6};
    cpu->table[EOR_IDY] = (Instruction){"EOR_IDY", &cpu_idy, &cpu_eor, 5};
    cpu->table[EOR_IMM] = (Instruction){"EOR_IMM", &cpu_imm, &cpu_eor, 2};
    cpu->table[EOR_ZPG] = (Instruction){"EOR_ZPG", &cpu_zpg, &cpu_eor, 3};
    cpu->table[EOR_ZPX] = (Instruction){"EOR_ZPX", &cpu_zpx, &cpu_eor, 4};

    cpu->table[INC_ABS] = (Instruction){"INC_ABS", &cpu_abs, &cpu_inc, 6};
    cpu->table[INC_ABX] = (Instruction){"INC_ABX", &cpu_abx, &cpu_inc, 7};
    cpu->table[INC_ZPG] = (Instruction){"INC_ZPG", &cpu_zpg, &cpu_inc, 5};
    cpu->table[INC_ZPX] = (Instruction){"INC_ZPX", &cpu_zpx, &cpu_inc, 6};

    cpu->table[INX_IMP] = (Instruction){"INX_IMP", &cpu_imp, &cpu_inx, 2};
    cpu->table[INY_IMP] = (Instruction){"INY_IMP", &cpu_imp, &cpu_iny, 2};

    cpu->table[JMP_ABS] = (Instruction){"JMP_ABS", &cpu_abs, &cpu_jmp, 3};
    cpu->table[JMP_IND] = (Instruction){"JMP_IND", &cpu_ind, &cpu_jmp, 5};

    cpu->table[JSR_ABS] = (Instruction){"JSR_ABS", &cpu_abs, &cpu_jsr, 6};

    cpu->table[LDA_ABS] = (Instruction){"LDA_ABS", &cpu_abs, &cpu_lda, 4};
    cpu->table[LDA_ABX] = (Instruction){"LDA_ABX", &cpu_abx, &cpu_lda, 4};
    cpu->table[LDA_ABY] = (Instruction){"LDA_ABY", &cpu_aby, &cpu_lda, 4};
    cpu->table[LDA_IDX] = (Instruction){"LDA_IDX", &cpu_idx, &cpu_lda, 6};
    cpu->table[LDA_IDY] = (Instruction){"LDA_IDY", &cpu_idy, &cpu_lda, 5};
    cpu->table[LDA_IMM] = (Instruction){"LDA_IMM", &cpu_imm, &cpu_lda, 2};
    cpu->table[LDA_ZPG] = (Instruction){"LDA_ZPG", &cpu_zpg, &cpu_lda, 3};
    cpu->table[LDA_ZPX] = (Instruction){"LDA_ZPX", &cpu_zpx, &cpu_lda, 4};

    cpu->table[LDX_ABS] = (Instruction){"LDX_ABS", &cpu_abs, &cpu_ldx, 4};
    cpu->table[LDX_ABY] = (Instruction){"LDX_ABY", &cpu_aby, &cpu_ldx, 4};
    cpu->table[LDX_IMM] = (Instruction){"LDX_IMM", &cpu_imm, &cpu_ldx, 2};
    cpu->table[LDX_ZPG] = (Instruction){"LDX_ZPG", &cpu_zpg, &cpu_ldx, 3};
    cpu->table[LDX_ZPY] = (Instruction){"LDX_ZPY", &cpu_zpy, &cpu_ldx, 4};

    cpu->table[LDY_ABS] = (Instruction){"LDY_ABS", &cpu_abs, &cpu_ldy, 4};
    cpu->table[LDY_ABX] = (Instruction){"LDY_ABX", &cpu_abx, &cpu_ldy, 4};
    cpu->table[LDY_IMM] = (Instruction){"LDY_IMM", &cpu_imm, &cpu_ldy, 2};
    cpu->table[LDY_ZPG] = (Instruction){"LDY_ZPG", &cpu_zpg, &cpu_ldy, 3};
    cpu->table[LDY_ZPX] = (Instruction){"LDY_ZPX", &cpu_zpx, &cpu_ldy, 4};

    cpu->table[LSR_ABS] = (Instruction){"LSR_ABS", &cpu_abs, &cpu_lsr, 6};
    cpu->table[LSR_ABX] = (Instruction){"LSR_ABX", &cpu_abx, &cpu_lsr, 7};
    cpu->table[LSR_IMP] = (Instruction){"LSR_IMP", &cpu_imp, &cpu_lsr, 2};
    cpu->table[LSR_ZPG] = (Instruction){"LSR_ZPG", &cpu_zpg, &cpu_lsr, 5};
    cpu->table[LSR_ZPX] = (Instruction){"LSR_ZPX", &cpu_zpx, &cpu_lsr, 6};

    cpu->table[NOP_IMP] = (Instruction){"NOP_IMP", &cpu_imp, &cpu_nop, 2};

    cpu->table[ORA_ABS] = (Instruction){"ORA_ABS", &cpu_abs, &cpu_ora, 4};
    cpu->table[ORA_ABX] = (Instruction){"ORA_ABX", &cpu_abx, &cpu_ora, 4};
    cpu->table[ORA_ABY] = (Instruction){"ORA_ABY", &cpu_aby, &cpu_ora, 4};
    cpu->table[ORA_IDX] = (Instruction){"ORA_IDX", &cpu_idx, &cpu_ora, 6};
    cpu->table[ORA_IDY] = (Instruction){"ORA_IDY", &cpu_idy, &cpu_ora, 5};
    cpu->table[ORA_IMM] = (Instruction){"ORA_IMM", &cpu_imm, &cpu_ora, 2};
    cpu->table[ORA_ZPG] = (Instruction){"ORA_ZPG", &cpu_zpg, &cpu_ora, 3};
    cpu->table[ORA_ZPX] = (Instruction){"ORA_ZPX", &cpu_zpx, &cpu_ora, 4};

    cpu->table[PHA_IMP] = (Instruction){"PHA_IMP", &cpu_imp, &cpu_pha, 3};
    cpu->table[PHP_IMP] = (Instruction){"PHP_IMP", &cpu_imp, &cpu_php, 3};
    cpu->table[PLA_IMP] = (Instruction){"PLA_IMP", &cpu_imp, &cpu_pla, 4};
    cpu->table[PLP_IMP] = (Instruction){"PLP_IMP", &cpu_imp, &cpu_plp, 4};

    cpu->table[ROL_ABS] = (Instruction){"ROL_ABS", &cpu_abs, &cpu_rol, 6};
    cpu->table[ROL_ABX] = (Instruction){"ROL_ABX", &cpu_abx, &cpu_rol, 7};
    cpu->table[ROL_IMP] = (Instruction){"ROL_IMP", &cpu_imp, &cpu_rol, 2};
    cpu->table[ROL_ZPG] = (Instruction){"ROL_ZPG", &cpu_zpg, &cpu_rol, 5};
    cpu->table[ROL_ZPX] = (Instruction){"ROL_ZPX", &cpu_zpx, &cpu_rol, 6};

    cpu->table[ROR_ABS] = (Instruction){"ROR_ABS", &cpu_abs, &cpu_ror, 6};
    cpu->table[ROR_ABX] = (Instruction){"ROR_ABX", &cpu_abx, &cpu_ror, 7};
    cpu->table[ROR_IMP] = (Instruction){"ROR_IMP", &cpu_imp, &cpu_ror, 2};
    cpu->table[ROR_ZPG] = (Instruction){"ROR_ZPG", &cpu_zpg, &cpu_ror, 5};
    cpu->table[ROR_ZPX] = (Instruction){"ROR_ZPX", &cpu_zpx, &cpu_ror, 6};

    cpu->table[RTI_IMP] = (Instruction){"RTI_IMP", &cpu_imp, &cpu_rti, 6};
    cpu->table[RTS_IMP] = (Instruction){"RTS_IMP", &cpu_imp, &cpu_rts, 6};

    cpu->table[SBC_ABS] = (Instruction){"SBC_ABS", &cpu_abs, &cpu_sbc, 4};
    cpu->table[SBC_ABX] = (Instruction){"SBC_ABX", &cpu_abx, &cpu_sbc, 4};
    cpu->table[SBC_ABY] = (Instruction){"SBC_ABY", &cpu_aby, &cpu_sbc, 4};
    cpu->table[SBC_IDX] = (Instruction){"SBC_IDX", &cpu_idx, &cpu_sbc, 6};
    cpu->table[SBC_IDY] = (Instruction){"SBC_IDY", &cpu_idy, &cpu_sbc, 5};
    cpu->table[SBC_IMM] = (Instruction){"SBC_IMM", &cpu_imm, &cpu_sbc, 2};
    cpu->table[SBC_ZPG] = (Instruction){"SBC_ZPG", &cpu_zpg, &cpu_sbc, 3};
    cpu->table[SBC_ZPX] = (Instruction){"SBC_ZPX", &cpu_zpx, &cpu_sbc, 4};

    cpu->table[SEC_IMP] = (Instruction){"SEC_IMP", &cpu_imp, &cpu_sec, 2};
    cpu->table[SED_IMP] = (Instruction){"SED_IMP", &cpu_imp, &cpu_sed, 2};
    cpu->table[SEI_IMP] = (Instruction){"SEI_IMP", &cpu_imp, &cpu_sei, 2};

    cpu->table[STA_ABS] = (Instruction){"STA_ABS", &cpu_abs, &cpu_sta, 4};
    cpu->table[STA_ABX] = (Instruction){"STA_ABX", &cpu_abx, &cpu_sta, 5};
    cpu->table[STA_ABY] = (Instruction){"STA_ABY", &cpu_aby, &cpu_sta, 5};
    cpu->table[STA_IDX] = (Instruction){"STA_IDX", &cpu_idx, &cpu_sta, 6};
    cpu->table[STA_IDY] = (Instruction){"STA_IDY", &cpu_idy, &cpu_sta, 6};
    cpu->table[STA_ZPG] = (Instruction){"STA_ZPG", &cpu_zpg, &cpu_sta, 3};
    cpu->table[STA_ZPX] = (Instruction){"STA_ZPX", &cpu_zpx, &cpu_sta, 4};

    cpu->table[STX_ABS] = (Instruction){"STX_ABS", &cpu_abs, &cpu_stx, 4};
    cpu->table[STX_ZPG] = (Instruction){"STX_ZPG", &cpu_zpg, &cpu_stx, 3};
    cpu->table[STX_ZPY] = (Instruction){"STX_ZPY", &cpu_zpy, &cpu_stx, 4};

    cpu->table[STY_ABS] = (Instruction){"STY_ABS", &cpu_abs, &cpu_sty, 4};
    cpu->table[STY_ZPG] = (Instruction){"STY_ZPG", &cpu_zpg, &cpu_sty, 3};
    cpu->table[STY_ZPX] = (Instruction){"STY_ZPX", &cpu_zpx, &cpu_sty, 4};

    cpu->table[TAX_IMP] = (Instruction){"TAX_IMP", &cpu_imp, &cpu_tax, 2};
    cpu->table[TAY_IMP] = (Instruction){"TAY_IMP", &cpu_imp, &cpu_tay, 2};

    cpu->table[TSX_IMP] = (Instruction){"TSX_IMP", &cpu_imp, &cpu_tsx, 2};
    cpu->table[TXA_IMP] = (Instruction){"TXA_IMP", &cpu_imp, &cpu_txa, 2};
    cpu->table[TXS_IMP] = (Instruction){"TXS_IMP", &cpu_imp, &cpu_txs, 2};
    cpu->table[TYA_IMP] = (Instruction){"TYA_IMP", &cpu_imp, &cpu_tya, 2};
}

void cpu_connect_bus(CPU *cpu, Bus *bus) {
    cpu->bus = bus;
}

uint8_t cpu_read(const CPU *cpu, uint16_t addr) {
    return bus_read(cpu->bus, addr);
}

void cpu_write(CPU *cpu, uint16_t addr, uint8_t value) {
    bus_write(cpu->bus, addr, value);
}

uint8_t cpu_fetch(CPU *cpu, uint16_t addr) {
    uint8_t byte = cpu_read(cpu, addr);
    cpu->pc++;

    return byte;
}

uint8_t cpu_get_flag(const CPU *cpu, Flag flag) {
    return (cpu->status & flag);
}

void cpu_set_flag(CPU *cpu, Flag flag, bool value) {
    if (value) cpu->status |= flag;
    else cpu->status &= ~flag;
}

// ======================================================================================================
// SIGNALS
// ======================================================================================================

void cpu_clock(CPU *cpu) {
    if (cpu->cycles == 0) {
        uint8_t opcode = cpu_fetch(cpu, cpu->pc);
        Instruction instruction = cpu->table[opcode];

        cpu->cycles = instruction.cycles;

        uint8_t extra_cycle1 = cpu->table[opcode].address(cpu);
        uint8_t extra_cycle2 = cpu->table[opcode].execute(cpu);

        cpu->cycles += (extra_cycle1 & extra_cycle2);
    }

    if (cpu->cycles > 0) cpu->cycles--;
}

void cpu_reset(CPU *cpu) {
    (void)cpu;
}

void cpu_irq(CPU *cpu) {
    (void)cpu;
}

void cpu_nmi(CPU *cpu) {
    (void)cpu;
}

// ======================================================================================================
// ADDRESSING MODES
// ======================================================================================================

uint8_t cpu_imp(CPU *cpu) {
    (void)cpu;
    return 0;
}

uint8_t cpu_imm(CPU *cpu) {
    (void)cpu;
    return 0;
}

uint8_t cpu_zpg(CPU *cpu) {
    (void)cpu;
    return 0;
}

uint8_t cpu_zpx(CPU *cpu) {
    (void)cpu;
    return 0;
}

uint8_t cpu_zpy(CPU *cpu) {
    (void)cpu;
    return 0;
}

uint8_t cpu_rel(CPU *cpu) {
    (void)cpu;
    return 0;
}

uint8_t cpu_abs(CPU *cpu) {
    (void)cpu;
    return 0;
}

uint8_t cpu_abx(CPU *cpu) {
    (void)cpu;
    return 0;
}

uint8_t cpu_aby(CPU *cpu) {
    (void)cpu;
    return 0;
}

uint8_t cpu_ind(CPU *cpu) {
    (void)cpu;
    return 0;
}

uint8_t cpu_idx(CPU *cpu) {
    (void)cpu;
    return 0;
}

uint8_t cpu_idy(CPU *cpu) {
    (void)cpu;
    return 0;
}

// ======================================================================================================
// OPCODES
// ======================================================================================================

uint8_t cpu_adc(CPU *cpu) {
    (void)cpu;
    return 0;
}

uint8_t cpu_and(CPU *cpu) {
    (void)cpu;
    return 0;
}

uint8_t cpu_asl(CPU *cpu) {
    (void)cpu;
    return 0;
}

uint8_t cpu_bcc(CPU *cpu) {
    (void)cpu;
    return 0;
}

uint8_t cpu_bcs(CPU *cpu) {
    (void)cpu;
    return 0;
}

uint8_t cpu_beq(CPU *cpu) {
    (void)cpu;
    return 0;
}

uint8_t cpu_bit(CPU *cpu) {
    (void)cpu;
    return 0;
}

uint8_t cpu_bmi(CPU *cpu) {
    (void)cpu;
    return 0;
}

uint8_t cpu_bne(CPU *cpu) {
    (void)cpu;
    return 0;
}

uint8_t cpu_bpl(CPU *cpu) {
    (void)cpu;
    return 0;
}

uint8_t cpu_brk(CPU *cpu) {
    (void)cpu;
    return 0;
}

uint8_t cpu_bvc(CPU *cpu) {
    (void)cpu;
    return 0;
}

uint8_t cpu_bvs(CPU *cpu) {
    (void)cpu;
    return 0;
}

uint8_t cpu_clc(CPU *cpu) {
    (void)cpu;
    return 0;
}

uint8_t cpu_cld(CPU *cpu) {
    (void)cpu;
    return 0;
}

uint8_t cpu_cli(CPU *cpu) {
    (void)cpu;
    return 0;
}

uint8_t cpu_clv(CPU *cpu) {
    (void)cpu;
    return 0;
}

uint8_t cpu_cmp(CPU *cpu) {
    (void)cpu;
    return 0;
}

uint8_t cpu_cpx(CPU *cpu) {
    (void)cpu;
    return 0;
}

uint8_t cpu_cpy(CPU *cpu) {
    (void)cpu;
    return 0;
}

uint8_t cpu_dec(CPU *cpu) {
    (void)cpu;
    return 0;
}

uint8_t cpu_dex(CPU *cpu) {
    (void)cpu;
    return 0;
}

uint8_t cpu_dey(CPU *cpu) {
    (void)cpu;
    return 0;
}

uint8_t cpu_eor(CPU *cpu) {
    (void)cpu;
    return 0;
}

uint8_t cpu_inc(CPU *cpu) {
    (void)cpu;
    return 0;
}

uint8_t cpu_inx(CPU *cpu) {
    (void)cpu;
    return 0;
}

uint8_t cpu_iny(CPU *cpu) {
    (void)cpu;
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
