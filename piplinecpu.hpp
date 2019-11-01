#pragma once
#include"cpu.hpp"
#include<thread>
#include<queue>
#include <mutex>

typedef struct SIGN
{
	// EX Stage
	int RegDst = 0;
	int ALUOp = 0;
	int ALUSrc = 0;

	// MEM Stage
	int Brch = 0;
	int MemRead = 0;
	int MemWrite = 0;

	// WB Stage
	int RegWrite = 0;
	int MemtoReg = 0;
}SIGN;

typedef struct IFID
{
	// 64
	word instruction;// 32
	word pc;// 32
}IFID;

typedef struct IM {
	word im;//[15-0]immeadiate extent 16->32

	word rd;//[15-11]5
	word shamt;//[10-6]5
	word funct;//[5-0]6
}IM;

typedef struct IDEX
{
	// 128 + 10
	word rs;// register data1 32
	word rt;// register data2 32
	IM im;// immeadiate[15-0] 16->32 or rd-shamt-funct
	word pc;// 32
	word rd_b;// write back address1[20-16] 5
	word rt_b;// write back address2[15-11] 5

	// controller sign
	SIGN sign;
}IDEX;

typedef struct EXMEM
{
	// 96 + 1 + 5
	word jadd;//jump address 32
	word alures;// memory write address or register data 32
	word rt;// memory write data 32
	int zero;// 1
	word wb;// write register address 5 (rd_b or rt_b)

	// controller sign
	SIGN sign;
}EXMEM;

typedef struct MEMWB
{
	// 64 + 5
	word mdata; //memory data 32
	word alures;//register data 32
	word wb;// write register address 5 (rd_b or rt_b)

	// controller sign
	SIGN sign;
}MEMWB;


class piplinecpu :public cpu {
public: // change to private
	


	
public:
	piplinecpu();
	piplinecpu(std::string binary);

	void run1stage();
	void run5stage();
	//TEST:

public:
	long long cycle;
	long long ifcycle;
	void CycleControl();
	IFID IF(word pc);//IFetch
	IDEX ID(IFID ifid);//Idecode
	EXMEM EX(IDEX idex);//Execute
	MEMWB MEM(EXMEM exmem);//MemAccess
	void WB(MEMWB memwb);

	std::mutex mutcycle;
	std::mutex mutcycledone;
	std::condition_variable cvcycle;
	std::condition_variable cvcycledone;

	std::queue<word> queue_if;
	std::mutex mutif;
	//std::condition_variable cvcycle;

	//instruction rs rt
	std::queue<std::tuple<instruction, s_word, s_word>> queue_id;
	std::mutex mutid;
	//std::condition_variable cvid;

	std::queue<instruction> queue_ex;
	std::mutex mutex;
	//std::condition_variable cvex;
	//std::queue<word> queue_ex;

	std::queue<std::tuple<std::string,word,word>> queue_mem;
	std::mutex mutmem;
	//std::condition_variable cvmem;

	std::queue<word> queue_wb;
	
	std::mutex mutwb;
	//std::condition_variable cvwb;
	virtual word execute_r(int ALUOp, int& zero, IDEX idex);
	virtual word execute_i(int ALUOp, int& zero, IDEX idex);
	virtual void execute_j(const instruction& inst, s_word rs, s_word rt);

    bool is_ex=false;

	//INSTRUCTIONS
	//R
	word ADD   (int& zero, s_word rs, s_word rt);
	word ADDU  (int& zero, s_word rs, s_word rt);
	word SUB   (int& zero, s_word rs, s_word rt);
	word SUBU  (int& zero, s_word rs, s_word rt);
	word AND   (int& zero, s_word rs, s_word rt);
	word OR    (int& zero, s_word rs, s_word rt);
	word XOR   (int& zero, s_word rs, s_word rt);
	word SLT   (int& zero, s_word rs, s_word rt);
	word SLTU  (int& zero, s_word rs, s_word rt);

	word SLL   (int& zero, int shamt, s_word rt);
	word SRL   (int& zero, int shamt, s_word rt);
	word SRA   (int& zero, int shamt, s_word rt);
	
	word SLLV  (int& zero, s_word rs, s_word rt);
	word SRLV  (int& zero, s_word rs, s_word rt);
	word SRAV  (int& zero, s_word rs, s_word rt);


	// I
	word ADDI  (int& zero, s_word rs, int im);
	word ADDIU (int& zero, s_word rs, int im);
	word ANDI  (int& zero, s_word rs, int im);
	word ORI   (int& zero, s_word rs, int im);
	word XORI  (int& zero, s_word rs, int im);
	word LUI   (int& zero, s_word rs, int im);
	word LW    (int& zero, s_word rs, int im);
	word SW    (int& zero, s_word rs, int im);
	word BEQ   (int& zero, s_word rs, s_word rt);
	word BNE   (int& zero, s_word rs, s_word rt);
	word SLTI  (int& zero, s_word rs, int im);
	word SLTIU (int& zero, s_word rs, int im);


	// J
	/*word J(int& zero, s_word rs, s_word rt);
	word JAL(int& zero, s_word rs, s_word rt);

	word BGEZ  (int& zero, s_word rs, s_word rt);
	word BGEZAL(int& zero, s_word rs, s_word rt);
	word BGTZ  (int& zero, s_word rs, s_word rt);
	word BLEZ  (int& zero, s_word rs, s_word rt);
	word BLTZ  (int& zero, s_word rs, s_word rt);
	word BLTZAL(int& zero, s_word rs, s_word rt);
	
	word DIV   (int& zero, s_word rs, s_word rt);
	word DIVU  (int& zero, s_word rs, s_word rt);
	
	word JALR  (int& zero, s_word rs, s_word rt);
	
	word JR    (int& zero, s_word rs, s_word rt);
	word LB    (int& zero, s_word rs, s_word rt);
	word LBU   (int& zero, s_word rs, s_word rt);
	word LH    (int& zero, s_word rs, s_word rt);
	word LH_mem(word res);
	word LH_wb (int src_t, word res);

	EXMEM LHU(const instruction& inst, s_word rs, s_word rt);
	
	
	EXMEM LWL(const instruction& inst, s_word rs, s_word rt);
	EXMEM LWR(const instruction& inst, s_word rs, s_word rt);
	EXMEM MFHI(const instruction& inst, s_word rs, s_word rt);
	EXMEM MFLO(const instruction& inst, s_word rs, s_word rt);
	EXMEM MTHI(const instruction& inst, s_word rs, s_word rt);
	EXMEM MTLO(const instruction& inst, s_word rs, s_word rt);
	EXMEM MULT(const instruction& inst, s_word rs, s_word rt);
	EXMEM MULTU(const instruction& inst, s_word rs, s_word rt);
	
	
	EXMEM SB(const instruction& inst, s_word rs, s_word rt);
	EXMEM SH(const instruction& inst, s_word rs, s_word rt);*/
	
	
	
	
	
	
	
	
	
	
	
	

	bool is_finish;
};
