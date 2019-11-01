#include"piplinecpu.hpp"

#include <string>
#include <iostream> //debug
#include <stdexcept> //std::exit
#include <condition_variable>
#include "instruction.hpp"

using namespace std;

piplinecpu::piplinecpu(): cpu(), is_finish(false){
	pc = 0x10000000;
	npc = 0x10000004;
	LO = 0;
	HI = 0;
}

piplinecpu::piplinecpu(std::string binary) :cpu(binary), is_finish(false) {
	pc = 0x10000000;
	npc = 0x10000004;
	LO = 0;
	HI = 0;
}

void piplinecpu::run1stage() {
	IFID ifid = IF(pc);
	// IF/ID
	IDEX idex = ID(ifid);
	// ID/EX
	EXMEM exmem = EX(idex);
	//
}

void piplinecpu::run5stage() {
	is_ex = true;
	thread trdcycle(&piplinecpu::CycleControl, this); // 
	
	
	
	trdcycle.join();
	trdif.join();
	trdid.join(); 
	trdex.join();
	trdmem.join();
	trdwb.join();

	/*while (true) {
		word instruct = this->IF();
		instruction c_inst = this->ID(instruct);
		this->EX(c_inst);

		if (pc == 0) {
			exit(r[2]);
		}
	}*/
}





void piplinecpu::CycleControl()
{
	
	while (!is_finish)
	{
		unique_lock<std::mutex> lkd(mutcycledone);
		std::this_thread::sleep_for(100ms);
		unique_lock<std::mutex> lk(mutcycle);
		
		cout << "===============================" << ++cycle << "=================================" << endl;
		cvcycle.notify_all();
		//std::this_thread::sleep_for(3s);
		
	}

	//return instruction;
}


IFID piplinecpu::IF(word pc)
{
	word instruction = m.read_inst(pc);
	word pc = pc_increase(4);
	
	IFID ifid;
	ifid.instruction = instruction;
	ifid.pc = pc;
	return ifid;
}

IDEX piplinecpu::ID(IFID ifid)
{
	IDEX idex;
	instruction inst(ifid.instruction);
	idex.sign.ALUOp = inst.opcode;
	idex.rs = r[inst.src_s];//32
	idex.rt = r[inst.src_t];//32
	idex.rd_b = inst.destn;//5
	idex.rt_b = inst.src_t;//5
	idex.im.im = inst.i_imi;//32
	idex.im.shamt = inst.shamt;
	idex.im.funct = inst.funct;

	if (inst.type == 'r') {
		idex.sign.RegDst = 1;
		idex.sign.RegWrite = 1;
	}
	else if (inst.is_LW(idex.sign.ALUOp)) {
		idex.sign.ALUSrc = 1;
		idex.sign.MemRead = 1;
		idex.sign.RegDst = 1;
		idex.sign.MemtoReg = 1;
	}
	else if (inst.is_SW(idex.sign.ALUOp)) {
		idex.sign.ALUSrc = 1;
		idex.sign.MemWrite = 1;
	}
	else if (inst.is_BRANCH(idex.sign.ALUOp)) {
		idex.sign.Brch = 1;
	}
	else if (inst.type == 'j') {
		// TODO::

	}

	idex.pc = ifid.pc;
	return idex;

}

EXMEM piplinecpu::EX(IDEX idex) {
	EXMEM exmem;
	SIGN sign = idex.sign;


	if (sign.ALUSrc == 0 && sign.RegDst == 1) {
		exmem.alures =  execute_r(sign.ALUOp, exmem.zero, idex);
		exmem.wb = idex.rd_b;
	}
	else if (sign.ALUSrc == 1 && sign.RegDst == 0) {
		if (sign.Brch == 1) {
			if (execute_i(sign.ALUOp, exmem.zero, idex)) {
				exmem.jadd = idex.pc + idex.im.im << 2;
			}
		}
		else {
			exmem.alures = execute_r(sign.ALUOp, exmem.zero, idex);
			exmem.wb = idex.rt_b;
		}
	}
	exmem.sign = sign;
	return exmem;
}

MEMWB piplinecpu::MEM(EXMEM exmem)
{
	MEMWB memwb;
	SIGN sign = exmem.sign;
	if (sign.Brch == 1 &&exmem.zero) { 
		pc = exmem.jadd;
		return memwb;
	}
	if (sign.MemWrite == 1) {
		m.write_b(exmem.rt, exmem.alures);
		return memwb;
	}
	if (sign.MemRead == 1) {
		memwb.mdata = m.read_b(exmem.rt);
	}
	memwb.alures = exmem.alures;
	memwb.wb = exmem.wb;
	return memwb;
}

void piplinecpu::WB(MEMWB memweb)
{
	SIGN sign = memweb.sign;
	if (sign.MemtoReg == 1) {
		r[memweb.wb] = memweb.mdata;
	}
	else {
		r[memweb.wb] = memweb.alures;
	}
}



word piplinecpu::execute_r(int ALUOp, int& zero, IDEX idex) {
	word rs = idex.rs;
	word rt = idex.rt;
	IM im = idex.im;

	switch (ALUOp) {
	case 0x00: cout << "SLL" << endl; return SLL(zero, im.shamt, rt);
	case 0x02: cout << "SRL" << endl; return SRL(zero, im.shamt, rt);
	case 0x03: cout << "SRA" << endl; return SRA(zero, im.shamt, rt);
	case 0x04: cout << "SLLV" << endl; return SLLV(zero, rs, rt);
	case 0x06: cout << "SRLV" << endl; return SRLV(zero, rs, rt);
	case 0x07: cout << "SRAV" << endl; return SRAV(zero, rs, rt);
		//case 0x08: cout << "JR" << endl; JR(inst, rs, rt); break;   //rt,rd
		//case 0x09: cout << "JALR" << endl; JALR(inst, rs, rt); break; //rt
		//case 0x10: cout << "MFHI" << endl; MFHI(inst, rs, rt); break; //rs,rt
		//case 0x11: cout << "MTHI" << endl; MTHI(inst, rs, rt); break; //rt,rd,shamt
		//case 0x12: cout << "MFLO" << endl; MFLO(inst, rs, rt); break; //rs,rt
		//case 0x13: cout << "MTLO" << endl; MTLO(inst, rs, rt); break; //rt,rd,shamt
		//case 0x18: cout << "MULT" << endl; MULT(inst, rs, rt); break; //rd,shamt
		//case 0x19: cout << "MULTU" << endl; MULTU(inst, rs, rt); break; //rd,shamt
		//case 0x1A: cout << "DIV" << endl; DIV(inst, rs, rt); break; //rd,shamt
		//case 0x1B: cout << "DIVU" << endl; DIVU(inst, rs, rt); break; //rd,shamt
	case 0x20: cout << "ADD" << endl; return ADD(zero, rs, rt);
	case 0x21: cout << "ADDU" << endl; return ADDU(zero, rs, rt);
	case 0x22: cout << "SUB" << endl; return SUB(zero, rs, rt);
	case 0x23: cout << "SUBU" << endl; return SUBU(zero, rs, rt);
	case 0x24: cout << "AND" << endl; return AND(zero, rs, rt);
	case 0x25: cout << "OR" << endl; return OR(zero, rs, rt);
	case 0x26: cout << "XOR" << endl; return XOR(zero, rs, rt);
	case 0x2A: cout << "SLT" << endl; return SLT(zero, rs, rt);
	case 0x2B: cout << "SLTU" << endl; return SLTU(zero, rs, rt);

	default: std::cerr << "error: r instruction not implemented" << '\n'; std::exit(-12);
	}
}

word piplinecpu::execute_i(int ALUOp, int& zero, IDEX idex) {
	//test_zero_fields_I(inst);
	word rs = idex.rs;
	word rt = idex.rt;
	IM im = idex.im;

	switch (ALUOp) {
		//case 0x01: { //branches
		//	switch (inst.src_t) {
		//	case 0x00:cout << "BLTZ" << endl; BLTZ(inst, rs, rt); break;
		//	case 0x01:cout << "BGEZ" << endl; BGEZ(inst, rs, rt); break;
		//	case 0x10:cout << "BLTZAL" << endl; BLTZAL(inst, rs, rt); break;
		//	case 0x11:cout << "BGEZAL" << endl; BGEZAL(inst, rs, rt); break;
		//	default: std::cerr << "error: i inst, rs, rtruction not implemented" << '\n'; std::exit(-12);
		//	}
		//} break;
	case 0x04:cout << "BEQ" << endl; return BEQ(zero, rs, rt);
	case 0x05:cout << "BNE" << endl; return BNE(zero, rs, rt);
		//case 0x06:cout << "BLEZ"  << endl; BLEZ(inst, rs, rt); break; //rt
		//case 0x07:cout << "BGTZ"  << endl; BGTZ(inst, rs, rt); break; //rt
	case 0x08:cout << "ADDI" << endl; return ADDI(zero, rs, im.im);
	case 0x09:cout << "ADDIU" << endl; return ADDIU(zero, rs, im.im);
	case 0x0A:cout << "SLTI" << endl; return SLTI(zero, rs, im.im);
	case 0x0B:cout << "SLTIU" << endl; return SLTIU(zero, rs, im.im);
	case 0x0C:cout << "ANDI" << endl; return ANDI(zero, rs, im.im);
	case 0x0D:cout << "ORI" << endl; return ORI(zero, rs, im.im);
	case 0x0E:cout << "XORI" << endl; return XORI(zero, rs, im.im);
	case 0x0F:cout << "LUI" << endl; return LUI(zero, rs, im.im);
		//case 0x20:cout << "LB" << endl; LB(inst, rs, rt); break;
		//case 0x21:cout << "LH" << endl; LH(inst, rs, rt); break;
		//case 0x22:cout << "LWL" << endl; LWL(inst, rs, rt); break;
	case 0x23:cout << "LW" << endl; return LW(zero, rs, im.im);
		//case 0x24:cout << "LBU" << endl; LBU(inst, rs, rt); break;
		//case 0x25:cout << "LHU" << endl; LHU(inst, rs, rt); break;
		//case 0x26:cout << "LWR" << endl; LWR(inst, rs, rt); break;
		//case 0x28:cout << "SB" << endl; SB(inst, rs, rt); break;
		//case 0x29:cout << "SH" << endl; SH(inst, rs, rt); break;
	case 0x2B:cout << "SW" << endl; return SW(zero, rs, im.im);
	default: std::cerr << "error: i instruction not implemented" << '\n'; std::exit(-12);
	}
}


//void piplinecpu::execute_j(int& zero, s_word rs, s_word rt) {
//	switch (inst.opcode) {
//	case 0x02:cout << "J" << endl; J(inst, rs, rt); break;
//	case 0x03:cout << "JAL" << endl; JAL(inst, rs, rt); break;
//	default: std::cerr << "error: j instruction not implemented" << '\n'; std::exit(-12);
//	}
//}




// INSTRUCTIONS
word piplinecpu::ADD(int& zero, s_word rs, s_word rt) {
	s_word res = rs + rt;
	if ((res < 0 && rs >= 0 && rt >= 0) || (res >= 0 && rs < 0 && rt < 0)) {
		zero = 1;
	}
	return res;
}

word piplinecpu::ADDU(int& zero, s_word rs, s_word rt) {
	return rs + rt;
}

word piplinecpu::SUB(int& zero, s_word rs, s_word rt) {
	s_word res = rs - rt;
	if ((rs >= 0 && rt < 0 && res < 0) || (rs < 0 && rt >= 0 && res > 0)) {
		zero = 1;
	}
	return res;
}

word piplinecpu::SUBU(int& zero, s_word rs, s_word rt) { // not tested
	return rs - rt;
}

word piplinecpu::AND(int& zero, s_word rs, s_word rt) {
	return rs & rt;
}

word piplinecpu::OR(int& zero, s_word rs, s_word rt) {
	return rs | rt;
}

word piplinecpu::XOR(int& zero, s_word rs, s_word rt) {
	return rs ^ rt;
}

word piplinecpu::SLT(int& zero, s_word rs, s_word rt) {
	return (rs < rt) ? 1 : 0;
}

word piplinecpu::SLTU(int& zero, s_word rs, s_word rt) {
	return (rs < rt) ? 1 : 0;
}

word piplinecpu::SLL(int& zero, int shamt, s_word rt) {
	return rt << shamt;
}

word piplinecpu::SRL(int& zero, int shamt, s_word rt) {
	return rt >> shamt;
}

word piplinecpu::SRA(int& zero, int shamt, s_word rt) {
	return rt >> shamt;
}

word piplinecpu::SLLV(int& zero, s_word rs, s_word rt) {
	word r1 = rs & 0x1F;
	word r2 = rt;
	word res = r2 << r1;
	return res
}

word piplinecpu::SRLV(int& zero, s_word rs, s_word rt) {
	word r1 = rs & 0x1F;
	word r2 = rt;
	return r2 >> r1;
}

word piplinecpu::SRAV(int& zero, s_word rs, s_word rt) {
	word r1 = rs;
	s_word r2 = rt;
	s_word res = r2 >> r1;
	return res;
}



word piplinecpu::ADDI(int& zero, s_word rs, int im) {
	s_word res = rs + im;

	if ((res <= 0 && rs > 0 && im > 0) || (res >= 0 && rs < 0 && im < 0)) {
		zero = 1;
	}
	return res;
}

word piplinecpu::ADDIU(int& zero, s_word rs, int im) {
	return rs + im;
}

word piplinecpu::ANDI(int& zero, s_word rs, int im) {
	return rs & im;
}

word piplinecpu::ORI(int& zero, s_word rs, int im) {
	return rs | im;
}

word piplinecpu::XORI(int& zero, s_word rs, int im) {
	return rs ^ im;
}

word piplinecpu::LUI(int& zero, s_word rs, int im) {
	return im << 16;
}

word piplinecpu::LW(int& zero, s_word rs, int im) {
	return rs + im;
}

word piplinecpu::SW(int& zero, s_word rs, int im) {
	return rs + im;
}

word piplinecpu::BEQ(int& zero, s_word rs, s_word rt) {
	if (rs == rt) {
		zero = 1;
	}
	return rt;
}

word piplinecpu::BNE(int& zero, s_word rs, s_word rt) {
	if (rs != rt) {
		zero = 1;
	}
	return rt;
}

word piplinecpu::SLTI(int& zero, s_word rs, int im) {
	if (rs < im) {
		zero = 1;
	}
	return im;
}

word piplinecpu::SLTIU(int& zero, s_word rs, int im) {
	if (rs < im) {
		zero = 1;
	}
	return im;
}


//
//void piplinecpu::BGEZ(int& zero, s_word rs, s_word rt) {
//	s_word r1 = rs;
//	if (r1 >= 0) {
//		word offset = sign_extend_imi(inst) << 2;
//		pc_increase(offset);
//	}
//	else {
//		pc_increase(4);
//	}
//}
//
//void piplinecpu::BGEZAL(int& zero, s_word rs, s_word rt) {
//	s_word r1 = rs;
//	r[31] = npc + 4;
//	if (r1 >= 0) {
//		word offset = sign_extend_imi(inst) << 2;
//		pc_increase(offset);
//	}
//	else {
//		pc_increase(4);
//	}
//}
//void piplinecpu::BGTZ(int& zero, s_word rs, s_word rt) {
//	s_word r1 = rs;
//	if (r1 > 0) {
//		word offset = sign_extend_imi(inst) << 2;
//		pc_increase(offset);
//	}
//	else {
//		pc_increase(4);
//	}
//}
//void piplinecpu::BLEZ(int& zero, s_word rs, s_word rt) {
//	s_word r1 = rs;
//	if (r1 <= 0) {
//		word offset = sign_extend_imi(inst) << 2;
//		pc_increase(offset);
//	}
//	else {
//		pc_increase(4);
//	}
//}
//void piplinecpu::BLTZ(int& zero, s_word rs, s_word rt) {
//	s_word r1 = rs;
//	if (r1 < 0) {
//		word offset = sign_extend_imi(inst) << 2;
//		pc_increase(offset);
//	}
//	else {
//		pc_increase(4);
//	}
//}
//void piplinecpu::BLTZAL(int& zero, s_word rs, s_word rt) {
//	s_word r1 = rs;
//	r[31] = npc + 4; // same stuff
//	if (r1 < 0) {
//		word offset = sign_extend_imi(inst) << 2;
//		pc_increase(offset);
//	}
//	else {
//		pc_increase(4);
//	}
//}
//
//void piplinecpu::DIV(int& zero, s_word rs, s_word rt) {
//	s_word r1 = rs;
//	s_word r2 = rt;
//	if (r2 != 0) {
//		LO = (s_word)r1 / r2;
//		HI = (s_word)r1 % r2;
//	}
//	pc_increase(4);
//}
//void piplinecpu::DIVU(int& zero, s_word rs, s_word rt) {
//	word r1 = rs;
//	word r2 = rt;
//	if (r2 != 0) {
//		LO = (word)r1 / r2;
//		HI = (word)r1 % r2;
//	}
//	pc_increase(4);
//}
//void piplinecpu::J(int& zero, s_word rs, s_word rt) {
//	pc = npc;
//	npc = (npc & 0xF0000000) | (inst.j_add << 2);
//}
//
//void piplinecpu::JALR(int& zero, s_word rs, s_word rt) {
//	r[inst.destn] = npc + 4;
//	word adr = rs;
//	pc = npc;
//	npc = adr;
//}
//
//void piplinecpu::JAL(int& zero, s_word rs, s_word rt) {
//	r[31] = npc + 4;
//	pc = npc;
//	npc = (word)((pc & 0xF0000000) | (inst.j_add << 2));
//}
//
//void piplinecpu::JR(int& zero, s_word rs, s_word rt) {
//	word jump_address = rs;
//	pc = npc;
//	npc = jump_address;
//}
//
//void piplinecpu::LB(int& zero, s_word rs, s_word rt) {
//	s_word base = rs;
//	s_word offset = sign_extend_imi(inst);
//	word adr = base + offset;
//	
//	word res = m.read_b(adr);
//	queue_mem.push(make_tuple("rb", adr, 0));
//	if (res >= 0x80) res = 0xFFFFFF00 | res;
//	//wb
//	r[inst.src_t] = res;
//	pc_increase(4);
//}
//
//void piplinecpu::LBU(int& zero, s_word rs, s_word rt) {
//	s_word base = rs;
//	s_word offset = sign_extend_imi(inst);
//	word adr = base + offset;
//	word res = m.read_b(adr);
//	queue_mem.push(make_tuple("rb", adr, 0));
//	r[inst.src_t] = res;
//	pc_increase(4);
//}/**/
//
//void piplinecpu::LH(int& zero, s_word rs, s_word rt) {
//	s_word base = rs;
//	s_word offset = sign_extend_imi(inst);
//	word adr = base + offset;
//	//word res = m.read_h(adr);
//	queue_mem.push(make_tuple("rh", adr, 0));
//	//if (res >= 0x8000) res = 0xFFFF0000 | res;
//	//wb
//	//r[inst.src_t] = res;
//	pc_increase(4);
//}
//
//void piplinecpu::LH_mem(word res) {
//	
//
//	//if (res >= 0x8000) res = 0xFFFF0000 | res;
//	queue_wb.push(res);
//	//wb
//	//r[inst.src_t] = res;
//	pc_increase(4);
//}
//
//void piplinecpu::LH_wb(int src_t,word res) {
//
//
//	if (res >= 0x8000) res = 0xFFFF0000 | res;
//	//wb
//	r[src_t] = res;
//	pc_increase(4);
//}
//
//
//void piplinecpu::LHU(int& zero, s_word rs, s_word rt) {
//	s_word base = rs;
//	s_word offset = sign_extend_imi(inst);
//	word adr = base + offset;
//	word res = m.read_h(adr);
//	queue_mem.push(make_tuple("rh", adr, 0));
//	//wb
//	r[inst.src_t] = res;
//	pc_increase(4);
//}
///**/
//
//
//
//void piplinecpu::LWL(int& zero, s_word rs, s_word rt) {
//	s_word base = rs;
//	s_word offset = sign_extend_imi(inst);
//	//word w_adr = (word) ((base + offset) - (base + offset) % 4);
//	word w_adr = (base + offset) - (base + offset) % 4;
//
//	word full_word = m.read_w(w_adr);
//	queue_mem.push(make_tuple("rw", w_adr, 0));
//	word res = r[inst.src_t];
//	int w_off = offset & 0x3;
//	switch (w_off) {
//	case 0x0: res = full_word; break;
//	case 0x1: res = (res & 0x000000FF) | (full_word & 0x00FFFFFF) << 8;  break;
//	case 0x2: res = (res & 0x0000FFFF) | (full_word & 0x0000FFFF) << 16; break;
//	case 0x3: res = (res & 0x00FFFFFF) | (full_word & 0x000000FF) << 24; break;
//	}
//	//wb
//	r[inst.src_t] = res;
//	pc_increase(4);
//}
//
//void piplinecpu::LWR(int& zero, s_word rs, s_word rt) {
//	s_word base = rs;
//	s_word offset = sign_extend_imi(inst);
//	//word w_adr = (word) ((base + offset) - (base + offset) % 4);
//	word w_adr = (base + offset) - (base + offset) % 4;
//
//	word full_word = m.read_w(w_adr);
//	queue_mem.push(make_tuple("rw", w_adr, 0));
//	word res = rt;
//	int w_off = offset & 0x3;
//	switch (w_off) {
//	case 0x0: res = (res & 0xFFFFFF00) | full_word >> 24; break;
//	case 0x1: res = (res & 0xFFFF0000) | full_word >> 16; break;
//	case 0x2: res = (res & 0xFF000000) | full_word >> 8;  break;
//	case 0x3: res = full_word; break;
//	}
//	//wb
//	r[inst.src_t] = res;
//	pc_increase(4);
//}
///**/
//void piplinecpu::MFHI(int& zero, s_word rs, s_word rt) {
//	word data = HI;
//	//wb
//	r[inst.destn] = data;
//	pc_increase(4);
//}
//
//void piplinecpu::MFLO(int& zero, s_word rs, s_word rt) {
//	word data = LO;
//	//wb
//	r[inst.destn] = data;
//	pc_increase(4);
//}
//
//void piplinecpu::MTHI(int& zero, s_word rs, s_word rt) {
//	HI = rs;
//	pc_increase(4);
//}
//
//void piplinecpu::MTLO(int& zero, s_word rs, s_word rt) {
//	LO = rs;
//	pc_increase(4);
//}
//void piplinecpu::MULT(int& zero, s_word rs, s_word rt) {
//	s_word r1 = rs;
//	s_word r2 = rt;
//	int64_t res = static_cast<int64_t> (r1) * static_cast<int64_t> (r2);
//	LO = static_cast<word> (res & 0x00000000FFFFFFFF);
//	HI = static_cast<word> ((res & 0xFFFFFFFF00000000) >> 32);
//	pc_increase(4);
//}
//void piplinecpu::MULTU(int& zero, s_word rs, s_word rt) {
//	word r1 = rs;
//	word r2 = rt;
//	uint64_t res = static_cast<uint64_t> (r1) * static_cast<uint64_t> (r2);
//	LO = static_cast<word> (res & 0x00000000FFFFFFFF);
//	HI = static_cast<word> ((res & 0xFFFFFFFF00000000) >> 32);
//	pc_increase(4);
//}
//
//
//void piplinecpu::SB(int& zero, s_word rs, s_word rt) {
//	s_word base = rs;
//	s_word offset = sign_extend_imi(inst);
//	word adr = base + offset;
//	word val = r[inst.src_t] & 0x000000FF;
//	
//	m.write_b(adr, val);
//	queue_mem.push(make_tuple("wb",adr, val));
//	pc_increase(4);
//}
//
//void piplinecpu::SH(int& zero, s_word rs, s_word rt) {
//	s_word base = rs;
//	s_word offset = sign_extend_imi(inst);
//	word adr = base + offset;
//	word val = rt & 0x0000FFFF;
//	//mem
//	m.write_h(adr, val);
//	queue_mem.push(make_tuple("wh",adr, val));
//}
//
//
















