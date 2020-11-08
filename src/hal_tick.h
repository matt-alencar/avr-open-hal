#ifndef _HAL_TICK_H_
#define _HAL_TICK_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "hal_def.h"
#include "hal_timer.h"

#ifndef HAL_TICK_TIMER
#define HAL_TICK_TIMER              TIM3
#endif

#ifndef HAL_TICK_TIMER_ISR_VECT
#define HAL_TICK_TIMER_ISR_VECT     TIMER3_OVF_vect
#endif

typedef uint32_t hal_tick_t;


void Tick_Inc();

hal_status_t Tick_Init();
hal_status_t Tick_DeInit();

void Tick_Suspend();
void Tick_Resume();

void Tick_Delay(hal_tick_t delay_ms);
hal_tick_t Tick_Get();


#ifdef __cplusplus
}
#endif

#endif /* _HAL_TICK_H_ */