#pragma once

#include <systemc.h>
#include "intersection.h"

SC_MODULE(Testbench) {

  // Switch inputs (shared among all intersections)
  sc_signal<bool> sw_signals[6];

  // Each intersection’s 7 LED outputs
  sc_signal<bool> LED_signals[5][7];

  // We have 5 intersections
  Intersection* intersection[5];

  // Clock
  sc_clock clk_signal;

  // Which intersection is currently "viewed" by the console
  int selectedIntersection;

  // LED states for each intersection, for detecting changes
  bool current_LED_states[5][7];
  bool prev_LED_states[5][7];

  SC_CTOR(Testbench)
    : clk_signal("clk_signal", 100, SC_NS), selectedIntersection(0)
  {
    // Instantiate 5 intersections
    for (int i = 0; i < 5; i++) {
      char name[64];
      sprintf(name, "intersection_%d", i);
      intersection[i] = new Intersection(name);

      // Connect clock and shared switches
      intersection[i]->clk(clk_signal);
      for(int j = 0; j < 6; j++) {
        intersection[i]->sw[j](sw_signals[j]);
      }

      // Connect LED outputs
      intersection[i]->NS_Red_LED(    LED_signals[i][0]);
      intersection[i]->NS_Orange_LED( LED_signals[i][1]);
      intersection[i]->NS_Green_LED(  LED_signals[i][2]);
      intersection[i]->EW_Red_LED(    LED_signals[i][3]);
      intersection[i]->EW_Orange_LED( LED_signals[i][4]);
      intersection[i]->EW_Green_LED(  LED_signals[i][5]);
      intersection[i]->Emergency_LED( LED_signals[i][6]);

      // Initialize LED tracking to false
      for (int k = 0; k < 7; k++) {
        prev_LED_states[i][k] = false;
        current_LED_states[i][k] = false;
      }
    }

    // We'll handle user input & printing in a SystemC thread
    SC_THREAD(user_interaction);
  }

  ~Testbench() {
    for(int i = 0; i < 5; i++) {
      delete intersection[i];
    }
  }

  void user_interaction();
};