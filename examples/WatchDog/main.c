#include "./main.h"

void Wdt_Blink(){
    Dio_TogglePin(DIO_ID_B, DIO_PIN_5);
}

int main(){
    Dio_WritePinMode(DIO_ID_B, DIO_PIN_5, DIO_MODE_OUTPUT);
    Wdt_Init(WDT_MODE_INT, WDT_PRESC_64K);
    Wdt_RegisterISR(Wdt_Blink);
    sei(); // Enable global interrupts

    while(1){
        /* Comment this line to blink the led */
        Wdt_Reset();
    }

    return 0;
}