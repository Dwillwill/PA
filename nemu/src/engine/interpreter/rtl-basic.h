#ifndef __RTL_BASIC_H__
#define __RTL_BASIC_H__

#include "c_op.h"
#include <memory/vaddr.h>
#include <elf.h>
#include "../../isa/riscv32/local-include/reg.h"

//Get the symtab and strtab after analyse the elf in monitor.c
#ifdef CONFIG_FTRACE
extern char *strtab;
extern Elf32_Sym *elf_symtab;
extern int count;
#endif
/* RTL basic instructions */

#define def_rtl_compute_reg(name) \
  static inline def_rtl(name, rtlreg_t* dest, const rtlreg_t* src1, const rtlreg_t* src2) { \
    *dest = concat(c_, name) (*src1, *src2); \
  }

#define def_rtl_compute_imm(name) \
  static inline def_rtl(name ## i, rtlreg_t* dest, const rtlreg_t* src1, const sword_t imm) { \
    *dest = concat(c_, name) (*src1, imm); \
  }

#define def_rtl_compute_reg_imm(name) \
  def_rtl_compute_reg(name) \
  def_rtl_compute_imm(name) \

// compute

def_rtl_compute_reg_imm(add)
def_rtl_compute_reg_imm(sub)
def_rtl_compute_reg_imm(and)
def_rtl_compute_reg_imm(or)
def_rtl_compute_reg_imm(xor)
def_rtl_compute_reg_imm(sll)
def_rtl_compute_reg_imm(srl)
def_rtl_compute_reg_imm(sra)

#ifdef CONFIG_ISA64
def_rtl_compute_reg_imm(addw)
def_rtl_compute_reg_imm(subw)
def_rtl_compute_reg_imm(sllw)
def_rtl_compute_reg_imm(srlw)
def_rtl_compute_reg_imm(sraw)
#define rtl_addiw rtl_addwi
#define rtl_slliw rtl_sllwi
#define rtl_srliw rtl_srlwi
#define rtl_sraiw rtl_srawi
#endif

static inline def_rtl(setrelop, uint32_t relop, rtlreg_t *dest,
    const rtlreg_t *src1, const rtlreg_t *src2) {
  *dest = interpret_relop(relop, *src1, *src2);
}

static inline def_rtl(setrelopi, uint32_t relop, rtlreg_t *dest,
    const rtlreg_t *src1, sword_t imm) {
  *dest = interpret_relop(relop, *src1, imm);
}

// mul/div

def_rtl_compute_reg(mulu_lo)
def_rtl_compute_reg(mulu_hi)
def_rtl_compute_reg(muls_hi)
def_rtl_compute_reg(divu_q)
def_rtl_compute_reg(divu_r)
def_rtl_compute_reg(divs_q)
def_rtl_compute_reg(divs_r)

#ifdef CONFIG_ISA64
def_rtl_compute_reg(mulw)
def_rtl_compute_reg(divw)
def_rtl_compute_reg(divuw)
def_rtl_compute_reg(remw)
def_rtl_compute_reg(remuw)
#endif

static inline def_rtl(div64u_q, rtlreg_t* dest,
    const rtlreg_t* src1_hi, const rtlreg_t* src1_lo, const rtlreg_t* src2) {
  uint64_t dividend = ((uint64_t)(*src1_hi) << 32) | (*src1_lo);
  uint32_t divisor = (*src2);
  *dest = dividend / divisor;
}

static inline def_rtl(div64u_r, rtlreg_t* dest,
    const rtlreg_t* src1_hi, const rtlreg_t* src1_lo, const rtlreg_t* src2) {
  uint64_t dividend = ((uint64_t)(*src1_hi) << 32) | (*src1_lo);
  uint32_t divisor = (*src2);
  *dest = dividend % divisor;
}

static inline def_rtl(div64s_q, rtlreg_t* dest,
    const rtlreg_t* src1_hi, const rtlreg_t* src1_lo, const rtlreg_t* src2) {
  int64_t dividend = ((uint64_t)(*src1_hi) << 32) | (*src1_lo);
  int32_t divisor = (*src2);
  *dest = dividend / divisor;
}

static inline def_rtl(div64s_r, rtlreg_t* dest,
    const rtlreg_t* src1_hi, const rtlreg_t* src1_lo, const rtlreg_t* src2) {
  int64_t dividend = ((uint64_t)(*src1_hi) << 32) | (*src1_lo);
  int32_t divisor = (*src2);
  *dest = dividend % divisor;
}

// memory

static inline def_rtl(lm, rtlreg_t *dest, const rtlreg_t* addr, word_t offset, int len) {
  *dest = vaddr_read(*addr + offset, len);
}

static inline def_rtl(sm, const rtlreg_t *src1, const rtlreg_t* addr, word_t offset, int len) {
  vaddr_write(*addr + offset, len, *src1);
}

static inline def_rtl(sb, const rtlreg_t *src1, const rtlreg_t* addr, word_t offset, int len) {
  vaddr_write(*addr + offset, len, (uint8_t)*src1); return;  // len = 1 or 4 ?(0x??????aa or 0x000000aa)
}

static inline def_rtl(lms, rtlreg_t *dest, const rtlreg_t* addr, word_t offset, int len) {
  word_t val = vaddr_read(*addr + offset, len);
  switch (len) {
    case 4: *dest = (sword_t)(int32_t)val; return;
    case 1: *dest = (sword_t)( int8_t)val; return;
    case 2: *dest = (sword_t)(int16_t)val; return;
    IFDEF(CONFIG_ISA64, case 8: *dest = (sword_t)(int64_t)val; return);
    IFDEF(CONFIG_RT_CHECK, default: assert(0));
  }
}

static inline def_rtl(lmsu, rtlreg_t *dest, const rtlreg_t* addr, word_t offset, int len) {
  word_t val = vaddr_read(*addr + offset, len);
  switch (len) {
    case 4: *dest = (sword_t)(uint32_t)val; return;
    case 1: *dest = (sword_t)(uint8_t)val; return;
    case 2: *dest = (sword_t)(uint16_t)val; return;
    IFDEF(CONFIG_ISA64, case 8: *dest = (sword_t)(int64_t)val; return);
    IFDEF(CONFIG_RT_CHECK, default: assert(0));
  }
}

static inline def_rtl(host_lm, rtlreg_t* dest, const void *addr, int len) {
  switch (len) {
    case 4: *dest = *(uint32_t *)addr; return;
    case 1: *dest = *( uint8_t *)addr; return;
    case 2: *dest = *(uint16_t *)addr; return;
    IFDEF(CONFIG_ISA64, case 8: *dest = *(uint64_t *)addr; return);
    IFDEF(CONFIG_RT_CHECK, default: assert(0));
  }
}

static inline def_rtl(host_sm, void *addr, const rtlreg_t *src1, int len) {
  switch (len) {
    case 4: *(uint32_t *)addr = *src1; return;
    case 1: *( uint8_t *)addr = *src1; return;
    case 2: *(uint16_t *)addr = *src1; return;
    IFDEF(CONFIG_ISA64, case 8: *(uint64_t *)addr = *src1; return);
    IFDEF(CONFIG_RT_CHECK, default: assert(0));
  }
}

// control

static inline def_rtl(j, vaddr_t target) {
  s->dnpc = target;
}

static inline def_rtl(jr, rtlreg_t *target) {
  s->dnpc = *target;
}

static inline def_rtl(jrelop, uint32_t relop,
    const rtlreg_t *src1, const rtlreg_t *src2, vaddr_t target) {
  bool is_jmp = interpret_relop(relop, *src1, *src2);
  rtl_j(s, (is_jmp ? target : s->snpc));
}


static inline def_rtl(jalr, rtlreg_t* dest, const rtlreg_t *src1, const rtlreg_t imm) {
  
  rtl_addi(s, dest, &(s->pc), 4);
  sword_t offset = (sword_t)imm;
  s->dnpc = (*src1 + offset) & ~1;
  
  #ifdef CONFIG_FTRACE
  extern word_t zero_null;
  if(dest == &zero_null && src1 == &cpu.gpr[check_reg_idx(1)]._32){
    for(int j = 0; j < count; j++){
      if(elf_symtab[j].st_info == 18 && (cpu.pc >= elf_symtab[j].st_value && cpu.pc <= elf_symtab[j].st_value + elf_symtab[j].st_size - 4)){
        printf(FMT_PADDR ":ret [%s]\n", cpu.pc, strtab + elf_symtab[j].st_name);
      }
    }
  }else{
    for(int j = 0; j < count; j++){
      if(elf_symtab[j].st_info == 18 && s->dnpc == elf_symtab[j].st_value){
        printf(FMT_PADDR ":call [%s@" FMT_PADDR "]\n", cpu.pc, strtab + elf_symtab[j].st_name, elf_symtab[j].st_value);
      }
    }
  }
  #endif
}


static inline def_rtl(jal, rtlreg_t* dest, const rtlreg_t imm) {
  rtl_addi(s, dest, &(s->pc), 4);
  // printf("jal imm:%d\n", imm);
  s->dnpc = s->pc + ((sword_t)imm << 1);
  // s->dnpc = 0x90000000;
  #ifdef CONFIG_FTRACE
  for(int j = 0; j < count; j++){
    if(s->dnpc == elf_symtab[j].st_value){
        printf(FMT_PADDR ":call [%s@" FMT_PADDR "]\n", cpu.pc, strtab + elf_symtab[j].st_name, elf_symtab[j].st_value);
    }
  }
  #endif

}

static inline def_rtl(auipc, rtlreg_t *dest, const rtlreg_t imm) {
  rtl_addi(s, dest, &(s->pc), imm);
  // printf("after_addi:0x%08x\n", *dest);
}

static inline def_rtl(bne, const rtlreg_t *src1, const rtlreg_t *src2, const rtlreg_t imm) {
  rtl_jrelop(s, RELOP_NE, src1, src2, s->pc + ((sword_t)imm << 1));
  // printf(ASNI_FMT("pc = 0x%08x\n", ASNI_FG_GREEN), s->pc);
}

static inline def_rtl(beq, const rtlreg_t *src1, const rtlreg_t *src2, const rtlreg_t imm) {
  rtl_jrelop(s, RELOP_EQ, src1, src2, s->pc + ((sword_t)imm << 1));
  // printf(ASNI_FMT("pc = 0x%08x\n", ASNI_FG_GREEN), s->pc);
}

static inline def_rtl(bge, const rtlreg_t *src1, const rtlreg_t *src2, const rtlreg_t imm) {
  rtl_jrelop(s, RELOP_GE, src1, src2, s->pc + ((sword_t)imm << 1));
  // printf(ASNI_FMT("pc = 0x%08x\n", ASNI_FG_GREEN), s->pc);
}

static inline def_rtl(blt, const rtlreg_t *src1, const rtlreg_t *src2, const rtlreg_t imm) {
  rtl_jrelop(s, RELOP_LT, src1, src2, s->pc + ((sword_t)imm << 1));
  // printf(ASNI_FMT("pc = 0x%08x\n", ASNI_FG_GREEN), s->pc);
}

static inline def_rtl(bltu, const rtlreg_t *src1, const rtlreg_t *src2, const rtlreg_t imm) {
  rtl_jrelop(s, RELOP_LTU, src1, src2, s->pc + ((sword_t)imm << 1));
  // printf(ASNI_FMT("pc = 0x%08x\n", ASNI_FG_GREEN), s->pc);
}

static inline def_rtl(bgeu, const rtlreg_t *src1, const rtlreg_t *src2, const rtlreg_t imm) {
  rtl_jrelop(s, RELOP_GEU, src1, src2, s->pc + ((sword_t)imm << 1));
  // printf(ASNI_FMT("pc = 0x%08x\n", ASNI_FG_GREEN), s->pc);
}
#endif
