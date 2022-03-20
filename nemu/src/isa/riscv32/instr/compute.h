def_EHelper(lui) {
  rtl_li(s, ddest, id_src1->imm);
}

def_EHelper(li){
  rtl_li(s, ddest, id_src1->simm);
}

def_EHelper(auipc){
  rtl_auipc(s, ddest, id_src1->simm);
}

