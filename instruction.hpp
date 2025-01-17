﻿#ifndef INSTRUCTION_HPP
#define INSTRUCTION_HPP
#include <iostream>
#include <cstdint>    //using
#include<bitset>
#include<string>
using word = uint32_t;

class instruction{
public:
  int opcode;
  int src_s, src_t, destn;//rs rt
  int shamt;//位移量
  int funct; //功能码
  int i_imi;
  int j_add;
  char type;
  std::bitset<32> bin;
  instruction();
  instruction(word inst);
//private:
  bool is_R_type(int opcode);
  bool is_I_type(int opcode);
  bool is_J_type(int opcode);
  bool is_LW(int opcode);
  bool is_SW(int opcode);
  bool is_BRANCH(int opcode);
};

std::ostream& operator<<(std::ostream&  os, const instruction& instr);


#endif
