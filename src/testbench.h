#pragma once
#include "traffic_light_module.h"
#include <systemc.h>

SC_MODULE(Testbench) {
  sc_signal<bool> sw_signals[6];
  sc_signal<bool> LED_signals[7];
  sc_clock clk_signal;

  TrafficLightController *controller;

  bool current_LED_states[7];
  bool prev_LED_states[7];

  SC_CTOR(Testbench) : clk_signal("clk_signal", 100, SC_NS) {
    controller = new TrafficLightController("controller");

    bool initial_state[6] = {0, 0, 0, 0, 0, 0};
    for (int i = 0; i < 6; ++i) {
      sw_signals[i].write(initial_state[i]);
      controller->sw[i](sw_signals[i]);
    }

    controller->clk(clk_signal);

    controller->NS_Red_LED(LED_signals[0]);
    controller->NS_Orange_LED(LED_signals[1]);
    controller->NS_Green_LED(LED_signals[2]);
    controller->EW_Red_LED(LED_signals[3]);
    controller->EW_Orange_LED(LED_signals[4]);
    controller->EW_Green_LED(LED_signals[5]);
    controller->Emergency_LED(LED_signals[6]);

    for (int i = 0; i < 7; ++i) {
      prev_LED_states[i] = LED_signals[i].read();
    }

    SC_THREAD(user_interaction);
  }

  void user_interaction();

  ~Testbench();
};