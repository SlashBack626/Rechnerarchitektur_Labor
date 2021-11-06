/*
 First PIC32MM program
 
 This simple example program lets LED1 blink
 */

#include <stdint.h>
#include <xc.h>

void SYSTEM_Initialize(void);

void setup() {
    SYSTEM_Initialize(); // set 24 MHz clock for CPU and Peripheral Bus
    // clock period = 41,667 ns = 0,0417 us
    TRISBbits.TRISB9 = 1; // set button as input
    TRISBbits.TRISB0 = 0;
    LATBbits.LATB0 = 1;
    initTimer();

}

void initTimer() {
    T3CONbits.ON = 0;
    T2CONbits.ON = 0;
    T2CONbits.T32 = 1; // operate in 32 bit mode (only on even timers)
    PR2 = 0x6E35; // period register: Timer2 hold least significant bits
    PR3 = 0x1; // Timer3 hold most significant bits
    TMR2 = 0; // reset Timer2 counter
    TMR3 = 0; // reset Timer3 counter
    T2CONbits.TCKPS = 0b111; // set prescaler to 1/256
    T2CONbits.TCS = 0; // clock source: user internal peripheral clock (1: user External Clock)

    // note: timer3 will hold interrupt enable, flag and priority
    IFS0bits.T3IF = 0; // clear timer3 interrupt flag
    IEC0bits.T3IE = 1; // enable timer3 interrupts
    // T2CONbits.ON = 1;
}
char released = 1;

void loop() {
    while (1) {
        if (IFS0bits.T3IF == 1) {
            IFS0bits.T3IF = 0;
            LATBINV = 0x1;
        }
        if (released && PORTBbits.RB9 == 0) { // Bit is 0 when button pressed
            TMR2 = 0; // reset Timer counter
            T2CONbits.ON = 1;
            released = 0;
        } else if (PORTBbits.RB9 == 1) {
            released = 1;
            T2CONbits.ON = 0;
        }
    }
}

int main(void) {
    setup();
    loop();
}