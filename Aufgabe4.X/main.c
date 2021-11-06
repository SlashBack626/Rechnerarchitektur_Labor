/*
 First PIC32MM program
 
 This simple example program lets LED1 blink
 */

#include <stdint.h>
#include <xc.h>

void SYSTEM_Initialize(void);


int state = 0b0000100000;
char dir = 0; // 0=left 1=right

void setup() {
    SYSTEM_Initialize(); // set 24 MHz clock for CPU and Peripheral Bus
    // clock period = 41,667 ns = 0,0417 us
    TRISASET = 0b0000001111100000; // RA5 - RA9

    initTimer();

}

void initTimer() {
    // 4Hz timer
    T3CONbits.ON = 0;
    T2CONbits.ON = 0;
    T2CONbits.T32 = 1; // operate in 32 bit mode (only on even timers)
    PR2 = 0x6E35; // period register: Timer2 hold least significant bits
    PR3 = 0x1; // Timer3 hold most significant bits
    TMR2 = 0; // reset Timer2 counter
    TMR3 = 0; // reset Timer3 counter
    T2CONbits.TCKPS = 0b110; // set prescaler to 1/64
    T2CONbits.TCS = 0; // clock source: user internal peripheral clock (1: user External Clock)

    // note: timer3 will hold interrupt enable, flag and priority
    IFS0bits.T3IF = 0; // clear timer3 interrupt flag
    IEC0bits.T3IE = 1; // enable timer3 interrupts
    T2CONbits.ON = 1;
}

void loop() {
    while (1) {
        if (IFS0bits.T3IF == 1) {
            IFS0bits.T3IF = 0;
            LATACLR = state; // clear old bit
            if (dir) {
                state >> 1; // shift bit by 1
            } else {
                state << 1;
            }
            if (state & 0b0000100000) {
                dir = 0; // shift left
            }
            if (state & 0b1000000000) {
                dir = 1; // shift right
            }
            LATASET = state; // set new bit
        }
        
    }
}

int main(void) {
    setup();
    loop();
}