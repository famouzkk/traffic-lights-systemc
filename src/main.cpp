#include <systemc.h>
#include <iostream>
#include <string>
#include <unistd.h> 
#include <termios.h>

void set_nonblocking_terminal(bool enable) {
    static struct termios oldt, newt;
    if (enable) {
        tcgetattr(STDIN_FILENO, &oldt);          
        newt = oldt;
        newt.c_lflag &= ~(ICANON | ECHO);        
        tcsetattr(STDIN_FILENO, TCSANOW, &newt); 
    } else {
        tcsetattr(STDIN_FILENO, TCSANOW, &oldt); 
    }
}


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

    enum State {NS_GREEN, NS_RED, NS_ORANGE_TO_GREEN, NS_ORANGE_TO_RED,
								PRIVILIEGED_VEHICLE, FLASHING_ORANGE, INVALID};
    State current_state;

		bool prev_sw[6];

    SC_CTOR(TrafficLightController) {
        SC_METHOD(control_logic);
        sensitive << clk.pos(); // Trigger on positive edge of clock
        current_state = NS_ORANGE_TO_GREEN; // Initialize to NS_GREEN state
    }

		std::string get_state_name() {
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

		void change_state() {
				bool sw_NS_GREEN = sw[0].read();
				bool sw_NS_RED = sw[1].read();
				bool sw_NS_ORANGE_TO_GREEN = sw[2].read();
				bool sw_NS_ORANGE_TO_RED = sw[3].read();
				bool sw_FLASHING_ORANGE = sw[4].read();
				bool sw_PRIVILIEGED_VEHICLE = sw[5].read();

				bool current_sw[6] = {sw_NS_GREEN, sw_NS_RED, sw_NS_ORANGE_TO_GREEN, sw_NS_ORANGE_TO_RED, sw_FLASHING_ORANGE, sw_PRIVILIEGED_VEHICLE};

				// Check if any switch has changed
				bool switch_changed = false;
				for (int i = 0; i < 6; ++i) {
					if (current_sw[i] != prev_sw[i]) {
						switch_changed = true;
						break;
					}
				}

				if(!switch_changed) {
					return;
				}

				std::cout << "Current state: " << get_state_name() << std::endl;

				// Update previous switch states
				for (int i = 0; i < 6; ++i) {
					prev_sw[i] = current_sw[i];
				}

        // Determine next state based on switch inputs
				if(sw_PRIVILIEGED_VEHICLE) {
					current_state = PRIVILIEGED_VEHICLE;
					return;
				}

				if (sw_FLASHING_ORANGE) {
					current_state = FLASHING_ORANGE;
					return;
				}

				if (sw_NS_GREEN) {
					if (current_state != NS_ORANGE_TO_GREEN) {
						current_state = INVALID;
						return;
					}

					current_state = NS_GREEN;
					return;
				} 

				if (sw_NS_RED) {
					if (current_state != NS_ORANGE_TO_RED) {
						current_state = INVALID;
						return;
					}

					current_state = NS_RED;
					return;
				}

				if (sw_NS_ORANGE_TO_GREEN) {
					if (current_state != NS_RED) {
						current_state = INVALID;
						return;
					}

					current_state = NS_ORANGE_TO_GREEN;
					return;
				}

				if (sw_NS_ORANGE_TO_RED) {
					if (current_state != NS_GREEN) {
						current_state = INVALID;
						return;
					}

					current_state = NS_ORANGE_TO_RED;
					return;
				}
		}

		void display_state() {
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

    // Control Logic
    void control_logic() {
			change_state();
			display_state();
    }

    // State Functions
		void set_NS_green_state() {
				// Set NS Green, EW Red
				NS_Red_LED.write(false);
				NS_Orange_LED.write(false);
				NS_Green_LED.write(true);
				EW_Red_LED.write(true);
				EW_Orange_LED.write(false);
				EW_Green_LED.write(false);
				Emergency_LED.write(false);
		}

		void set_NS_red_state() {
				// Set NS Red, EW Green
				NS_Red_LED.write(true);
				NS_Orange_LED.write(false);
				NS_Green_LED.write(false);
				EW_Red_LED.write(false);
				EW_Orange_LED.write(false);
				EW_Green_LED.write(true);
				Emergency_LED.write(false);
		}				

		void set_NS_orange_to_green_state() {
				// Set NS Orange to Green, EW Red
				NS_Red_LED.write(true);
				NS_Orange_LED.write(true);
				NS_Green_LED.write(false);
				EW_Red_LED.write(false);
				EW_Orange_LED.write(true);
				EW_Green_LED.write(false);
				Emergency_LED.write(false);
		}

		void set_NS_orange_to_red_state() {
				// Set NS Orange to Red, EW Green
				NS_Red_LED.write(false);
				NS_Orange_LED.write(true);
				NS_Green_LED.write(false);
				EW_Red_LED.write(true);
				EW_Orange_LED.write(true);
				EW_Green_LED.write(false);
				Emergency_LED.write(false);
		}

		void set_privileged_vehicle_state() {
				// Set NS Red, EW Red
				NS_Red_LED.write(true);
				NS_Orange_LED.write(false);
				NS_Green_LED.write(false);
				EW_Red_LED.write(true);
				EW_Orange_LED.write(false);
				EW_Green_LED.write(false);
				Emergency_LED.write(false);
		}

		void set_flashing_orange_state() {
				// Set Flashing Orange
				NS_Red_LED.write(false);
				NS_Orange_LED.write(true);
				NS_Green_LED.write(false);
				EW_Red_LED.write(false);
				EW_Orange_LED.write(true);
				EW_Green_LED.write(false);
				Emergency_LED.write(false);
		}

    void trigger_emergency() {
        // Activate Emergency LED
        Emergency_LED.write(true);
    }
};

// =============================================================================
// Testbench Module with Keyboard Interaction
// =============================================================================
SC_MODULE(Testbench) {
    // Signals
    sc_signal<bool> sw_signals[6];
    sc_signal<bool> LED_signals[7]; // LEDs: NS_Red, NS_Orange, NS_Green, EW_Red, EW_Orange, EW_Green, Emergency
    sc_clock clk_signal; // Clock with default period 100ns

    // Instantiate the TrafficLightController
    TrafficLightController* controller;

    // Arrays to store current and previous LED states
    bool current_LED_states[7];
    bool prev_LED_states[7];

    // Constructor
    SC_CTOR(Testbench) : clk_signal("clk_signal", 100, SC_NS) { // Clock period 100ns
        // Instantiate the controller
        controller = new TrafficLightController("controller");

				// Valid initial state, NS Green, EW Red
				bool initial_state[6] = {0, 0, 0, 0, 0, 0};
				for (int i = 0; i < 6; ++i) {
						sw_signals[i].write(initial_state[i]);
						controller->sw[i](sw_signals[i]);
				}

        // Connect the clock
        controller->clk(clk_signal);

        // Connect the LEDs
        controller->NS_Red_LED(LED_signals[0]);
        controller->NS_Orange_LED(LED_signals[1]);
        controller->NS_Green_LED(LED_signals[2]);
        controller->EW_Red_LED(LED_signals[3]);
        controller->EW_Orange_LED(LED_signals[4]);
        controller->EW_Green_LED(LED_signals[5]);
        controller->Emergency_LED(LED_signals[6]);

        // Initialize previous LED states
        for (int i = 0; i < 7; ++i) {
            prev_LED_states[i] = LED_signals[i].read();
        }

        // Start the user interaction thread
        SC_THREAD(user_interaction);
    }

    // Destructor
    ~Testbench() {
        delete controller;
		}

    // User Interaction Process
    void user_interaction() {
        set_nonblocking_terminal(true); // Enable non-blocking input

        while (true) {
            wait(10, SC_MS); // Wait for 10 milliseconds

            // Read input character without blocking
            char ch = '\0';
            ssize_t bytes_read = read(STDIN_FILENO, &ch, 1);
            if (bytes_read > 0) {
                // Map keys to switch actions using a switch statement
                switch (ch) {
                    case '1':
                        sw_signals[0].write(true);
												break;
										case 'q':
										case 'Q':
												sw_signals[0].write(false);
												break;
										case '2':
												sw_signals[1].write(true);
												break;
										case 'w':
										case 'W':
												sw_signals[1].write(false);
												break;
										case '3':
												sw_signals[2].write(true);
												break;
										case 'e':
										case 'E':
												sw_signals[2].write(false);
												break;
										case '4':
												sw_signals[3].write(true);
												break;
										case 'r':
										case 'R':
												sw_signals[3].write(false);
												break;
										case '5':
												sw_signals[4].write(true);
												break;
										case 't':
										case 'T':
												sw_signals[4].write(false);
												break;
										case '6':
												sw_signals[5].write(true);
												break;
										case 'y':
										case 'Y':
												sw_signals[5].write(false);
												break;
										case 'x':
										case 'X':
												std::cout << "Quitting..." << std::endl;
												set_nonblocking_terminal(false); // Restore blocking input
												sc_stop(); // Stop the simulation
												break;
										default:
												break;
                }
            }

            // Update current LED states
            for (int i = 0; i < 7; ++i) {
                current_LED_states[i] = LED_signals[i].read();
            }

            // Check for LED state changes using dependency array
            bool led_changed = false;
            for (int i = 0; i < 7; ++i) {
                if (current_LED_states[i] != prev_LED_states[i]) {
                    led_changed = true;
                    break;
                }
            }

            // If LED states have changed, print the new states
               if (led_changed) {
								// Same as above, but with emojis too
								std::cout << "Current Simulation Time: " << sc_time_stamp() << std::endl;

								// leds shown visually, on all 4 sides
								// north
								std::cout << "\t" << (LED_signals[0].read() ? "🔴" : "⚪") << std::endl
													<< "\t" << (LED_signals[1].read() ? "🟠" : "⚪") << std::endl
													<< "\t" << (LED_signals[2].read() ? "🟢" : "⚪") << std::endl;

								// east and west
								std::cout << (LED_signals[3].read() ? "🔴" : "⚪") << "\t\t" << (LED_signals[3].read() ? "🔴" : "⚪") << std::endl
													<< (LED_signals[4].read() ? "🟠" : "⚪") << "\t\t" << (LED_signals[4].read() ? "🟠" : "⚪") << std::endl
													<< (LED_signals[5].read() ? "🟢" : "⚪") << "\t\t" << (LED_signals[5].read() ? "🟢" : "⚪") << std::endl;

								// south
								std::cout << "\t" << (LED_signals[0].read() ? "🔴" : "⚪") << std::endl
													<< "\t" << (LED_signals[1].read() ? "🟠" : "⚪") << std::endl
													<< "\t" << (LED_signals[2].read() ? "🟢" : "⚪") << std::endl;


								// debug view
								std::cout << "NS_Red_LED: " << (LED_signals[0].read() ? "🔴" : "⚪")
													<< ", NS_Orange_LED: " << (LED_signals[1].read() ? "🟠" : "⚪")
													<< ", NS_Green_LED: " << (LED_signals[2].read() ? "🟢" : "⚪") << std::endl;
								std::cout << "EW_Red_LED: " << (LED_signals[3].read() ? "🔴" : "⚪")
													<< ", EW_Orange_LED: " << (LED_signals[4].read() ? "🟠" : "⚪")
													<< ", EW_Green_LED: " << (LED_signals[5].read() ? "🟢" : "⚪") << std::endl;
								std::cout << "Emergency_LED: " << (LED_signals[6].read() ? "🚨" : "⚪") << std::endl;
								

                // Update previous LED states
                for (int i = 0; i < 7; ++i) {
                    prev_LED_states[i] = current_LED_states[i];
                }

								// Show current switch states
								std::cout << "Switch States: ";
								for (int i = 0; i < 6; ++i) {
									std::cout << (sw_signals[i].read() ? "1" : "0");
								}
								std::cout << std::endl << std::endl;
            }
        }
    }
};

// =============================================================================
// sc_main Function
// =============================================================================
int sc_main(int argc, char* argv[]) {
    // Instantiate the Testbench
    Testbench tb("tb");

    // Start the simulation
    sc_start();

    return 0;
}
