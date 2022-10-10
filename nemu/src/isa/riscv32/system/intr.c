#include <isa.h>

enum {MSTATUS = 0, MTVEC, MEPC, MCAUSE};
word_t isa_raise_intr(word_t NO, vaddr_t epc) {
  /* TODO: Trigger an interrupt/exception with ``NO''.
   * Then return the address of the interrupt/exception vector.
   */
  cpu.csr[MEPC]._32 = epc;
  cpu.csr[MCAUSE]._32 = NO;
  #ifdef CONFIG_ETRACE
  printf(ASNI_FMT("MEPC: 0x%08x\n", ASNI_FG_GREEN), cpu.csr[MEPC]._32);
  printf(ASNI_FMT("MCAUSE: 0x%08x\n", ASNI_FG_GREEN), cpu.csr[MAUSE]._32);
  printf(ASNI_FMT("MTVEC: 0x%08x\n", ASNI_FG_GREEN), cpu.csr[MTVEC]._32);
  #endif
  return cpu.csr[MTVEC]._32;
}

word_t isa_query_intr() {
  return INTR_EMPTY;
}
