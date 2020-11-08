#include "hal_tick.h"

// the prescaler is set so that timer ticks every 64 clock cycles, and the
// the overflow handler is called every 256 ticks.
#define MICROSECONDS_PER_TIMER_OVERFLOW (clockCyclesToMicroseconds(64 * 256))

// the whole number of milliseconds per timer overflow
#define MILLIS_INC (MICROSECONDS_PER_TIMER_OVERFLOW / 1000)

// the fractional number of milliseconds per timer overflow. we shift right
// by three to fit these numbers into a byte. (for the clock speeds we care
// about - 8 and 16 MHz - this doesn't lose precision.)
#define FRACT_INC ((MICROSECONDS_PER_TIMER_OVERFLOW % 1000) >> 3)
#define FRACT_MAX (1000 >> 3)


static volatile hal_tick_t ticks_cnt;
static volatile uint8_t ticks_fract;


void Tick_Inc(){
    // copy these to local variables so they can be stored in registers
	// (volatile variables must be read from memory on every access)
    hal_tick_t m = ticks_cnt;
    uint8_t f = ticks_fract;
    m += MILLIS_INC;
	f += FRACT_INC;
	if (f >= FRACT_MAX) {
		f -= FRACT_MAX;
		m += 1;
	}

	ticks_fract = f;
	ticks_cnt = m;
}

hal_tick_t Tick_Get(){
    hal_tick_t m;

    uint8_t oldSREG = SREG;
    // disable interrupts while we read ticks_cnt or we might get an
	// inconsistent value (e.g. in the middle of a write to ticks_cnt)
	cli();
    m = ticks_cnt;
    SREG = oldSREG;
    return m;
}

void Tick_Delay(hal_tick_t delay_ms){
    hal_tick_t start = Tick_Get();

    while((Tick_Get() - start) < delay_ms);
}

hal_status_t Tick_Init(){
    timer_type_t type = Timer_GetType(HAL_TICK_TIMER);
    uint8_t op_mode;

    if(type == TIMER8_TYPE){
        op_mode = TIMER8_MODE_FAST_PWM_MAX_8BIT;
    }
    else if(type == TIMER16_TYPE){
        op_mode = TIMER16_MODE_FAST_PWM_MAX_8BIT;
    }
    else{
        return HAL_ERROR;
    }

    Timer_SetMode(HAL_TICK_TIMER, op_mode);
    Timer_SetCounter(HAL_TICK_TIMER, 0);
    Timer_SetClock(HAL_TICK_TIMER, TIMER_CLOCK_PRESC_64);
    Timer_Enable_IRQ(HAL_TICK_TIMER, TIMER_INT_OVF);
    ticks_cnt = 0;
    ticks_fract = 0;
    return HAL_OK;
}

hal_status_t Tick_DeInit(){
    Tick_Suspend();
    Timer_SetCounter(HAL_TICK_TIMER, 0);
    ticks_cnt = 0;
    ticks_fract = 0;
    return HAL_OK;
}

void Tick_Suspend(){
    Timer_SetClock(HAL_TICK_TIMER, TIMER_CLOCK_DISABLE);
    Timer_Disable_IRQ(HAL_TICK_TIMER, TIMER_INT_OVF);
}

void Tick_Resume(){
    Timer_SetClock(HAL_TICK_TIMER, TIMER_CLOCK_PRESC_64);
    Timer_Enable_IRQ(HAL_TICK_TIMER, TIMER_INT_OVF);
}