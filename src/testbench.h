#pragma once
#include "traffic_light_module_1.h"
#include "traffic_light_module_2.h"
#include <systemc.h>

SC_MODULE(Testbench) {
  // Switch inputs (shared for demonstration)
  sc_signal<bool> sw_signals[6];

  // LED outputs (we’ll display them in user_interaction)
  sc_signal<bool> LED_signals[7];

  // Clock
  sc_clock clk_signal;

  // Modules
  TrafficLightModule1* module1;  // First module with n-1 states
  TrafficLightModule2* module2;  // Second module with n-th state

  // FIFO channel for communication
  sc_fifo<int> state_fifo;

  // LED states for user display
  bool current_LED_states[7];
  bool prev_LED_states[7];

  SC_CTOR(Testbench) 
    : clk_signal("clk_signal", 100, SC_NS),
      state_fifo("state_fifo", 8 /* FIFO depth */)
  {
    // Instantiate first module (contains n-1 states)
    module1 = new TrafficLightModule1("module1");
    module1->clk(clk_signal);
    for(int i = 0; i < 6; ++i) {
      module1->sw[i](sw_signals[i]);
    }
    // This module writes the "n-th" task trigger (INVALID state) into the FIFO
    module1->out_fifo(state_fifo);

    // Instantiate second module (contains n-th state)
    module2 = new TrafficLightModule2("module2");
    module2->clk(clk_signal);
    // The second module reads from the FIFO
    module2->in_fifo(state_fifo);

    // Connect second module’s LED output for the n-th task
    module2->Emergency_LED(LED_signals[6]); 

    // For demonstration, the first module still drives the standard traffic lights
    module1->NS_Red_LED(LED_signals[0]);
    module1->NS_Orange_LED(LED_signals[1]);
    module1->NS_Green_LED(LED_signals[2]);
    module1->EW_Red_LED(LED_signals[3]);
    module1->EW_Orange_LED(LED_signals[4]);
    module1->EW_Green_LED(LED_signals[5]);
    // The second module only drives the Emergency_LED (LED_signals[6]) above

    // Initialize LED states
    for (int i = 0; i < 7; ++i) {
      prev_LED_states[i] = LED_signals[i].read();
    }

    SC_THREAD(user_interaction);
  }

  void user_interaction();

  ~Testbench();
};
