#include "hal_wdt.h"
#include <avr/wdt.h>


void WDT_Init(wdt_mode_t mode, wdt_presc_t presc){
    wdt_disable();
    uint8_t presc_raw = (presc & 0x08 ? _BV(WDP3) : 0x00) | (presc & 0x07);
    uint8_t mode_raw = (mode & 0x01 ? _BV(WDIE) : 0x00) | (mode & 0x02 ? _BV(WDE) : 0x00);
    uint8_t wdt_config = (presc_raw | mode_raw);
    uint8_t oldSREG = SREG;
    cli();
    WDTCSR |= (_BV(WDCE) | _BV(WDE));
    WDTCSR = wdt_config;
    SREG = oldSREG;
}

void WDT_DeInit(){
    wdt_disable();
}

uint8_t WDT_IsEnabled(){
    return (WDTCSR & _BV(WDE) || WDTCSR & _BV(WDIE));
}

void WDT_Reset(){
    wdt_reset();
}
