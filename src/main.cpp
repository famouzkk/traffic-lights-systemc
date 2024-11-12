#include <systemc.h>

// =============================================================================
// Moduł Kontrolera Świateł Drogowych
// =============================================================================
SC_MODULE(TrafficLightController) {
    // Wejścia: Przełączniki (slidery) oraz Zegar
    sc_in<bool> sw[8];         // sw[0] do sw[7]
    sc_in_clk clk;             // Sygnał zegara

    // Wyjścia: Diody LED
    sc_out<bool> NS_Red_LED;
    sc_out<bool> NS_Orange_LED;
    sc_out<bool> NS_Green_LED;
    sc_out<bool> EW_Red_LED;
    sc_out<bool> EW_Orange_LED;
    sc_out<bool> EW_Green_LED;
    sc_out<bool> Emergency_LED;

    // Zmienne wewnętrzne
    enum State {STATE_0, STATE_1, STATE_2, STATE_3, STATE_4, STATE_5, STATE_6, STATE_7};
    State current_state, next_state;

    // Konstruktor
    SC_CTOR(TrafficLightController) {
        SC_METHOD(control_logic);
        sensitive << clk.pos(); // Wyzwalane na dodatnią krawędź zegara
        current_state = STATE_0;
        next_state = STATE_0;
    }

    // Logika Kontrolera
    void control_logic() {
        // Sprawdzenie trybu Migających Pomarańczowych Świateł
        if (sw[6].read()) {
            // Ustawienie wszystkich świateł na migające pomarańczowe
            NS_Red_LED.write(false);
            NS_Orange_LED.write(true); // Zakładając, że miganie jest obsługiwane zewnętrznie
            NS_Green_LED.write(false);
            EW_Red_LED.write(false);
            EW_Orange_LED.write(true); // Zakładając, że miganie jest obsługiwane zewnętrznie
            EW_Green_LED.write(false);
            Emergency_LED.write(false);
            return;
        }

        // Sprawdzenie trybu Pojazdów Ratunkowych
        if (sw[7].read()) {
            // Ustawienie wszystkich świateł na czerwone z wyjątkiem kierunku pojazdu ratunkowego
            // Dla uproszczenia zakładamy, że kierunek Północ-Południe (NS) otrzymuje zielone światło
            NS_Red_LED.write(false);
            NS_Orange_LED.write(false);
            NS_Green_LED.write(true);
            EW_Red_LED.write(true);
            EW_Orange_LED.write(false);
            EW_Green_LED.write(false);
            Emergency_LED.write(false);
            return;
        }

        // Odczytanie stanu przełączników i określenie nowego stanu
        bool NS_Red = sw[0].read();
        bool NS_Orange = sw[1].read();
        bool NS_Green = sw[2].read();
        bool EW_Red = sw[3].read();
        bool EW_Orange = sw[4].read();
        bool EW_Green = sw[5].read();

        // Mapowanie wejść przełączników na unikalny kod
        int input_code = (NS_Red << 5) | (NS_Orange << 4) | (NS_Green << 3)
                         | (EW_Red << 2) | (EW_Orange << 1) | (EW_Green << 0);

        // Określenie następnego stanu na podstawie kodu wejściowego i bieżącego stanu
        switch (input_code) {
            case 0b100100: // NS: Czerwone, EW: Czerwone
                if (current_state == STATE_3)
                    next_state = STATE_4;
                else if (current_state == STATE_7)
                    next_state = STATE_0;
                else
                    next_state = STATE_0; // Domyślnie do Stan 0
                break;
            case 0b110100: // NS: Czerwone + Pomarańczowe, EW: Czerwone
                next_state = STATE_1;
                break;
            case 0b001100: // NS: Zielone, EW: Czerwone
                next_state = STATE_2;
                break;
            case 0b010100: // NS: Pomarańczowe, EW: Czerwone
                next_state = STATE_3;
                break;
            case 0b100110: // NS: Czerwone, EW: Czerwone + Pomarańczowe
                next_state = STATE_5;
                break;
            case 0b100001: // NS: Czerwone, EW: Zielone
                next_state = STATE_6;
                break;
            case 0b100010: // NS: Czerwone, EW: Pomarańczowe
                next_state = STATE_7;
                break;
            default:
                // Nieprawidłowa kombinacja, aktywacja alarmu
                Emergency_LED.write(true);
                return;
        }

        // Walidacja przejścia między stanami
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
            // Nieprawidłowe przejście, aktywacja alarmu
            Emergency_LED.write(true);
            return;
        } else {
            // Prawidłowe przejście, aktualizacja stanu i wyjść
            current_state = next_state;
            Emergency_LED.write(false);

            // Ustawienie wyjść na podstawie bieżącego stanu
            switch (current_state) {
                case STATE_0: // Wszystkie Czerwone
                    NS_Red_LED.write(true);
                    NS_Orange_LED.write(false);
                    NS_Green_LED.write(false);
                    EW_Red_LED.write(true);
                    EW_Orange_LED.write(false);
                    EW_Green_LED.write(false);
                    break;
                case STATE_1: // NS Czerwone + Pomarańczowe, EW Czerwone
                    NS_Red_LED.write(true);
                    NS_Orange_LED.write(true);
                    NS_Green_LED.write(false);
                    EW_Red_LED.write(true);
                    EW_Orange_LED.write(false);
                    EW_Green_LED.write(false);
                    break;
                case STATE_2: // NS Zielone, EW Czerwone
                    NS_Red_LED.write(false);
                    NS_Orange_LED.write(false);
                    NS_Green_LED.write(true);
                    EW_Red_LED.write(true);
                    EW_Orange_LED.write(false);
                    EW_Green_LED.write(false);
                    break;
                case STATE_3: // NS Pomarańczowe, EW Czerwone
                    NS_Red_LED.write(false);
                    NS_Orange_LED.write(true);
                    NS_Green_LED.write(false);
                    EW_Red_LED.write(true);
                    EW_Orange_LED.write(false);
                    EW_Green_LED.write(false);
                    break;
                case STATE_4: // Wszystkie Czerwone
                    NS_Red_LED.write(true);
                    NS_Orange_LED.write(false);
                    NS_Green_LED.write(false);
                    EW_Red_LED.write(true);
                    EW_Orange_LED.write(false);
                    EW_Green_LED.write(false);
                    break;
                case STATE_5: // NS Czerwone, EW Czerwone + Pomarańczowe
                    NS_Red_LED.write(true);
                    NS_Orange_LED.write(false);
                    NS_Green_LED.write(false);
                    EW_Red_LED.write(true);
                    EW_Orange_LED.write(true);
                    EW_Green_LED.write(false);
                    break;
                case STATE_6: // NS Czerwone, EW Zielone
                    NS_Red_LED.write(true);
                    NS_Orange_LED.write(false);
                    NS_Green_LED.write(false);
                    EW_Red_LED.write(false);
                    EW_Orange_LED.write(false);
                    EW_Green_LED.write(true);
                    break;
                case STATE_7: // NS Czerwone, EW Pomarańczowe
                    NS_Red_LED.write(true);
                    NS_Orange_LED.write(false);
                    NS_Green_LED.write(false);
                    EW_Red_LED.write(false);
                    EW_Orange_LED.write(true);
                    EW_Green_LED.write(false);
                    break;
                default:
                    // Nie powinno się tutaj znaleźć
                    break;
            }
        }
    }
};

// =============================================================================
// Moduł Testbench
// =============================================================================
SC_MODULE(Testbench) {
    // Sygnały
    sc_signal<bool> sw_signals[8];
    sc_signal<bool> NS_Red_LED;
    sc_signal<bool> NS_Orange_LED;
    sc_signal<bool> NS_Green_LED;
    sc_signal<bool> EW_Red_LED;
    sc_signal<bool> EW_Orange_LED;
    sc_signal<bool> EW_Green_LED;
    sc_signal<bool> Emergency_LED;
    sc_clock clk_signal; // Zegar o domyślnym okresie 10ns

    // Instancja Kontrolera Świateł Drogowych
    TrafficLightController* controller;

    // Proces Sekwencji Testowej
    void test_sequence() {
        // Inicjalizacja przełączników do Stanu 0
        sw_signals[0].write(true);  // NS Czerwone
        sw_signals[1].write(false); // NS Pomarańczowe
        sw_signals[2].write(false); // NS Zielone
        sw_signals[3].write(true);  // EW Czerwone
        sw_signals[4].write(false); // EW Pomarańczowe
        sw_signals[5].write(false); // EW Zielone
        sw_signals[6].write(false); // Tryb Migających Pomarańczowych
        sw_signals[7].write(false); // Tryb Pojazdów Ratunkowych

        wait(10, SC_NS); // Oczekiwanie 10 ns

        // Przejście do Stanu 1: NS Czerwone + Pomarańczowe, EW Czerwone
        sw_signals[1].write(true); // NS Pomarańczowe
        wait(10, SC_NS);

        // Przejście do Stanu 2: NS Zielone, EW Czerwone
        sw_signals[0].write(false); // NS Czerwone wyłączone
        sw_signals[1].write(false); // NS Pomarańczowe wyłączone
        sw_signals[2].write(true);  // NS Zielone włączone
        wait(10, SC_NS);

        // Przejście do Stanu 3: NS Pomarańczowe, EW Czerwone
        sw_signals[2].write(false); // NS Zielone wyłączone
        sw_signals[1].write(true);  // NS Pomarańczowe włączone
        wait(10, SC_NS);

        // Przejście do Stanu 4: Wszystkie Czerwone
        sw_signals[1].write(false); // NS Pomarańczowe wyłączone
        sw_signals[0].write(true);  // NS Czerwone włączone
        wait(10, SC_NS);

        // Przejście do Stanu 5: NS Czerwone, EW Czerwone + Pomarańczowe
        sw_signals[4].write(true);  // EW Pomarańczowe włączone
        wait(10, SC_NS);

        // Przejście do Stanu 6: NS Czerwone, EW Zielone
        sw_signals[3].write(false); // EW Czerwone wyłączone
        sw_signals[4].write(false); // EW Pomarańczowe wyłączone
        sw_signals[5].write(true);  // EW Zielone włączone
        wait(10, SC_NS);

        // Przejście do Stanu 7: NS Czerwone, EW Pomarańczowe
        sw_signals[5].write(false); // EW Zielone wyłączone
        sw_signals[4].write(true);  // EW Pomarańczowe włączone
        wait(10, SC_NS);

        // Powrót do Stanu 0: Wszystkie Czerwone
        sw_signals[4].write(false); // EW Pomarańczowe wyłączone
        sw_signals[3].write(true);  // EW Czerwone włączone
        wait(10, SC_NS);

        // Testowanie Trybu Migających Pomarańczowych
        sw_signals[6].write(true); // Aktywacja Trybu Migających Pomarańczowych
        wait(10, SC_NS);

        sw_signals[6].write(false); // Dezaktywacja Trybu Migających Pomarańczowych
        wait(10, SC_NS);

        // Testowanie Trybu Pojazdów Ratunkowych
        sw_signals[7].write(true); // Aktywacja Trybu Pojazdów Ratunkowych
        wait(10, SC_NS);

        sw_signals[7].write(false); // Dezaktywacja Trybu Pojazdów Ratunkowych
        wait(10, SC_NS);

        // Testowanie Nieprawidłowego Przejścia: Zarazem NS i EW Zielone
        sw_signals[2].write(true); // NS Zielone włączone
        sw_signals[5].write(true); // EW Zielone włączone (Nieprawidłowe)
        wait(10, SC_NS);

        // Sprawdzenie czy Emergency_LED jest aktywowany
        if (Emergency_LED.read()) {
            cout << sc_time_stamp() << ": Wykryto sytuację awaryjną zgodnie z oczekiwaniami." << endl;
        } else {
            cout << sc_time_stamp() << ": Sytuacja awaryjna NIE została wykryta, mimo że powinna." << endl;
        }

        // Reset do bezpiecznego stanu
        sw_signals[2].write(false); // NS Zielone wyłączone
        sw_signals[5].write(false); // EW Zielone wyłączone
        sw_signals[0].write(true);  // NS Czerwone włączone
        sw_signals[3].write(true);  // EW Czerwone włączone
        wait(10, SC_NS);

        // Zakończenie Testu
        sc_stop();
    }

    // Konstruktor
    SC_CTOR(Testbench) : clk_signal("clk_signal", 10, SC_NS) { // Okres zegara 10ns
        // Instancja Kontrolera Świateł Drogowych
        controller = new TrafficLightController("controller");

        // Podłączenie przełączników
        for (int i = 0; i < 8; ++i) {
            controller->sw[i](sw_signals[i]);
        }

        // Podłączenie zegara
        controller->clk(clk_signal);

        // Podłączenie diod LED
        controller->NS_Red_LED(NS_Red_LED);
        controller->NS_Orange_LED(NS_Orange_LED);
        controller->NS_Green_LED(NS_Green_LED);
        controller->EW_Red_LED(EW_Red_LED);
        controller->EW_Orange_LED(EW_Orange_LED);
        controller->EW_Green_LED(EW_Green_LED);
        controller->Emergency_LED(Emergency_LED);

        // Inicjalizacja wyjść LED
        NS_Red_LED.write(false);
        NS_Orange_LED.write(false);
        NS_Green_LED.write(false);
        EW_Red_LED.write(false);
        EW_Orange_LED.write(false);
        EW_Green_LED.write(false);
        Emergency_LED.write(false);

        // Uruchomienie sekwencji testowej
        SC_THREAD(test_sequence);
        // Usunięto linię `sensitive << clk_signal.pos();` ponieważ nie jest ona potrzebna
    }

    // Destruktor
    ~Testbench() {
        delete controller;
    }
};

// =============================================================================
// Funkcja sc_main
// =============================================================================
int sc_main(int argc, char* argv[]) {
    // Instancja Testbench
    Testbench tb("tb");

    // Rozpoczęcie symulacji
    sc_start();

    return 0;
}
