#pragma once
#include <systemc.h>

enum State {
  NS_RED = 0,
  NS_GREEN = 1,
  NS_ORANGE_TO_GREEN = 2,
  NS_ORANGE_TO_RED = 3,
  PRIVILIEGED_VEHICLE = 4,
  FLASHING_ORANGE = 5,
  INVALID = 6
};

SC_MODULE(TrafficLightModule1) {
  sc_in<bool> sw[6];
  sc_in<bool> clk;

  sc_out<bool> NS_Red_LED;
  sc_out<bool> NS_Orange_LED;
  sc_out<bool> NS_Green_LED;
  sc_out<bool> EW_Red_LED;
  sc_out<bool> EW_Orange_LED;
  sc_out<bool> EW_Green_LED;

  sc_fifo_out<int> out_fifo;

  State current_state;
  bool prev_sw[6];

  SC_CTOR(TrafficLightModule1) {
    SC_METHOD(control_logic);
    sensitive << clk.pos();
    current_state = NS_GREEN;

    for(int i = 0; i < 6; i++) {
      prev_sw[i] = false;
    }
  }

  void control_logic();
  void change_state();
  void display_state();

  bool canChangeFromState(State from);

  void set_NS_green_state();
  void set_NS_red_state();
  void set_NS_orange_to_green_state();
  void set_NS_orange_to_red_state();
  void set_privileged_vehicle_state();
  void set_flashing_orange_state();
};
