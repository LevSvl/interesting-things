// int foo(int a, int b, int c, int d,int e,\
//         int f, int g, int h, int i, int j)
// {
//   return a + 1;
// }

int bar(int x)
{
  return x;
}

int foo(int a)
{
  int b = 7;
  int c = 19;
  int d = 67;
  int x = bar(c);
  int y = 7;
  return a + 1;
}

int main(int argc, char const *argv[])
{
  int a = 3232;
  // int b = foo(a, 0, 1, 2, 3, 4, 4, 5, 7, 10);
  int b = foo(a);
  return 0;
}
