#include "traffic_light_module.h"
#include <iostream>
#include <string>
#include <systemc.h>
#include <termios.h>
#include <unistd.h>

std::string TrafficLightController::get_state_name() {
  std::string state_name;
  switch (current_state) {
  case NS_GREEN:
    state_name = "NS_GREEN";
    break;
  case NS_RED:
    state_name = "NS_RED";
    break;
  case NS_ORANGE_TO_GREEN:
    state_name = "NS_ORANGE_TO_GREEN";
    break;
  case NS_ORANGE_TO_RED:
    state_name = "NS_ORANGE_TO_RED";
    break;
  case PRIVILIEGED_VEHICLE:
    state_name = "PRIVILIEGED_VEHICLE";
    break;
  case FLASHING_ORANGE:
    state_name = "FLASHING_ORANGE";
    break;
  case INVALID:
    state_name = "INVALID";
    break;
  }

  return state_name;
}

bool TrafficLightController::canChangeFromState(State from) {
  if(current_state == FLASHING_ORANGE || current_state == PRIVILIEGED_VEHICLE) {
    return true;
  }

  if(current_state == from) {
    return true;
  }

  return false;
}
  

void TrafficLightController::change_state() {
  bool sw_NS_GREEN = sw[0].read();
  bool sw_NS_RED = sw[1].read();
  bool sw_NS_ORANGE_TO_GREEN = sw[2].read();
  bool sw_NS_ORANGE_TO_RED = sw[3].read();
  bool sw_FLASHING_ORANGE = sw[4].read();
  bool sw_PRIVILIEGED_VEHICLE = sw[5].read();

  bool current_sw[6] = {sw_NS_GREEN,           sw_NS_RED,
                        sw_NS_ORANGE_TO_GREEN, sw_NS_ORANGE_TO_RED,
                        sw_FLASHING_ORANGE,    sw_PRIVILIEGED_VEHICLE};

  bool switch_changed = false;
  for (int i = 0; i < 6; ++i) {
    if (current_sw[i] != prev_sw[i]) {
      switch_changed = true;
      break;
    }
  }

  if (!switch_changed) {
    return;
  }

  std::cout << "Aktualny stan: " << get_state_name() << std::endl;

  for (int i = 0; i < 6; ++i) {
    prev_sw[i] = current_sw[i];
  }

  if (sw_PRIVILIEGED_VEHICLE) {
    current_state = PRIVILIEGED_VEHICLE;
    return;
  }

  if (sw_FLASHING_ORANGE) {
    current_state = FLASHING_ORANGE;
    return;
  }

  if (sw_NS_GREEN) {
    if (!canChangeFromState(NS_ORANGE_TO_GREEN)) {
      current_state = INVALID;
      return;
    }
    current_state = NS_GREEN;
    return;
  }

  if (sw_NS_RED) {
    if (!canChangeFromState(NS_ORANGE_TO_RED)) {
      current_state = INVALID;
      return;
    }
    current_state = NS_RED;
    return;
  }

  if (sw_NS_ORANGE_TO_GREEN) {
    if (!canChangeFromState(NS_RED)) {
      current_state = INVALID;
      return;
    }
    current_state = NS_ORANGE_TO_GREEN;
    return;
  }

  if (sw_NS_ORANGE_TO_RED) {
    if (!canChangeFromState(NS_GREEN)) {
      current_state = INVALID;
      return;
    }
    current_state = NS_ORANGE_TO_RED;
    return;
  }
}

void TrafficLightController::display_state() {
  switch (current_state) {
  case NS_GREEN:
    set_NS_green_state();
    break;
  case NS_RED:
    set_NS_red_state();
    break;
  case NS_ORANGE_TO_GREEN:
    set_NS_orange_to_green_state();
    break;
  case NS_ORANGE_TO_RED:
    set_NS_orange_to_red_state();
    break;
  case PRIVILIEGED_VEHICLE:
    set_privileged_vehicle_state();
    break;
  case FLASHING_ORANGE:
    set_flashing_orange_state();
    break;
  case INVALID:
    trigger_emergency();
    break;
  }
}

void TrafficLightController::control_logic() {
  change_state();
  display_state();
}

void TrafficLightController::set_NS_green_state() {
  NS_Red_LED.write(false);
  NS_Orange_LED.write(false);
  NS_Green_LED.write(true);
  EW_Red_LED.write(true);
  EW_Orange_LED.write(false);
  EW_Green_LED.write(false);
  Emergency_LED.write(false);
}

void TrafficLightController::set_NS_red_state() {
  NS_Red_LED.write(true);
  NS_Orange_LED.write(false);
  NS_Green_LED.write(false);
  EW_Red_LED.write(false);
  EW_Orange_LED.write(false);
  EW_Green_LED.write(true);
  Emergency_LED.write(false);
}

void TrafficLightController::set_NS_orange_to_green_state() {
  NS_Red_LED.write(true);
  NS_Orange_LED.write(true);
  NS_Green_LED.write(false);
  EW_Red_LED.write(false);
  EW_Orange_LED.write(true);
  EW_Green_LED.write(false);
  Emergency_LED.write(false);
}

void TrafficLightController::set_NS_orange_to_red_state() {
  NS_Red_LED.write(false);
  NS_Orange_LED.write(true);
  NS_Green_LED.write(false);
  EW_Red_LED.write(true);
  EW_Orange_LED.write(true);
  EW_Green_LED.write(false);
  Emergency_LED.write(false);
}

void TrafficLightController::set_privileged_vehicle_state() {
  NS_Red_LED.write(true);
  NS_Orange_LED.write(false);
  NS_Green_LED.write(false);
  EW_Red_LED.write(true);
  EW_Orange_LED.write(false);
  EW_Green_LED.write(false);
  Emergency_LED.write(false);
}

void TrafficLightController::set_flashing_orange_state() {
  NS_Red_LED.write(false);
  NS_Orange_LED.write(true);
  NS_Green_LED.write(false);
  EW_Red_LED.write(false);
  EW_Orange_LED.write(true);
  EW_Green_LED.write(false);
  Emergency_LED.write(false);
}

void TrafficLightController::trigger_emergency() { Emergency_LED.write(true); }