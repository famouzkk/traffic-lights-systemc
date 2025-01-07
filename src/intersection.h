#pragma once

#include <systemc.h>
#include "traffic_light_module_1.h"
#include "traffic_light_module_2.h"

SC_MODULE(Intersection) {
  sc_in<bool> sw[6]; 
  sc_in<bool> clk;


  sc_out<bool> NS_Red_LED;
  sc_out<bool> NS_Orange_LED;
  sc_out<bool> NS_Green_LED;
  sc_out<bool> EW_Red_LED;
  sc_out<bool> EW_Orange_LED;
  sc_out<bool> EW_Green_LED;
  sc_out<bool> Emergency_LED;

  sc_fifo<int> state_fifo;

  TrafficLightModule1* module1;
  TrafficLightModule2* module2;

  SC_CTOR(Intersection) : state_fifo("state_fifo", 8) {
    module1 = new TrafficLightModule1("module1");
    module1->clk(clk);
    for (int i = 0; i < 6; i++) {
      module1->sw[i](sw[i]);
    }
    module1->out_fifo(state_fifo);

    module2 = new TrafficLightModule2("module2");
    module2->clk(clk);
    module2->in_fifo(state_fifo);
    module2->Emergency_LED(Emergency_LED);

    module1->NS_Red_LED(NS_Red_LED);
    module1->NS_Orange_LED(NS_Orange_LED);
    module1->NS_Green_LED(NS_Green_LED);
    module1->EW_Red_LED(EW_Red_LED);
    module1->EW_Orange_LED(EW_Orange_LED);
    module1->EW_Green_LED(EW_Green_LED);
  }
};