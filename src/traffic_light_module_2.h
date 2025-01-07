#pragma once
#include <systemc.h>

SC_MODULE(TrafficLightModule2) {
  sc_in<bool> clk;

  sc_fifo_in<int> in_fifo;

  sc_out<bool> Emergency_LED;

  SC_CTOR(TrafficLightModule2) {
    SC_THREAD(main_process);
    sensitive << clk.pos();
    dont_initialize();
  }

  void main_process();

  void trigger_emergency();
};
