/*
 First PIC32MM program
 
 This simple example program lets LED1 blink
 */

#include <stdint.h>
#include <xc.h>

void SYSTEM_Initialize(void);


char state = 0;
char dir = 0; // 0=left 1=right

void setup() {
    SYSTEM_Initialize(); // set 24 MHz clock for CPU and Peripheral Bus
    // clock period = 41,667 ns = 0,0417 us
    TRISASET = 0b0000001111100000; // RA5 - RA9

    initTimer();
    initPWM();

}

int PWM[5] = {0xEA6, 0x3A9, 0x753, 0xAFD, 0x0};

void initPWM() {
    CCP1CON1bits.TRIGEN = 0; // Set Sync/Triggered mode (Synchronous Mode)
    CCP1CON1bits.SYNC = 0; // rolls over at FFFFh or match
    // with period register (self sync)
    CCP1CON1bits.T32 = 0; // 16-bit dual timer mode
    CCP1CON1bits.TMRSYNC = 0; // Set timebase synchronization (Synchronized)
    CCP1CON1bits.CLKSEL = 0; // Set the clock source (SystemClock Tcy)
    CCP1CON1bits.TMRPS = 0b11; // Set the clock prescaler (1:64)
    CCP1PRbits.PRL = PWM[2]; // 16-bit MCCP1 low period bits
    CCP1PRbits.PRH = PWM[2]; // 16-bit MCCP1 high period bits
    CCP1CON1bits.ON = 1; // Start the Timer
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
            if (dir) {
                state++; // shift bit by 1
            } else {
                state--;
            }
            if (state == 4){
                dir = 0;
            }
            if(state == 0){
                dir = 1;
            }
            CCP1PRbits.PRL = PWM[state]; // 16-bit MCCP1 low period bits
            CCP1PRbits.PRH = PWM[4-state];
        }

    }
}

int main(void) {
    setup();
    loop();
}