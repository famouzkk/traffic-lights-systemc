#pragma once

#include <systemc.h>
#include "intersection.h"

// If you want multiple intersections, just store them in an array or vector.
// For simplicity, we'll do one intersection here.
SC_MODULE(Testbench) {
  // Switch inputs (shared for demonstration)
  sc_signal<bool> sw_signals[6];

  // LED outputs
  sc_signal<bool> LED_signals[7];

  // Clock
  sc_clock clk_signal;

  // Intersection (contains module1 + module2)
  Intersection* intersection;

  // LED states for user display
  bool current_LED_states[7];
  bool prev_LED_states[7];

  SC_CTOR(Testbench)
    : clk_signal("clk_signal", 100, SC_NS)
  {
    // Instantiate the Intersection
    intersection = new Intersection("intersection");
    intersection->clk(clk_signal);

    // Connect switch signals
    for(int i = 0; i < 6; ++i) {
      intersection->sw[i](sw_signals[i]);
    }

    // Connect LED outputs
    intersection->NS_Red_LED(LED_signals[0]);
    intersection->NS_Orange_LED(LED_signals[1]);
    intersection->NS_Green_LED(LED_signals[2]);
    intersection->EW_Red_LED(LED_signals[3]);
    intersection->EW_Orange_LED(LED_signals[4]);
    intersection->EW_Green_LED(LED_signals[5]);
    intersection->Emergency_LED(LED_signals[6]);

    // Initialize LED states for printing
    for (int i = 0; i < 7; ++i) {
      prev_LED_states[i] = false;
    }

    SC_THREAD(user_interaction);
  }

  void user_interaction();

  ~Testbench() {
    delete intersection;
  }
};