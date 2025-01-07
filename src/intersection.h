#pragma once

#include <systemc.h>
#include "traffic_light_module_1.h"
#include "traffic_light_module_2.h"

SC_MODULE(Intersection) {
  // External inputs
  sc_in<bool> sw[6];   // shared among all intersections
  sc_in<bool> clk;


  // Outputs for this intersection
  sc_out<bool> NS_Red_LED;
  sc_out<bool> NS_Orange_LED;
  sc_out<bool> NS_Green_LED;
  sc_out<bool> EW_Red_LED;
  sc_out<bool> EW_Orange_LED;
  sc_out<bool> EW_Green_LED;
  sc_out<bool> Emergency_LED;

  // Internal FIFO connecting module1 -> module2
  sc_fifo<int> state_fifo;

  // Two traffic light modules
  TrafficLightModule1* module1;
  TrafficLightModule2* module2;

  SC_CTOR(Intersection) : state_fifo("state_fifo", 8) {
    // Module 1
    module1 = new TrafficLightModule1("module1");
    module1->clk(clk);
    for (int i = 0; i < 6; i++) {
      module1->sw[i](sw[i]);
    }
    module1->out_fifo(state_fifo);

    // Module 2
    module2 = new TrafficLightModule2("module2");
    module2->clk(clk);
    module2->in_fifo(state_fifo);
    module2->Emergency_LED(Emergency_LED);

    // Hook up standard traffic lights to module1
    module1->NS_Red_LED(NS_Red_LED);
    module1->NS_Orange_LED(NS_Orange_LED);
    module1->NS_Green_LED(NS_Green_LED);
    module1->EW_Red_LED(EW_Red_LED);
    module1->EW_Orange_LED(EW_Orange_LED);
    module1->EW_Green_LED(EW_Green_LED);
  }
};