#include <xc.h>

void setup()
{
    initDisplay();
    sevenSegWrite(0);
}

void loop()
{
    while (1)
    {
    }
}

void main()
{
    SYSTEM_Initialize();
    setup();
    // loop();
}