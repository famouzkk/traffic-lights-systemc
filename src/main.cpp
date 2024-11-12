#include <systemc.h>
#include <iostream>
#include <string>
#include <unistd.h>    // For read()
#include <termios.h>   // For terminal I/O control

// =============================================================================
// Terminal Settings for Non-blocking Input
// =============================================================================
void set_nonblocking_terminal(bool enable) {
    static struct termios oldt, newt;
    if (enable) {
        tcgetattr(STDIN_FILENO, &oldt);          // Save old terminal attributes
        newt = oldt;
        newt.c_lflag &= ~(ICANON | ECHO);        // Disable canonical mode and echo
        tcsetattr(STDIN_FILENO, TCSANOW, &newt); // Set new attributes
    } else {
        tcsetattr(STDIN_FILENO, TCSANOW, &oldt); // Restore old attributes
    }
}

// =============================================================================
// TrafficLightController Module
// =============================================================================
SC_MODULE(TrafficLightController) {
    // Inputs: Sliders (switches) and Clock
    sc_in<bool> sw[8];         // sw[0] to sw[7]
    sc_in<bool> clk;           // Clock signal

    // Outputs: LEDs
    sc_out<bool> NS_Red_LED;
    sc_out<bool> NS_Orange_LED;
    sc_out<bool> NS_Green_LED;
    sc_out<bool> EW_Red_LED;
    sc_out<bool> EW_Orange_LED;
    sc_out<bool> EW_Green_LED;
    sc_out<bool> Emergency_LED;

    // Internal Variables
    enum State {STATE_0, STATE_1, STATE_2, STATE_3, STATE_4, STATE_5, STATE_6, STATE_7};
    State current_state, next_state;

    // Constructor
    SC_CTOR(TrafficLightController) {
        SC_METHOD(control_logic);
        sensitive << clk.pos(); // Trigger on positive edge of clock
        current_state = STATE_0;
        next_state = STATE_0;
    }

    // Control Logic
    void control_logic() {
        // Check for Flashing Orange Mode
        if (sw[6].read()) {
            // Set all lights to flashing orange
            NS_Red_LED.write(false);
            NS_Orange_LED.write(true);
            NS_Green_LED.write(false);
            EW_Red_LED.write(false);
            EW_Orange_LED.write(true);
            EW_Green_LED.write(false);
            Emergency_LED.write(false);
            return;
        }

        // Check for Emergency Vehicle Mode
        if (sw[7].read()) {
            // Set all lights to red except the direction for the emergency vehicle
            // For simplicity, let's assume NS direction gets green
            NS_Red_LED.write(false);
            NS_Orange_LED.write(false);
            NS_Green_LED.write(true);
            EW_Red_LED.write(true);
            EW_Orange_LED.write(false);
            EW_Green_LED.write(false);
            Emergency_LED.write(false);
            return;
        }

        // Read slider inputs and determine new state
        bool NS_Red = sw[0].read();
        bool NS_Orange = sw[1].read();
        bool NS_Green = sw[2].read();
        bool EW_Red = sw[3].read();
        bool EW_Orange = sw[4].read();
        bool EW_Green = sw[5].read();

        // Map slider inputs to states
        // Create a unique code for each combination
        int input_code = (NS_Red << 5) | (NS_Orange << 4) | (NS_Green << 3)
                         | (EW_Red << 2) | (EW_Orange << 1) | (EW_Green << 0);

        // Determine the next state based on input_code and current_state
        switch (input_code) {
            case 0b100100: // NS: Red, EW: Red
                if (current_state == STATE_3)
                    next_state = STATE_4;
                else if (current_state == STATE_7)
                    next_state = STATE_0;
                else
                    next_state = STATE_0; // Default to State 0
                break;
            case 0b110100: // NS: Red+Orange, EW: Red
                next_state = STATE_1;
                break;
            case 0b001100: // NS: Green, EW: Red
                next_state = STATE_2;
                break;
            case 0b010100: // NS: Orange, EW: Red
                next_state = STATE_3;
                break;
            case 0b100110: // NS: Red, EW: Red+Orange
                next_state = STATE_5;
                break;
            case 0b100001: // NS: Red, EW: Green
                next_state = STATE_6;
                break;
            case 0b100010: // NS: Red, EW: Orange
                next_state = STATE_7;
                break;
            default:
                // Invalid combination, trigger emergency
                Emergency_LED.write(true);
                return;
        }

        // Validate the transition
        bool valid_transition = false;
        switch (current_state) {
            case STATE_0:
                if (next_state == STATE_1)
                    valid_transition = true;
                break;
            case STATE_1:
                if (next_state == STATE_2)
                    valid_transition = true;
                break;
            case STATE_2:
                if (next_state == STATE_3)
                    valid_transition = true;
                break;
            case STATE_3:
                if (next_state == STATE_4)
                    valid_transition = true;
                break;
            case STATE_4:
                if (next_state == STATE_5)
                    valid_transition = true;
                break;
            case STATE_5:
                if (next_state == STATE_6)
                    valid_transition = true;
                break;
            case STATE_6:
                if (next_state == STATE_7)
                    valid_transition = true;
                break;
            case STATE_7:
                if (next_state == STATE_0)
                    valid_transition = true;
                break;
            default:
                valid_transition = false;
        }

        if (!valid_transition) {
            // Invalid transition, trigger emergency
            Emergency_LED.write(true);
            return;
        } else {
            // Valid transition, update state and outputs
            current_state = next_state;
            Emergency_LED.write(false);

            // Set the outputs based on the current state
            switch (current_state) {
                case STATE_0: // All Red
                    NS_Red_LED.write(true);
                    NS_Orange_LED.write(false);
                    NS_Green_LED.write(false);
                    EW_Red_LED.write(true);
                    EW_Orange_LED.write(false);
                    EW_Green_LED.write(false);
                    break;
                case STATE_1: // NS Red+Orange, EW Red
                    NS_Red_LED.write(true);
                    NS_Orange_LED.write(true);
                    NS_Green_LED.write(false);
                    EW_Red_LED.write(true);
                    EW_Orange_LED.write(false);
                    EW_Green_LED.write(false);
                    break;
                case STATE_2: // NS Green, EW Red
                    NS_Red_LED.write(false);
                    NS_Orange_LED.write(false);
                    NS_Green_LED.write(true);
                    EW_Red_LED.write(true);
                    EW_Orange_LED.write(false);
                    EW_Green_LED.write(false);
                    break;
                case STATE_3: // NS Orange, EW Red
                    NS_Red_LED.write(false);
                    NS_Orange_LED.write(true);
                    NS_Green_LED.write(false);
                    EW_Red_LED.write(true);
                    EW_Orange_LED.write(false);
                    EW_Green_LED.write(false);
                    break;
                case STATE_4: // All Red
                    NS_Red_LED.write(true);
                    NS_Orange_LED.write(false);
                    NS_Green_LED.write(false);
                    EW_Red_LED.write(true);
                    EW_Orange_LED.write(false);
                    EW_Green_LED.write(false);
                    break;
                case STATE_5: // NS Red, EW Red+Orange
                    NS_Red_LED.write(true);
                    NS_Orange_LED.write(false);
                    NS_Green_LED.write(false);
                    EW_Red_LED.write(true);
                    EW_Orange_LED.write(true);
                    EW_Green_LED.write(false);
                    break;
                case STATE_6: // NS Red, EW Green
                    NS_Red_LED.write(true);
                    NS_Orange_LED.write(false);
                    NS_Green_LED.write(false);
                    EW_Red_LED.write(false);
                    EW_Orange_LED.write(false);
                    EW_Green_LED.write(true);
                    break;
                case STATE_7: // NS Red, EW Orange
                    NS_Red_LED.write(true);
                    NS_Orange_LED.write(false);
                    NS_Green_LED.write(false);
                    EW_Red_LED.write(false);
                    EW_Orange_LED.write(true);
                    EW_Green_LED.write(false);
                    break;
                default:
                    // Should not reach here
                    break;
            }
        }
    }
};

// =============================================================================
// Testbench Module with Keyboard Interaction
// =============================================================================
SC_MODULE(Testbench) {
    // Signals
    sc_signal<bool> sw_signals[8];
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

        // Connect the switches
        for (int i = 0; i < 8; ++i) {
            controller->sw[i](sw_signals[i]);
            sw_signals[i].write(false); // Initialize switches to 0
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
                    case '2':
                        sw_signals[1].write(true);
                        break;
                    case '3':
                        sw_signals[2].write(true);
                        break;
                    case '4':
                        sw_signals[3].write(true);
                        break;
                    case '5':
                        sw_signals[4].write(true);
                        break;
                    case '6':
                        sw_signals[5].write(true);
                        break;
                    case '7':
                        sw_signals[6].write(true); // Flashing Orange Mode
                        break;
                    case '8':
                        sw_signals[7].write(true); // Emergency Vehicle Mode
                        break;
                    case 'q':
                    case 'Q':
                        sw_signals[0].write(false);
                        break;
                    case 'w':
                    case 'W':
                        sw_signals[1].write(false);
                        break;
                    case 'e':
                    case 'E':
                        sw_signals[2].write(false);
                        break;
                    case 'r':
                    case 'R':
                        sw_signals[3].write(false);
                        break;
                    case 't':
                    case 'T':
                        sw_signals[4].write(false);
                        break;
                    case 'y':
                    case 'Y':
                        sw_signals[5].write(false);
                        break;
                    case 'u':
                    case 'U':
                        sw_signals[6].write(false); // Disable Flashing Orange Mode
                        break;
                    case 'i':
                    case 'I':
                        sw_signals[7].write(false); // Disable Emergency Vehicle Mode
                        break;
                    case 'x':
                    case 'X':
                        // Exit the simulation
                        std::cout << "Exiting simulation." << std::endl;
                        set_nonblocking_terminal(false); // Restore terminal settings
                        sc_stop();
                        return;
                    default:
                        // Do nothing for unrecognized characters
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
                std::cout << "Current Simulation Time: " << sc_time_stamp() << std::endl;
                std::cout << "NS_Red_LED: " << LED_signals[0].read()
                          << ", NS_Orange_LED: " << LED_signals[1].read()
                          << ", NS_Green_LED: " << LED_signals[2].read() << std::endl;
                std::cout << "EW_Red_LED: " << LED_signals[3].read()
                          << ", EW_Orange_LED: " << LED_signals[4].read()
                          << ", EW_Green_LED: " << LED_signals[5].read() << std::endl;
                std::cout << "Emergency_LED: " << LED_signals[6].read() << std::endl;

                // Update previous LED states
                for (int i = 0; i < 7; ++i) {
                    prev_LED_states[i] = current_LED_states[i];
                }
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
