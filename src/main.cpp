#include "testbench.h"
#include <iostream>
#include <systemc.h>
#include <string>

int sc_main(int argc, char *argv[]) {
  Testbench tb("tb");
  sc_start();
  return 0;
}
