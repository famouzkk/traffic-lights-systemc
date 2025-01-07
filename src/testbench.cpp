#include "testbench.h"
#include <termios.h>
#include <unistd.h>
#include <iostream>
#include <string>

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

void Testbench::user_interaction() {
  set_nonblocking_terminal(true);

  while (true) {
    wait(10, SC_MS);

    bool pressed_something = true;

    char ch = '\0';
    ssize_t bytes_read = read(STDIN_FILENO, &ch, 1);
    if (bytes_read > 0) {
      pressed_something = true;
      switch (ch) {
        
        case '1':
          sw_signals[0].write(true);
          break;
        case 'q': case 'Q':
          sw_signals[0].write(false);
          break;

        case '2':
          sw_signals[1].write(true);
          break;
        case 'w': case 'W':
          sw_signals[1].write(false);
          break;

        case '3':
          sw_signals[2].write(true);
          break;
        case 'e': case 'E':
          sw_signals[2].write(false);
          break;

        case '4':
          sw_signals[3].write(true);
          break;
        case 'r': case 'R':
          sw_signals[3].write(false);
          break;

        case '5':
          sw_signals[4].write(true);
          break;
        case 't': case 'T':
          sw_signals[4].write(false);
          break;

        case '6':
          sw_signals[5].write(true);
          break;
        case 'y': case 'Y':
          sw_signals[5].write(false);
          break;

        
        case 'a': case 'A': selectedIntersection = 0; break;
        case 's': case 'S': selectedIntersection = 1; break;
        case 'd': case 'D': selectedIntersection = 2; break;
        case 'f': case 'F': selectedIntersection = 3; break;
        case 'g': case 'G': selectedIntersection = 4; break;

        case 'x':
        case 'X':
          std::cout << "Zamykanie..." << std::endl;
          set_nonblocking_terminal(false);
          sc_stop();
          return;

        default:
          break;
      }
    }

    
    bool led_changed = false;
    for (int j = 0; j < 7; j++) {
      current_LED_states[selectedIntersection][j] = 
        LED_signals[selectedIntersection][j].read();
      if (current_LED_states[selectedIntersection][j] != 
          prev_LED_states[selectedIntersection][j]) {
        led_changed = true;
      }
    }

    if (led_changed || pressed_something) {
      std::cout << "=== Wybrane skrzyżowanie #" 
                << selectedIntersection << " ===" << std::endl;
      std::cout << "Aktualny czas symulacji: " << sc_time_stamp() << std::endl;

      std::cout << "\t" << (LED_signals[selectedIntersection][0].read() ? "🔴" : "⚪") << std::endl
                << "\t" << (LED_signals[selectedIntersection][1].read() ? "🟠" : "⚪") << std::endl
                << "\t" << (LED_signals[selectedIntersection][2].read() ? "🟢" : "⚪") << std::endl;

      std::cout << (LED_signals[selectedIntersection][3].read() ? "🔴" : "⚪") << "\t\t"
                << (LED_signals[selectedIntersection][3].read() ? "🔴" : "⚪") << std::endl
                << (LED_signals[selectedIntersection][4].read() ? "🟠" : "⚪") << "\t\t"
                << (LED_signals[selectedIntersection][4].read() ? "🟠" : "⚪") << std::endl
                << (LED_signals[selectedIntersection][5].read() ? "🟢" : "⚪") << "\t\t"
                << (LED_signals[selectedIntersection][5].read() ? "🟢" : "⚪") << std::endl;

      std::cout << "\t" << (LED_signals[selectedIntersection][0].read() ? "🔴" : "⚪") << std::endl
                << "\t" << (LED_signals[selectedIntersection][1].read() ? "🟠" : "⚪") << std::endl
                << "\t" << (LED_signals[selectedIntersection][2].read() ? "🟢" : "⚪") << std::endl;

      std::cout << "NS_Red_LED: " << (LED_signals[selectedIntersection][0].read() ? "🔴" : "⚪")
                << ", NS_Orange_LED: " << (LED_signals[selectedIntersection][1].read() ? "🟠" : "⚪")
                << ", NS_Green_LED: " << (LED_signals[selectedIntersection][2].read() ? "🟢" : "⚪")
                << std::endl;
      std::cout << "EW_Red_LED: " << (LED_signals[selectedIntersection][3].read() ? "🔴" : "⚪")
                << ", EW_Orange_LED: " << (LED_signals[selectedIntersection][4].read() ? "🟠" : "⚪")
                << ", EW_Green_LED: " << (LED_signals[selectedIntersection][5].read() ? "🟢" : "⚪")
                << std::endl;
      std::cout << "Emergency_LED: " << (LED_signals[selectedIntersection][6].read() ? "🚨" : "⚪")
                << std::endl;

      
      for (int j = 0; j < 7; j++) {
        prev_LED_states[selectedIntersection][j] = 
          current_LED_states[selectedIntersection][j];
      }

      
      std::cout << "Stany przełączników: ";
      for (int i = 0; i < 6; i++) {
        std::cout << (sw_signals[i].read() ? "1" : "0");
      }
      std::cout << std::endl << std::endl;
    }
  }
}