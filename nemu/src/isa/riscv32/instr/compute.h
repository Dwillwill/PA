// U-Type
def_EHelper(lui) {
  rtl_li(s, ddest, id_src1->imm);
}

def_EHelper(auipc){
  rtl_auipc(s, ddest, id_src1->simm);
}

// J-Type
def_EHelper(jal){
  rtl_jal(s, ddest, id_src1->simm);
}

// I-Type
def_EHelper(jalr){
  rtl_jalr(s, ddest, dsrc1, id_src2->simm);
}

def_EHelper(addi){
  rtl_addi(s, ddest, dsrc1, id_src2->simm);
  // printf("after_addi:0x%08x\n", *ddest);
}

def_EHelper(xori){
  rtl_xori(s, ddest, dsrc1, id_src2->simm);
}

def_EHelper(ori){
  rtl_ori(s, ddest, dsrc1, id_src2->simm);
}

def_EHelper(andi){
  rtl_andi(s, ddest, dsrc1, id_src2->simm);
}

def_EHelper(srai){
  rtl_srai(s, ddest, dsrc1, id_src2->simm);
}

def_EHelper(slli){
  rtl_slli(s, ddest, dsrc1, id_src2->simm);
}

def_EHelper(srli){
  rtl_srli(s, ddest, dsrc1, id_src2->simm);
}

def_EHelper(sltiu){
  *ddest = interpret_relop(RELOP_LTU, *dsrc1, id_src2->simm) ? 1 : 0;
}

def_EHelper(slti){
  *ddest = interpret_relop(RELOP_LT, *dsrc1, id_src2->simm) ? 1 : 0;
}

// R-Type
def_EHelper(add){
  rtl_add(s, ddest, dsrc1, dsrc2);
}

def_EHelper(sub){
  rtl_sub(s, ddest, dsrc1, dsrc2);
}

def_EHelper(mul){
  rtl_mulu_lo(s, ddest, dsrc1, dsrc2);
}

def_EHelper(mulh){
  rtl_muls_hi(s, ddest, dsrc1, dsrc2);
}

def_EHelper(mulhu){
  rtl_mulu_hi(s, ddest, dsrc1, dsrc2);
}

def_EHelper(div){
  rtl_divs_q(s, ddest, dsrc1, dsrc2);
}

def_EHelper(divu){
  rtl_divu_q(s, ddest, dsrc1, dsrc2);
}

def_EHelper(rem){
  rtl_divs_r(s, ddest, dsrc1, dsrc2);
}

def_EHelper(remu){
  rtl_divu_r(s, ddest, dsrc1, dsrc2);
}

def_EHelper(xor){
  rtl_xor(s, ddest, dsrc1, dsrc2);
}

def_EHelper(or){
  rtl_or(s, ddest, dsrc1, dsrc2);
}

def_EHelper(sll){
  rtl_sll(s, ddest, dsrc1, dsrc2);
}

def_EHelper(sra){
  rtl_sra(s, ddest, dsrc1, dsrc2);
}

def_EHelper(srl){
  rtl_srl(s, ddest, dsrc1, dsrc2);
}

def_EHelper(and){
  rtl_and(s, ddest, dsrc1, dsrc2);
}

def_EHelper(sltu){
  *ddest = interpret_relop(RELOP_LTU, *dsrc1, *dsrc2) ? 1 : 0;
}

def_EHelper(slt){
  *ddest = interpret_relop(RELOP_LT, *dsrc1, *dsrc2) ? 1 : 0;
}

// B-Type
def_EHelper(bne){
  rtl_bne(s, dsrc1, dsrc2, id_dest->simm);
}

def_EHelper(beq){
  rtl_beq(s, dsrc1, dsrc2, id_dest->simm);
}

def_EHelper(bge){
  rtl_bge(s, dsrc1, dsrc2, id_dest->simm);
}

def_EHelper(blt){
  rtl_blt(s, dsrc1, dsrc2, id_dest->simm);
}

def_EHelper(bltu){
  rtl_bltu(s, dsrc1, dsrc2, id_dest->simm);
}

def_EHelper(bgeu){
  rtl_bgeu(s, dsrc1, dsrc2, id_dest->simm);
}