#include "TestClass.h"

TestClass::TestClass(void)
{
  test = 56;
}
int TestClass::run(void)
{
  std::cout << "RUNNING 'run'" << endl;
  return(test);
}

