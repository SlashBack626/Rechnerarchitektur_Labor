#include <xc.h>
#include <sys/attrib.h>

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
    initADC();
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

void initADC()
{
    ANSELCbits.ANSC5 = 1;
    TRISCbits.TRISC5 = 1;
    AD1CHSbits.CH0SA = 15;
    AD1CON3bits.ADRC = 0;
    AD1CON3bits.ADCS = 20;
    AD1CON1bits.FORM = 0;
    AD1CON1bits.MODE12 = 1;
    AD1CON1bits.ON = 1;
}

unsigned int readADC()
{
    AD1CON1bits.SAMP = 1;
    delay_us(100);
    AD1CON1bits.SAMP = 0;
    while (!AD1CON1bits.DONE)
        ;
    return (ADC1BUF0 - 621) / 12.42;
}

void __ISR(_Timer_3_Vector, ipl3) Timer1Handler(void){
    temp = readADC();
    IFS0bits.T3IF = 0;
}

int temp = 0;
void loop()
{
    int counter = 0;
    while (1)
    {
        if (state)
        {
            sevenSegWrite(temp / 10);
        }
        else
        {
            sevenSegWrite(temp % 10);
        }
        counter++;
        if (counter == 100)
        {
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