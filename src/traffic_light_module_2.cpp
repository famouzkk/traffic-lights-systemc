#include "traffic_light_module_2.h"
#include "traffic_light_module_1.h"
#include <iostream>

void TrafficLightModule2::main_process() {
  while(true) {
    int incoming_state = in_fifo.read(); // blocking read
    printf("[MODULE2] Received state: %d\n", incoming_state);
    
    if(incoming_state == INVALID) { // 'INVALID' index
      std::cout << "[MODULE2] Detected INVALID state -> triggering emergency" << std::endl;
      trigger_emergency();
    }
  }
}

void TrafficLightModule2::trigger_emergency() {
  Emergency_LED.write(true);
}