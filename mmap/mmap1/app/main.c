#include "init.h"

int main()
{
  mmap_init();

  app1();
  memcpy1();
  memcpy2();
  app2();

  all_clean();
}

