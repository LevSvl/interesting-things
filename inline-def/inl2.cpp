#include "iostream"
#include "cstring"
#include "funcs.h"

inline std::string test = "World";

void foo()
{
  std::cout << test << std::endl;
}

int inl2_bar()  {return bar();}

inline void func(){
  std::cout << "func from inl2"<< std::endl;
}

void inl2_func(){
  func();
}