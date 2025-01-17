﻿#include <string>
#include <iostream> //debug
#include <stdexcept> //std::exit

#include "cpu.hpp"
#include "instruction.hpp"


using namespace std;

cpu::cpu(){
  pc = 0x10000000;
  npc = 0x10000004;
  LO = 0;
  HI = 0;
}

cpu::cpu(std::string binary): m(binary), r() {
  pc = 0x10000000;
  npc = 0x10000004;
  LO = 0;
  HI = 0;
}

void cpu::run() {
	int cycle = 0;
	while (true) {

		word next_instruction = m.read_inst(pc);

		instruction c_inst(next_instruction);



		this->execute(c_inst);
		cout << "cycle :" << ++cycle << "pc:" << pc <<" npc:"<<npc << endl;
		
		cout << "register[0] " << r[0] << "register[2] " << r[2] << "register[29] " << r[29] << " register[30]" << r[30] << endl;

		if (pc == -1) {

			exit(r[2]);

		}

	}

}

//void cpu::run5stage(){
//  while(true) {
//	  word instruct=this->IF();
//	  instruction c_inst=this->ID(instruct);
//    this->execute(c_inst);
//
//    if(pc == 0){
//      exit(r[2]);
//    }
//  }
//}

word cpu::pc_increase(word offset){
	return pc + 4;
}

void cpu::execute(const instruction& inst){
	cout << inst;
  switch(inst.type){
    case 'r': execute_r(inst); break;
    case 'i': execute_i(inst); break;
    case 'j': execute_j(inst); break;
    default: std::cerr << "error: incorect instruction type" ;std::exit(-20);
  }
}

//word cpu::IF()
//{
//	word instruction = m.read_inst(pc);
//	return instruction;
//}
//
//instruction cpu::ID(word next_instruction)
//{
//	instruction inst(next_instruction);
//	rs = r[inst.src_s];
//	rt = r[inst.src_t];
//}
//
//void cpu::EX()
//{
//}
//
//void cpu::Mem()
//{
//}
//
//void cpu::WB()
//{
//}

void cpu::execute_r(const instruction& inst){
	
  switch (inst.funct){
    case 0x00: cout<<"SLL"  <<endl; SLL(inst); break;  //rs
    case 0x02: cout<<"SRL"  <<endl; SRL(inst); break;  //rs
    case 0x03: cout<<"SRA"  <<endl; SRA(inst); break;  //rs
    case 0x04: cout<<"SLLV" <<endl; SLLV(inst); break; // TODO: dodalem to nie wiem czy gdzies jescze trzeba cos zmienic
    case 0x06: cout<<"SRLV" <<endl; SRLV(inst); break;
    case 0x07: cout<<"SRAV" <<endl; SRAV(inst); break; //shamt
    case 0x08: cout<<"JR"   <<endl; JR(inst); break;   //rt,rd
    case 0x09: cout<<"JALR" <<endl; JALR(inst); break; //rt
    case 0x10: cout<<"MFHI" <<endl; MFHI(inst); break; //rs,rt
    case 0x11: cout<<"MTHI" <<endl; MTHI(inst); break; //rt,rd,shamt
    case 0x12: cout<<"MFLO" <<endl; MFLO(inst); break; //rs,rt
    case 0x13: cout<<"MTLO" <<endl; MTLO(inst); break; //rt,rd,shamt
    case 0x18: cout<<"MULT" <<endl; MULT(inst); break; //rd,shamt
    case 0x19: cout<<"MULTU"<<endl; MULTU(inst); break; //rd,shamt
    case 0x1A: cout<<"DIV"  <<endl; DIV(inst); break; //rd,shamt
    case 0x1B: cout<<"DIVU" <<endl; DIVU(inst); break; //rd,shamt
    case 0x20: cout<<"ADD"  <<endl; ADD(inst); break;  //shamt
    case 0x21: cout<<"ADDU" <<endl; ADDU(inst); break; //shamt
    case 0x22: cout<<"SUB"  <<endl; SUB(inst); break; //shamt
    case 0x23: cout<<"SUBU" <<endl; SUBU(inst); break; //shamt
    case 0x24: cout<<"AND"  <<endl; AND(inst); break; //shamt
    case 0x25: cout<<"OR"   <<endl; OR(inst); break; //shamt
    case 0x26: cout<<"XOR"  <<endl; XOR(inst); break; //shamt
    case 0x2A: cout<<"SLT"  <<endl; SLT(inst); break; //shamt
    case 0x2B: cout<<"SLTU" <<endl; SLTU(inst); break; //shamt

    default: std::cerr << "error: r instruction not implemented" << '\n'; std::exit(-12);
  }
}

void cpu::execute_i(const instruction& inst){
  //test_zero_fields_I(inst);
  switch (inst.opcode){
    case 0x01: { //branches
      switch (inst.src_t){
        case 0x00:cout<<"BLTZ"<<endl; BLTZ(inst); break;
        case 0x01:cout<<"BGEZ"<<endl; BGEZ(inst); break;
        case 0x10:cout<<"BLTZAL"<<endl; BLTZAL(inst); break;
        case 0x11:cout<<"BGEZAL"<<endl; BGEZAL(inst); break;
        default: std::cerr << "error: i instruction not implemented" << '\n'; std::exit(-12);
      }
     } break;
    case 0x04:cout<<"BEQ"<<endl; BEQ(inst); break;
    case 0x05:cout<<"BNE"<<endl; BNE(inst); break;
    case 0x06:cout<<"BLEZ"<<endl; BLEZ(inst); break; //rt
    case 0x07:cout<<"BGTZ"<<endl; BGTZ(inst); break; //rt
    case 0x08:cout<<"ADDI"<<endl; ADDI(inst); break;
    case 0x09:cout<<"ADDIU"<<endl; ADDIU(inst); break;
    case 0x0A:cout<<"SLTI"<<endl; SLTI(inst); break;
    case 0x0B:cout<<"SLTIU"<<endl; SLTIU(inst); break;
    case 0x0C:cout<<"ANDI"<<endl; ANDI(inst); break;
    case 0x0D:cout<<"ORI"<<endl; ORI(inst); break;
    case 0x0E:cout<<"XORI"<<endl; XORI(inst); break;
    case 0x0F:cout<<"LUI"<<endl; LUI(inst); break; //rs
    case 0x20:cout<<"LB"<<endl; LB(inst); break;
    case 0x21:cout<<"LH"<<endl; LH(inst); break;
    case 0x22:cout<<"LWL"<<endl; LWL(inst); break;
    case 0x23:cout<<"LW"<<endl; LW(inst); break;
    case 0x24:cout<<"LBU"<<endl; LBU(inst); break;
    case 0x25:cout<<"LHU"<<endl; LHU(inst); break;
    case 0x26:cout<<"LWR"<<endl; LWR(inst); break;
    case 0x28:cout<<"SB"<<endl; SB(inst); break;
    case 0x29:cout<<"SH"<<endl; SH(inst); break;
    case 0x2B:cout<<"SW"<<endl; SW(inst); break;
    default: std::cerr << "error: i instruction not implemented" << '\n'; std::exit(-12);
  }
 }
void cpu::execute_j(const instruction& inst){
  switch (inst.opcode) {
    case 0x02:cout<<"J"<<endl; J(inst); break;
    case 0x03:cout<<"JAL"<<endl; JAL(inst); break;
    default: std::cerr << "error: j instruction not implemented" << '\n'; std::exit(-12);
  }
 }

word cpu::sign_extend_imi(const instruction& inst){
  word imi = inst.i_imi;
  //取高16位
  return (imi >= 0x8000) ? (0xFFFF0000 | imi) : imi;
 }

// INSTRUCTIONS
void cpu::ADD(const instruction& inst){
  //id
  s_word r1 = r[inst.src_s];
  s_word r2 = r[inst.src_t];
  //ex
  s_word res = r1 + r2;

  if((res < 0 && r1 >= 0 && r2 >= 0)||(res >= 0 && r1 < 0 && r2 < 0)){
    std::cerr << "exception: arithmetic error" << std::endl;
    std::exit(-10);
  }

  r[inst.destn] = res;
  pc_increase(4);
 }
void cpu::ADDI(const instruction& inst){
  //TODO: check immiatde sing extension
  s_word r1 = r[inst.src_s];
  s_word imi = sign_extend_imi(inst);
  s_word res = r1 + imi;

  if( (res <= 0 && r1 > 0 && imi > 0) || (res >= 0 && r1 < 0 && imi < 0) ){
    std::cerr << "exception: arithmetic error" << '\n';
    std::exit(-10);
  }

  r[inst.src_t] = res;
  pc_increase(4);
 }
void cpu::ADDIU(const instruction& inst){
  word r1 = r[inst.src_s];
  word imi = sign_extend_imi(inst);
  s_word res = r1 + imi;
  r[inst.src_t] = res;
  pc_increase(4);
 }
void cpu::ADDU(const instruction& inst){
  word r1 = r[inst.src_s];
  word r2 = r[inst.src_t];
  word res = r1 + r2;
  r[inst.destn] = res;
  pc_increase(4);
 }
void cpu::AND(const instruction& inst){
  word r1 = r[inst.src_s];
  word r2 = r[inst.src_t];
  word res = r1 & r2;
  r[inst.destn] = res;
  pc_increase(4);
 }
void cpu::ANDI(const instruction& inst){
  word r1 = r[inst.src_s];
  word r2 = inst.i_imi;
  word res = r1 & r2;
  r[inst.src_t] = res;
  pc_increase(4);
 }
void cpu::BEQ(const instruction& inst){
  word r1 = r[inst.src_s];
  word r2 = r[inst.src_t];
  if(r1 == r2){
    word offset = sign_extend_imi(inst) << 2;
    pc_increase(offset);
  }
  else{
    pc_increase(4);
  }
 }
void cpu::BGEZ(const instruction& inst){
  s_word r1 = r[inst.src_s];
  if(r1 >= 0){
    word offset = sign_extend_imi(inst) << 2;
    pc_increase(offset);
  }
  else{
    pc_increase(4);
  }
 }
void cpu::BGEZAL(const instruction& inst){
  s_word r1 = r[inst.src_s];
  r[31] = npc + 4;
  if(r1 >= 0){
    word offset = sign_extend_imi(inst) << 2;
    pc_increase(offset);
  }
  else{
    pc_increase(4);
  }
 }
void cpu::BGTZ(const instruction& inst){
  s_word r1 = r[inst.src_s];
  if(r1 > 0){
    word offset = sign_extend_imi(inst) << 2;
    pc_increase(offset);
  }
  else{
    pc_increase(4);
  }
 }
void cpu::BLEZ(const instruction& inst){
  s_word r1 = r[inst.src_s];
  if(r1 <= 0){
    word offset = sign_extend_imi(inst) << 2;
    pc_increase(offset);
  }
  else{
    pc_increase(4);
  }
 }
void cpu::BLTZ(const instruction& inst){
  s_word r1 = r[inst.src_s];
  if(r1 < 0){
    word offset = sign_extend_imi(inst) << 2;
    pc_increase(offset);
  }
  else{
    pc_increase(4);
  }
 }
void cpu::BLTZAL(const instruction& inst){
  s_word r1 = r[inst.src_s];
  r[31] = npc + 4; // same stuff
  if(r1 < 0){
    word offset = sign_extend_imi(inst) << 2;
    pc_increase(offset);
  }
  else{
    pc_increase(4);
  }
 }
void cpu::BNE(const instruction& inst){
  word r1 = r[inst.src_s];
  word r2 = r[inst.src_t];
  if(r1 != r2){
    word offset = sign_extend_imi(inst) << 2;
    pc_increase(offset);
  }
  else{
    pc_increase(4);
  }
 }
void cpu::DIV(const instruction& inst){
  s_word r1 = r[inst.src_s];
  s_word r2 = r[inst.src_t];
  if(r2 != 0){
    LO = (s_word) r1 / r2;
    HI = (s_word) r1 % r2;
  }
  pc_increase(4);
 }
void cpu::DIVU(const instruction& inst){
  word r1 = r[inst.src_s];
  word r2 = r[inst.src_t];
  if(r2 != 0){
  LO = (word) r1 / r2;
  HI = (word) r1 % r2;
    }
  pc_increase(4);
 }
void cpu::J(const instruction& inst){
  pc = npc;
  npc = (npc & 0xF0000000)|(inst.j_add << 2);
 }

void cpu::JALR(const instruction& inst){
  r[inst.destn] = npc + 4;
  word adr = r[inst.src_s];
  pc = npc;
  npc = adr;
 }

void cpu::JAL(const instruction& inst){
  r[31] = npc + 4;
  pc = npc;
  npc = (word)((pc & 0xF0000000)|(inst.j_add << 2));
 }

void cpu::JR(const instruction& inst){
  word jump_address = r[inst.src_s];
  pc = npc;
  npc = jump_address;
 }

void cpu::LB(const instruction& inst){
  s_word base = r[inst.src_s];
  s_word offset = sign_extend_imi(inst);
  word adr = base + offset;
  word res = m.read_b(adr);
  if(res >= 0x80) res = 0xFFFFFF00 | res;
  r[inst.src_t] = res;
  pc_increase(4);
 }

void cpu::LBU(const instruction& inst){
  s_word base = r[inst.src_s];
  s_word offset = sign_extend_imi(inst);
  word adr = base + offset;
  word res = m.read_b(adr);
  r[inst.src_t] = res;
  pc_increase(4);
 }

void cpu::LH(const instruction& inst){
  s_word base = r[inst.src_s];
  s_word offset = sign_extend_imi(inst);
  word adr = base + offset;
  word res = m.read_h(adr);
  if(res >= 0x8000) res = 0xFFFF0000 | res;
  r[inst.src_t] = res;
  pc_increase(4);
 }

void cpu::LHU(const instruction& inst){
  s_word base = r[inst.src_s];
  s_word offset = sign_extend_imi(inst);
  word adr = base + offset;
  word res = m.read_h(adr);
  r[inst.src_t] = res;
  pc_increase(4);
 }

void cpu::LUI(const instruction& inst){
  word data = inst.i_imi << 16;
  r[inst.src_t] = data;
  pc_increase(4);
 }

void cpu::LW(const instruction& inst){
  s_word base = r[inst.src_s];
  s_word offset = sign_extend_imi(inst);
  word adr = base + offset;
  word res = m.read_w(adr);
  r[inst.src_t] = res;
  pc_increase(4);
 }
void cpu::LWL(const instruction& inst){
  s_word base = r[inst.src_s];
  s_word offset = sign_extend_imi(inst);
  //word w_adr = (word) ((base + offset) - (base + offset) % 4);
  word w_adr = (base + offset) - (base + offset) % 4;

  word full_word = m.read_w(w_adr);
  word res = r[inst.src_t];
  int w_off = offset & 0x3;
  switch(w_off){
    case 0x0: res = full_word; break;
    case 0x1: res = (res & 0x000000FF) | (full_word & 0x00FFFFFF) << 8;  break;
    case 0x2: res = (res & 0x0000FFFF) | (full_word & 0x0000FFFF) << 16; break;
    case 0x3: res = (res & 0x00FFFFFF) | (full_word & 0x000000FF) << 24; break;
  }
  r[inst.src_t] = res;
  pc_increase(4);
 }
void cpu::LWR(const instruction& inst){
  s_word base = r[inst.src_s];
  s_word offset = sign_extend_imi(inst);
  //word w_adr = (word) ((base + offset) - (base + offset) % 4);
  word w_adr = (base + offset) - (base + offset) % 4;

  word full_word = m.read_w(w_adr);
  word res = r[inst.src_t];
  int w_off = offset & 0x3;
  switch(w_off){
    case 0x0: res = (res & 0xFFFFFF00) | full_word >> 24; break;
    case 0x1: res = (res & 0xFFFF0000) | full_word >> 16; break;
    case 0x2: res = (res & 0xFF000000) | full_word >> 8;  break;
    case 0x3: res = full_word; break;
  }
  r[inst.src_t] = res;
  pc_increase(4);
 }
void cpu::MFHI(const instruction& inst){
  word data = HI;
  r[inst.destn] = data;
  pc_increase(4);
 }
void cpu::MFLO(const instruction& inst){
  word data = LO;
  r[inst.destn] = data;
  pc_increase(4);
 }
void cpu::MTHI(const instruction& inst){
  HI = r[inst.src_s];
  pc_increase(4);
 }
void cpu::MTLO(const instruction& inst){
  LO = r[inst.src_s];
  pc_increase(4);
 }
void cpu::MULT(const instruction& inst){
  s_word r1 = r[inst.src_s];
  s_word r2 = r[inst.src_t];
  int64_t res = static_cast<int64_t> (r1) * static_cast<int64_t> (r2);
  LO = static_cast<word> (res & 0x00000000FFFFFFFF);
  HI = static_cast<word> ((res & 0xFFFFFFFF00000000) >> 32);
  pc_increase(4);
 }
void cpu::MULTU(const instruction& inst){
  word r1 = r[inst.src_s];
  word r2 = r[inst.src_t];
  uint64_t res = static_cast<uint64_t> (r1) * static_cast<uint64_t> (r2);
  LO = static_cast<word> (res & 0x00000000FFFFFFFF);
  HI = static_cast<word> ((res & 0xFFFFFFFF00000000) >> 32);
  pc_increase(4);
 }
void cpu::OR(const instruction& inst){
  word r1 = r[inst.src_s];
  word r2 = r[inst.src_t];
  word res = r1 | r2;
  r[inst.destn] = res;
  pc_increase(4);
 }
void cpu::ORI(const instruction& inst){
  word r1 = r[inst.src_s];
  word res = r1 | inst.i_imi;
  r[inst.src_t] = res;
  pc_increase(4);
 }

void cpu::SB(const instruction& inst){
  s_word base = r[inst.src_s];
  s_word offset = sign_extend_imi(inst);
  word adr = base + offset;
  word val = r[inst.src_t] & 0x000000FF;
  m.write_b(adr, val);
  pc_increase(4);
 }

void cpu::SH(const instruction& inst){
  s_word base = r[inst.src_s];
  s_word offset = sign_extend_imi(inst);
  word adr = base + offset;
  word val = r[inst.src_t] & 0x0000FFFF;
  m.write_h(adr, val);
  pc_increase(4);
 }

void cpu::SLL(const instruction& inst){
  word r1 = r[inst.src_t];
  word res = r1 << inst.shamt;
  r[inst.destn] = res;
  pc_increase(4);
 }

void cpu::SLLV(const instruction& inst){
  word r1 = r[inst.src_s] & 0x1F;
  word r2 = r[inst.src_t];
  word res = r2 << r1;
  r[inst.destn] = res;
  pc_increase(4);
 }

void cpu::SLT(const instruction& inst){
  s_word r1 = r[inst.src_s];
  s_word r2 = r[inst.src_t];
  word res = (r1 < r2) ? 1:0;
  r[inst.destn] = res;
  pc_increase(4);
 }
void cpu::SLTI(const instruction& inst){
  s_word r1 = r[inst.src_s];
  s_word r2 = sign_extend_imi(inst);
  word res = (r1 < r2) ? 1:0;
  r[inst.src_t] = res;
  pc_increase(4);
 }
void cpu::SLTIU(const instruction& inst){
  word r1 = r[inst.src_s];
  word r2 = sign_extend_imi(inst);
  word res = (r1 < r2) ? 1:0;
  r[inst.src_t] = res;
  pc_increase(4);
 }
void cpu::SLTU(const instruction& inst){
  word r1 = r[inst.src_s];
  word r2 = r[inst.src_t];
  word res = (r1 < r2) ? 1:0;
  r[inst.destn] = res;
  pc_increase(4);
 }

void cpu::SRA(const instruction& inst){
  s_word r1 = r[inst.src_t];
  s_word res = r1 >> inst.shamt;
  r[inst.destn] = res;
  pc_increase(4);
 }

void cpu::SRAV(const instruction& inst){
  word r1 = r[inst.src_s];
  s_word r2 = r[inst.src_t];
  s_word res = r2 >> r1;
  r[inst.destn] = res;
  pc_increase(4);
}

void cpu::SRL(const instruction& inst){
  word r1 = r[inst.src_t];
  word res = r1 >> inst.shamt;
  r[inst.destn] = res;
  pc_increase(4);
 }

void cpu::SRLV(const instruction& inst){
  word r1 = r[inst.src_s] & 0x1F;
  word r2 = r[inst.src_t];
  word res = r2 >> r1;
  r[inst.destn] = res;
  pc_increase(4);
}

void cpu::SUB(const instruction& inst){
  s_word r1 = r[inst.src_s];
  s_word r2 = r[inst.src_t];
  s_word res = r1 - r2;
  if((r1 >= 0 && r2 < 0 && res < 0)||(r1 < 0 && r2 >= 0 && res > 0 )){
    std::cerr << "exception: arithmetic error" << std::endl;
    std::exit(-10);
  }
  else{
    r[inst.destn] = res;
    pc_increase(4);
  }
 }
void cpu::SUBU(const instruction& inst){ // not tested
  word r1 = r[inst.src_s];
  word r2 = r[inst.src_t];
  word res = r1 - r2;
  r[inst.destn] = res;
  pc_increase(4);
 }
void cpu::SW(const instruction& inst){
  s_word base = r[inst.src_s];
  s_word offset = sign_extend_imi(inst);
  word adr = base + offset;
  word val = r[inst.src_t];
  m.write_w(adr, val);
  pc_increase(4);
 }
void cpu::XOR(const instruction& inst){
  word r1 = r[inst.src_s];
  word r2 = r[inst.src_t];
  word res = r1 ^ r2;
  r[inst.destn] = res;
  pc_increase(4);
 }
void cpu::XORI(const instruction& inst){
  word r1 = r[inst.src_s];
  word res = r1 ^ inst.i_imi;
  r[inst.src_t] = res;
  pc_increase(4);
 }


void cpu::test_fill(){

 }

void cpu::reg_s(){
  for(int i = 1; i < 32; i++){
    std::cerr<<"reg"<<i<<"\t"<<(s_word)r.get(i)<<std::endl;
  }
 }

void cpu::reg_print(bool s_nbr){
  if(s_nbr){
    for(int i = 0; i < 4; i++){
      for(int j = 0; j < 8; j++){
        std::string v = std::to_string((s_word)r.get(i*8 + j));
        std::cerr << v;
        for(int i = v.length(); i < 12; i++){
          std::cerr << ' ';
        }
      }
    std::cerr << '\n';
    }
  }
  else{
    for(int i = 0; i < 4; i++){
      for(int j = 0; j < 8; j++){
        std::string v = std::to_string(r.get(i*8 + j));
        std::cerr << v;
        for(int i = v.length(); i < 12; i++){
          std::cerr << ' ';
        }
      }
    std::cerr << '\n';
    }
  }
 }


void cpu::test_zero_fields_R(const instruction& inst){
  if(inst.opcode != 0){
    std::cerr << "error: invalid instruction (opcode)" << '\n';
    std::exit(-12);
  }
  switch (inst.funct){
    case 0x00: if(inst.src_s != 0) {std::cerr << "error: invalid instruction srcs" << '\n'; std::exit(-12);} break;  //rs
    case 0x02: if(inst.src_s != 0) {std::cerr << "error: invalid instruction" << '\n'; std::exit(-12);} break;  //rs
    case 0x03: if(inst.src_s != 0) {std::cerr << "error: invalid instruction" << '\n'; std::exit(-12);} break; //rs
    case 0x07: if(inst.shamt != 0) {std::cerr << "error: invalid instruction" << '\n'; std::exit(-12);} break; //shamt
    case 0x08: if(inst.src_t != 0 || inst.destn != 0) {std::cerr << "error: invalid instruction" << '\n'; std::exit(-12);} break;   //rt,rd
    case 0x09: if(inst.src_t != 0) {std::cerr << "error: invalid instruction" << '\n'; std::exit(-12);} break; //rt
    case 0x10: if(inst.src_s != 0 || inst.src_t != 0) {std::cerr << "error: invalid instruction" << '\n'; std::exit(-12);} break; //rs,rt
    case 0x11: if(inst.src_t != 0 || inst.destn != 0 || inst.shamt != 0) {std::cerr << "error: invalid instruction" << '\n'; std::exit(-12);} break; //rt,rd,shamt
    case 0x12: if(inst.src_s != 0 || inst.src_t != 0) {std::cerr << "error: invalid instruction" << '\n'; std::exit(-12);} break; //rs,rt
    case 0x13: if(inst.src_t != 0 || inst.destn != 0 || inst.shamt != 0) {std::cerr << "error: invalid instruction" << '\n'; std::exit(-12);} break; //rt,rd,shamt
    case 0x18: if(inst.destn != 0 || inst.shamt != 0) {std::cerr << "error: invalid instruction" << '\n'; std::exit(-12);} break; //rd,shamt
    case 0x19: if(inst.destn != 0 || inst.shamt != 0) {std::cerr << "error: invalid instruction" << '\n'; std::exit(-12);} break; //rd,shamt
    case 0x1A: if(inst.destn != 0 || inst.shamt != 0) {std::cerr << "error: invalid instruction" << '\n'; std::exit(-12);} break; //rd,shamt
    case 0x1B: if(inst.destn != 0 || inst.shamt != 0) {std::cerr << "error: invalid instruction" << '\n'; std::exit(-12);} break; //rd,shamt
    case 0x20: if(inst.shamt != 0) {std::cerr << "error: invalid instruction" << '\n'; std::exit(-12);} break;  //shamt
    case 0x21: if(inst.shamt != 0) {std::cerr << "error: invalid instruction" << '\n'; std::exit(-12);} break; //shamt
    case 0x22: if(inst.shamt != 0) {std::cerr << "error: invalid instruction" << '\n'; std::exit(-12);} break; //shamt
    case 0x23: if(inst.shamt != 0) {std::cerr << "error: invalid instruction" << '\n'; std::exit(-12);} break; //shamt
    case 0x24: if(inst.shamt != 0) {std::cerr << "error: invalid instruction" << '\n'; std::exit(-12);} break; //shamt
    case 0x25: if(inst.shamt != 0) {std::cerr << "error: invalid instruction" << '\n'; std::exit(-12);} break; //shamt
    case 0x26: if(inst.shamt != 0) {std::cerr << "error: invalid instruction" << '\n'; std::exit(-12);} break; //shamt
    case 0x2A: if(inst.shamt != 0) {std::cerr << "error: invalid instruction" << '\n'; std::exit(-12);} break; //shamt
    case 0x2B: if(inst.shamt != 0) {std::cerr << "error: invalid instruction" << '\n'; std::exit(-12);} break; //shamt
    default: std::cerr << "error: r instruction not implemented" << '\n'; std::exit(-12);
  }
}

  void cpu::test_zero_fields_I(const instruction& inst){
    switch (inst.opcode){
    case 0x06: if(inst.src_t != 0) {std::cerr << "error: invalid instruction" << '\n'; std::exit(-12);} break; //rt
    case 0x07: if(inst.src_t != 0) {std::cerr << "error: invalid instruction" << '\n'; std::exit(-12);} break; //rt
    case 0x0F: if(inst.src_s != 0) {std::cerr << "error: invalid instruction" << '\n'; std::exit(-12);} break; //rs
    }
  }
