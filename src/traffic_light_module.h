#include <systemc.h>

SC_MODULE(TrafficLightController) {
  sc_in<bool> sw[6];
  sc_in<bool> clk;

  sc_out<bool> NS_Red_LED;
  sc_out<bool> NS_Orange_LED;
  sc_out<bool> NS_Green_LED;
  sc_out<bool> EW_Red_LED;
  sc_out<bool> EW_Orange_LED;
  sc_out<bool> EW_Green_LED;
  sc_out<bool> Emergency_LED;

  enum State {
    NS_GREEN,
    NS_RED,
    NS_ORANGE_TO_GREEN,
    NS_ORANGE_TO_RED,
    PRIVILIEGED_VEHICLE,
    FLASHING_ORANGE,
    INVALID
  };
  State current_state;

  bool prev_sw[6];

  SC_CTOR(TrafficLightController) {
    SC_METHOD(control_logic);
    sensitive << clk.pos();
    current_state = NS_GREEN;
  }

  bool canChangeFromState(State from);

  std::string get_state_name();
  void change_state();
  void display_state();
  void control_logic();
  void set_NS_green_state();
  void set_NS_red_state();
  void set_NS_orange_to_green_state();
  void set_NS_orange_to_red_state();
  void set_privileged_vehicle_state();
  void set_flashing_orange_state();
  void trigger_emergency();
};