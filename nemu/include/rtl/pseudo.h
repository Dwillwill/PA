#ifndef __RTL_PSEUDO_H__
#define __RTL_PSEUDO_H__

#ifndef __RTL_RTL_H__
#error "Should be only included by <rtl/rtl.h>"
#endif

/* RTL pseudo instructions */

static inline def_rtl(auipc, rtlreg_t *dest, const rtlreg_t imm) {
  rtl_addi(s, dest, &(s->pc), imm);
  printf("after_addi:0x%08x\n", *dest);
}

static inline def_rtl(mv, rtlreg_t* dest, const rtlreg_t *src1) {
  rtl_addi(s, dest, src1, 0);
}

static inline def_rtl(jal, rtlreg_t* dest, const rtlreg_t imm) {
  rtl_addi(s, dest, &(s->pc), 4);
  printf("jal imm:%d\n", imm);
  s->dnpc = s->pc + ((sword_t)imm << 1);
  printf("pc = 0x%08x\n", s->pc);
}


static inline void rtl_li(Decode *s, rtlreg_t* dest, const rtlreg_t imm) {
  rtl_addi(s, dest, (&rzero), imm);
}

static inline def_rtl(jalr, rtlreg_t* dest, const rtlreg_t *src1, const rtlreg_t imm) {
  rtl_addi(s, dest, &(s->pc), 4);
  sword_t offset = (sword_t)imm;
  s->dnpc = (*src1 + offset) & ~1;
  printf("pc = 0x%08x\n", s->pc);
}

static inline def_rtl(not, rtlreg_t *dest, const rtlreg_t* src1) {
  // dest <- ~src1
  TODO();
}

static inline def_rtl(neg, rtlreg_t *dest, const rtlreg_t* src1) {
  // dest <- -src1
  TODO();
}

static inline def_rtl(sext, rtlreg_t* dest, const rtlreg_t* src1, int width) {
  // dest <- signext(src1[(width * 8 - 1) .. 0])
  TODO();
}

static inline def_rtl(zext, rtlreg_t* dest, const rtlreg_t* src1, int width) {
  // dest <- zeroext(src1[(width * 8 - 1) .. 0])
  TODO();
}

static inline def_rtl(msb, rtlreg_t* dest, const rtlreg_t* src1, int width) {
  // dest <- src1[width * 8 - 1]
  TODO();
}
#endif
