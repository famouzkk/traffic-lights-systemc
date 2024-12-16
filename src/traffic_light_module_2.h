#pragma once
#include <systemc.h>

// Second module, responsible for the n-th task: handling INVALID state (or emergency)
SC_MODULE(TrafficLightModule2) {
  sc_in<bool> clk;

  // Reads from FIFO (the moment an INVALID state is triggered)
  sc_fifo_in<int> in_fifo;

  // The n-th task output (emergency LED)
  sc_out<bool> Emergency_LED;

  SC_CTOR(TrafficLightModule2) {
    SC_THREAD(main_process);
    sensitive << clk.pos();
    dont_initialize();
  }

  void main_process();

  void trigger_emergency();
};
