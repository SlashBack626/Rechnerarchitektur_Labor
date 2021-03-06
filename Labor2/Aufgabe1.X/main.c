#include <xc.h>

void initTimer();
void initDisplay();

int digit1 = 0;
int digit2 = 0;
char state = 0;
void setup()
{
    SYSTEM_Initialize(); // set 24 MHz clock for CPU and Peripheral Bus
                         // clock period = 41,667 ns = 0,0417 us
    initDisplay();
    initTimer();
    TRISBbits.TRISB1 = 0;
    TRISBbits.TRISB2 = 0;
    LATBbits.LATB1 = 0;
    LATBbits.LATB2 = 1;
}

void initTimer()
{
    // 4Hz timer
    T3CONbits.ON = 0;
    T2CONbits.ON = 0;
    T2CONbits.T32 = 1;       // operate in 32 bit mode (only on even timers)
    PR2 = 0x6E35;            // period register: Timer2 hold least significant bits
    PR3 = 0x1;               // Timer3 hold most significant bits
    TMR2 = 0;                // reset Timer2 counter
    TMR3 = 0;                // reset Timer3 counter
    T2CONbits.TCKPS = 0b111; // set prescaler to 1/64
    T2CONbits.TCS = 0;       // clock source: user internal peripheral clock (1: user External Clock)

    // note: timer3 will hold interrupt enable, flag and priority
    IFS0bits.T3IF = 0; // clear timer3 interrupt flag
    IEC0bits.T3IE = 1; // enable timer3 interrupts
    T2CONbits.ON = 1;
}

void loop()
{ 
    int counter = 0;  
    while (1)
    {
        if (IFS0bits.T3IF == 1)
        {
            IFS0bits.T3IF = 0;
            digit2++;
            if(digit2 %10 == 0){
                digit1++;
                digit2 = 0;
            }
            digit1 %= 10;
        }
        if(state){
        sevenSegWrite(digit1);

        }else {
        sevenSegWrite(digit2);
        }
        counter ++;
        if (counter == 100){
        LATBINV = 0b110;
        state = !state;
        counter = 0;
        }
    }
}

void main()
{
    setup();
    loop();
}