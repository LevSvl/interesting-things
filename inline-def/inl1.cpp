#include "iostream"
#include "cstring"
#include "funcs.h"

extern void foo(), inl2_func();
extern int inl2_bar();

inline std::string test = "Hello";

inline void func(){
  std::cout << "func from inl1"<< std::endl;
}

int main(int argc, char const *argv[])
{
  std::cout << test;
  std::cout << " ";
  foo();

  std::cout << "bar from inl1: " << bar() << std::endl;
  std::cout << "bar from inl2: " << inl2_bar() << std::endl;
  
  // Компоновщик объединит все определения инлайн-функций 
  // для одного идентификатора в одно определени
  func();
  inl2_func();

  return 0;
}