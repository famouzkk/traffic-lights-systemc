#include "traffic_light_module_1.h"
#include <iostream>

static const char* state_to_str[] = {
  "NS_GREEN", "NS_RED", "NS_ORANGE_TO_GREEN", "NS_ORANGE_TO_RED",
  "PRIVILIEGED_VEHICLE", "FLASHING_ORANGE", "INVALID"
};

void TrafficLightModule1::control_logic() {
  change_state();
  display_state();
}

void TrafficLightModule1::change_state() {
  bool sw_NS_GREEN           = sw[0].read();
  bool sw_NS_RED             = sw[1].read();
  bool sw_NS_ORANGE_TO_GREEN = sw[2].read();
  bool sw_NS_ORANGE_TO_RED   = sw[3].read();
  bool sw_FLASHING_ORANGE    = sw[4].read();
  bool sw_PRIVILIEGED_VEHICLE= sw[5].read();

  bool current_sw[6] = {
    sw_NS_GREEN, sw_NS_RED,
    sw_NS_ORANGE_TO_GREEN, sw_NS_ORANGE_TO_RED,
    sw_FLASHING_ORANGE, sw_PRIVILIEGED_VEHICLE
  };

  bool switch_changed = false;
  for(int i = 0; i < 6; i++){
    if(current_sw[i] != prev_sw[i]) {
      switch_changed = true;
      break;
    }
  }

  if(!switch_changed) {
    return;
  }

  std::cout << "[" << name() << "] " << "Poprzedni stan: " << state_to_str[current_state] << std::endl;

  for(int i = 0; i < 6; i++){
    prev_sw[i] = current_sw[i];
  }

  if (sw_PRIVILIEGED_VEHICLE) {
    current_state = PRIVILIEGED_VEHICLE;
  }

  if (sw_FLASHING_ORANGE) {
    current_state = FLASHING_ORANGE;
  }

  if (sw_NS_GREEN) {
    if (!canChangeFromState(NS_ORANGE_TO_GREEN)) {
      current_state = INVALID;
    } else {
      current_state = NS_GREEN;
    }
  }

  if (sw_NS_RED) {
    if (!canChangeFromState(NS_ORANGE_TO_RED)) {
      current_state = INVALID;
    } else {
      current_state = NS_RED;
    }
  }

  if (sw_NS_ORANGE_TO_GREEN) {
    if (!canChangeFromState(NS_RED)) {
      current_state = INVALID;
    } else {
      current_state = NS_ORANGE_TO_GREEN;
    }
  }

  if (sw_NS_ORANGE_TO_RED) {
    if (!canChangeFromState(NS_GREEN)) {
      current_state = INVALID;
    } else {
      current_state = NS_ORANGE_TO_RED;
    }
  }

  if (current_state == INVALID) {
    std::cout << "[" << name() << "] " << "INVALID STATE" << std::endl;
    out_fifo.write((int)INVALID);
  }
}

void TrafficLightModule1::display_state() {
  switch(current_state) {
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
      NS_Red_LED.write(false);
      NS_Orange_LED.write(false);
      NS_Green_LED.write(false);
      EW_Red_LED.write(false);
      EW_Orange_LED.write(false);
      EW_Green_LED.write(false);
      break;
  }
}


bool TrafficLightModule1::canChangeFromState(State from) {
  if(current_state == FLASHING_ORANGE || current_state == PRIVILIEGED_VEHICLE) {
    return true;
  }

  if(current_state == from) {
    return true;
  }

  return false;
}

void TrafficLightModule1::set_NS_green_state() {
  NS_Red_LED.write(false);
  NS_Orange_LED.write(false);
  NS_Green_LED.write(true);

  EW_Red_LED.write(true);
  EW_Orange_LED.write(false);
  EW_Green_LED.write(false);
}

void TrafficLightModule1::set_NS_red_state() {
  NS_Red_LED.write(true);
  NS_Orange_LED.write(false);
  NS_Green_LED.write(false);

  EW_Red_LED.write(false);
  EW_Orange_LED.write(false);
  EW_Green_LED.write(true);
}

void TrafficLightModule1::set_NS_orange_to_green_state() {
  NS_Red_LED.write(true);
  NS_Orange_LED.write(true);
  NS_Green_LED.write(false);

  EW_Red_LED.write(false);
  EW_Orange_LED.write(true);
  EW_Green_LED.write(false);
}

void TrafficLightModule1::set_NS_orange_to_red_state() {
  NS_Red_LED.write(false);
  NS_Orange_LED.write(true);
  NS_Green_LED.write(false);

  EW_Red_LED.write(true);
  EW_Orange_LED.write(true);
  EW_Green_LED.write(false);
}

void TrafficLightModule1::set_privileged_vehicle_state() {
  NS_Red_LED.write(true);
  NS_Orange_LED.write(false);
  NS_Green_LED.write(false);

  EW_Red_LED.write(true);
  EW_Orange_LED.write(false);
  EW_Green_LED.write(false);
}

void TrafficLightModule1::set_flashing_orange_state() {
  NS_Red_LED.write(false);
  NS_Orange_LED.write(true);
  NS_Green_LED.write(false);

  EW_Red_LED.write(false);
  EW_Orange_LED.write(true);
  EW_Green_LED.write(false);
}
