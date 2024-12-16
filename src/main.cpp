#include "testbench.h"
#include <iostream>
#include <systemc.h>
#include <string>

int sc_main(int argc, char *argv[]) {
  // Instantiate the Testbench, which in turn instantiates our two modules
  Testbench tb("tb");

  // Start simulation (infinite until user presses 'x')
  sc_start();
  return 0;
}
