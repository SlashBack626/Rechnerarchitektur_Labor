#include <xc.h>
#include <sys/attribs.h>

void initTimer();
void initDisplay();
void initDAC();
void setup()
{
    SYSTEM_Initialize(); // set 24 MHz clock for CPU and Peripheral Bus
                         // clock period = 41,667 ns = 0,0417 us
    initTimer();
    TRISBbits.TRISB14 = 0;
    initADC();
    initDAC();
//    setFreq(4095);
}

void initTimer()
{
    // 4Hz timer
    T3CONbits.ON = 0;
    T2CONbits.ON = 0;
    T2CONbits.T32 = 1;       // operate in 32 bit mode (only on even timers)
    PR2 = 1500;            // period register: Timer2 hold least significant bits
    PR3 = 0x0;               // Timer3 hold most significant bits
    TMR2 = 0;                // reset Timer2 counter
    TMR3 = 0;                // reset Timer3 counter
    T2CONbits.TCKPS = 0b000; // set prescaler to 1/64
    T2CONbits.TCS = 0;       // clock source: user internal peripheral clock (1: user External Clock)

    // note: timer3 will hold interrupt enable, flag and priority
    IFS0bits.T3IF = 0; // clear timer3 interrupt flag
    IPC4bits.T3IP = 5;
    IEC0bits.T3IE = 1; // enable timer3 interrupts
    T2CONbits.ON = 1;
}

void setFreq(int potVal){
    T2CONbits.ON = 0;
    int freq = (800*potVal + 200*4095)/4095;
    TMR2 = 0;
    PR2 = 24000000/(1*freq*32);
    T2CONbits.ON = 1;
}

void initADC()
{
    ANSELCbits.ANSC5 = 1;
    TRISCbits.TRISC5 = 1;
    AD1CHSbits.CH0SA = 14;
    AD1CON3bits.ADRC = 0;
    AD1CON3bits.ADCS = 20;
    AD1CON1bits.FORM = 0;
    AD1CON1bits.MODE12 = 1;
    AD1CON1bits.ON = 1;
}

void initDAC(){
    DAC1CONbits.REFSEL = 0b11;
    DAC1CONbits.ON = 1;
    DAC1CONbits.DACOE = 1;
}

unsigned int readADC()
{
    AD1CON1bits.SAMP = 1;
    delay_us(100);
    AD1CON1bits.SAMP = 0;
    while (!AD1CON1bits.DONE)
        ;
    return ADC1BUF0;
}
char steps = 0;

void nextOutput(){
    DAC1CONbits.DACDAT = steps;
    steps++;
    steps %= 32;
}

void __ISR(_TIMER_3_VECTOR, IPL5SOFT) Timer1Handler(void){
//    readADC();
    nextOutput();
    IFS0bits.T3IF = 0;
}

void loop()
{
    while (1)
    {
        delay_us(1000);
       int potVal = readADC();
       
       setFreq(potVal);
    }
}

void main()
{
    setup();
    loop();
}