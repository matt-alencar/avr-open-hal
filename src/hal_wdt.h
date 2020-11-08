#ifndef _WDT_DRIVER_H_
#define _WDT_DRIVER_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "hal_def.h"

/**
 * @brief WatchDog timer prescaller counter selector values
 * 
 */
typedef enum {
    WDT_PRESC_2K = 0,
    WDT_PRESC_4K,
    WDT_PRESC_8K,
    WDT_PRESC_16K,
    WDT_PRESC_32K,
    WDT_PRESC_64K,
    WDT_PRESC_128K,
    WDT_PRESC_256K,
    WDT_PRESC_512K,
    WDT_PRESC_1024K
}wdt_presc_t;

/**
 * @brief WatchDog timer operation modes
 * 
 */
typedef enum {
    WDT_MODE_STOPPED = 0,
    WDT_MODE_INT,
    WDT_MODE_RST,
    WDT_MODE_INT_RST
}wdt_mode_t;


void WDT_Init(wdt_mode_t, wdt_presc_t);
void WDT_DeInit();

uint8_t WDT_IsEnabled();

void WDT_Reset();

#ifdef __cplusplus
}
#endif

#endif /* _WDT_DRIVER_H_ */