#include "testbench.h"
#include <iostream>
#include <string>
#include <systemc.h>
#include <termios.h>
#include <unistd.h>

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

Testbench::~Testbench() {
  delete module1;
  delete module2;
}


void Testbench::user_interaction() {
  set_nonblocking_terminal(true);

  while (true) {
    wait(10, SC_MS);

    char ch = '\0';
    ssize_t bytes_read = read(STDIN_FILENO, &ch, 1);
    if (bytes_read > 0) {
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
        std::cout << "Zamykanie..." << std::endl;
        set_nonblocking_terminal(false);
        sc_stop();
        break;
      default:
        break;
      }
    }

    for (int i = 0; i < 7; ++i) {
      current_LED_states[i] = LED_signals[i].read();
    }

    bool led_changed = false;
    for (int i = 0; i < 7; ++i) {
      if (current_LED_states[i] != prev_LED_states[i]) {
        led_changed = true;
        break;
      }
    }

    if (led_changed) {
      std::cout << "Aktualny czas symulacji: " << sc_time_stamp() << std::endl;

      std::cout << "\t" << (LED_signals[0].read() ? "🔴" : "⚪") << std::endl
                << "\t" << (LED_signals[1].read() ? "🟠" : "⚪") << std::endl
                << "\t" << (LED_signals[2].read() ? "🟢" : "⚪") << std::endl;

      std::cout << (LED_signals[3].read() ? "🔴" : "⚪") << "\t\t"
                << (LED_signals[3].read() ? "🔴" : "⚪") << std::endl
                << (LED_signals[4].read() ? "🟠" : "⚪") << "\t\t"
                << (LED_signals[4].read() ? "🟠" : "⚪") << std::endl
                << (LED_signals[5].read() ? "🟢" : "⚪") << "\t\t"
                << (LED_signals[5].read() ? "🟢" : "⚪") << std::endl;

      std::cout << "\t" << (LED_signals[0].read() ? "🔴" : "⚪") << std::endl
                << "\t" << (LED_signals[1].read() ? "🟠" : "⚪") << std::endl
                << "\t" << (LED_signals[2].read() ? "🟢" : "⚪") << std::endl;

      std::cout << "NS_Red_LED: " << (LED_signals[0].read() ? "🔴" : "⚪")
                << ", NS_Orange_LED: " << (LED_signals[1].read() ? "🟠" : "⚪")
                << ", NS_Green_LED: " << (LED_signals[2].read() ? "🟢" : "⚪")
                << std::endl;
      std::cout << "EW_Red_LED: " << (LED_signals[3].read() ? "🔴" : "⚪")
                << ", EW_Orange_LED: " << (LED_signals[4].read() ? "🟠" : "⚪")
                << ", EW_Green_LED: " << (LED_signals[5].read() ? "🟢" : "⚪")
                << std::endl;
      std::cout << "Emergency_LED: " << (LED_signals[6].read() ? "🚨" : "⚪")
                << std::endl;

      for (int i = 0; i < 7; ++i) {
        prev_LED_states[i] = current_LED_states[i];
      }

      std::cout << "Stany przełączników: ";
      for (int i = 0; i < 6; ++i) {
        std::cout << (sw_signals[i].read() ? "1" : "0");
      }
      std::cout << std::endl << std::endl;
    }
  }
}