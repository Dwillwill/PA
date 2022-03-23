def_EHelper(lui) {
  rtl_li(s, ddest, id_src1->imm);
}

def_EHelper(addi){
  rtl_addi(s, ddest, dsrc1, id_src2->simm);
  printf("after_addi:0x%08x\n", *ddest);
}

def_EHelper(auipc){
  rtl_auipc(s, ddest, id_src1->simm);
}

def_EHelper(jal){
  rtl_jal(s, ddest, id_src1->simm);
}

def_EHelper(jalr){
  rtl_jalr(s, ddest, dsrc1, id_src2->simm);
}


