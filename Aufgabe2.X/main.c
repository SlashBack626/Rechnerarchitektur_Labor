#include <xc.h>

char state = 0; // 0=off 1=on
char released = 1; //0=release 1=press

void setup() {
    TRISBbits.TRISB9 = 1; // set button as input
    TRISBbits.TRISB0 = 0; // set LED as output
}

void loop() {
    while (1) {
        if (released && PORTBbits.RB9 == 0) { // Bit is 0 when button pressed
            state = ++state % 2;
            released = 0;
        } else if (PORTBbits.RB9 == 1) {
            released = 1;
        }
        LATBbits.LATB0 = state;
    }
}

void main() {
    SYSTEM_Initialize();
    setup();
    loop();
}