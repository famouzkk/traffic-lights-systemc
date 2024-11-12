#include "testbench.h"
#include <iostream>
#include <string>
#include <systemc.h>
#include <termios.h>
#include <unistd.h>

int sc_main(int argc, char *argv[]) {
  Testbench tb("tb");
  sc_start();
  return 0;
}
