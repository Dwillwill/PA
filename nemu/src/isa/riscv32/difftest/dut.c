#include <isa.h>
#include <cpu/difftest.h>
#include "../local-include/reg.h"

bool isa_difftest_checkregs(CPU_state *ref_r, vaddr_t pc) {
  bool flag = true;
  // printf("ref_pc:"FMT_PADDR "\n", ref_r->pc);
  // printf("duc_dpc:"FMT_PADDR "\n", cpu.pc);
  // printf("fun pc:"FMT_PADDR "\n", pc);
  if(ref_r->pc != cpu.pc){
    // printf("flag:%d\n", flag);
    return false;
  }
  for(int i = 0; i < 32; i++){
    // printf("ref_r[%d]:" FMT_WORD "\n", i, ref_r->gpr[i]._32);
    // printf("duc_r[%d]:" FMT_WORD "\n", i, cpu.gpr[i]._32);
    if(ref_r->gpr[i]._32 != cpu.gpr[i]._32){
      flag = false;
      break;
    }
  }
  // printf("flag:%d\n", flag);
  return flag;
}

void isa_difftest_attach() {
}
