/**
 * @file hal_timer.h
 * @author Matheus Alencar Nascimento (matt-alencar)
 * @brief Header file of TIM HAL module.
 **************************************************************************
 * @copyright MIT License.
 * 
 */

#ifndef _TIMER_DRIVER_H_
#define _TIMER_DRIVER_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "hal_device.h"
#include "hal_gpio.h"


/**
  * @brief Timer Hardware Type
  */
typedef enum {
	TIMER8_TYPE,	/*!< 8-bit timer */
	TIMER16_TYPE,	/*!< 16-bit timer */
	TIMER_TYPE_NOT_DEFINED	/*!< Timer type is not defined */
}timer_type_t;


/** 
  * @brief Timer Comparator
  * @note Not all timers have all these comparators available
  */
typedef enum {
	TIMER_COMP_OCRA = 0,
	TIMER_COMP_OCRB,
	TIMER_COMP_OCRC,
}timer_comp_t;


/** 
  * @brief Timer Output Mode
  */
typedef enum {
	TIMER_OUTPUT_DISABLE = 0,
	TIMER_OUTPUT_TOGGLE,
	TIMER_OUTPUT_NON_INVERTING,
	TIMER_OUTPUT_INVERTING
}timer_out_mode_t;


/** 
  * @brief Timer Clock Mode
  */
typedef enum {
	TIMER_CLOCK_DISABLE = 0,
	TIMER_CLOCK_PRESC_1,
	TIMER_CLOCK_PRESC_8,
	TIMER_CLOCK_PRESC_64,
	TIMER_CLOCK_PRESC_256,
	TIMER_CLOCK_PRESC_1024,
	TIMER_CLOCK_EXT_FALLING_EDGE,
	TIMER_CLOCK_EXT_RISING_EDGE,
}timer_clock_t;


/** 
  * @brief Timer Interrupt Type
  * @note Not all timers have all these interrupt modes available
  */
typedef enum {
	TIMER_INT_OVF = TOIE0,		/*!< TIM Overflow IRQ */
	TIMER_INT_OCRA = OCIE0A,	/*!< TIM Comparator A match IRQ */
	TIMER_INT_OCRB = OCIE0B,	/*!< TIM Comparator B match IRQ */
	TIMER_INT_OCRC = OCIE1C,	/*!< TIM Comparator C match IRQ */
	TIMER_INT_ICR = ICIE1		/*!< TIM Input Comparator match IRQ */
}timer_int_t;


/** 
  * @brief Timer8 Operation Mode
  */
typedef enum {
	TIMER8_MODE_NORMAL = 0x00,
	TIMER8_MODE_CTC = 0x02,
	TIMER8_MODE_FAST_PWM_MAX_8BIT = 0x03,
	TIMER8_MODE_FAST_PWM_OCRA = 0x07,
	TIMER8_MODE_PHASE_CORRECT_PWM_MAX_8BIT = 0x01,
	TIMER8_MODE_PHASE_CORRECT_PWM_OCRA = 0x05
}timer8_mode_t;


/** 
  * @brief Timer16 Operation Mode
  */
typedef enum {
	TIMER16_MODE_NORMAL = 0x00,
	
	TIMER16_MODE_CTC_OCRA = 0x04,
	TIMER16_MODE_CTC_ICR = 0x0C,

	TIMER16_MODE_FAST_PWM_MAX_8BIT = 0x05,
	TIMER16_MODE_FAST_PWM_MAX_9BIT = 0x06,
	TIMER16_MODE_FAST_PWM_MAX_10BIT = 0x07,
	TIMER16_MODE_FAST_PWM_ICR = 0x0E,
	TIMER16_MODE_FAST_PWM_OCRA = 0x0F,
	
	TIMER16_MODE_PHASE_CORRECT_PWM_MAX_8BIT = 0x01,
	TIMER16_MODE_PHASE_CORRECT_PWM_MAX_9BIT = 0x02,
	TIMER16_MODE_PHASE_CORRECT_PWM_MAX_10BIT = 0x03,
	TIMER16_MODE_PHASE_CORRECT_PWM_ICR = 0x0A,
	TIMER16_MODE_PHASE_CORRECT_PWM_OCRA = 0x0B,

	TIMER16_MODE_PHASE_FREQUENCY_CORRECT_PWM_ICR = 0x08,
	TIMER16_MODE_PHASE_FREQUENCY_CORRECT_PWM_OCRA = 0x09
}timer16_mode_t;


/** 
  * @brief Timer input capture trigger edge
  */
typedef enum {
	TIMER_IC_POLARITY_FALLING,
	TIMER_IC_POLARITY_RISING,
}timer_ic_polarity_t;


/** 
  * @brief Timer input capture filter
  */
typedef enum {
	TIMER_IC_FILER_DISABLE,
	TIMER_IC_FILER_ENABLE,
}timer_ic_filter_t;


/** 
  * @brief Timer Configuration Struct definition
  */
typedef struct {
	uint8_t Mode;							/*!< Specifies the TIM operation mode. @ref timer8_mode_t, @ref timer16_mode_t */
	timer_clock_t Clock;					/*!< Specifies the TIM clock source */
	uint16_t CompA;							/*!< Specifies the TIM comparator A value */
	uint16_t CompB;							/*!< Specifies the TIM comparator B value */
	uint16_t CompC;							/*!< Specifies the TIM comparator C value (Only available on Timer-16) */
	timer_out_mode_t OutModeA;				/*!< Specifies the TIM output comparator A mode */
	timer_out_mode_t OutModeB;				/*!< Specifies the TIM output comparator B mode */
	timer_out_mode_t OutModeC;				/*!< Specifies the TIM output comparator C mode (Only available on Timer-16) */
	uint16_t Counter;						/*!< Specifies the TIM counter initial value */
	timer_ic_polarity_t ICPolarity;			/*!< Specifies the TIM input capture trigger edge (Only available on Timer-16) */
	timer_ic_filter_t ICFilter;				/*!< Specifies the TIM input capture filter state (Only available on Timer-16) */
}timer_init_t;


/**
 * @brief Enable global timer clock prescallers
 * @return None
 */
void Timer_Enable_Presc();


/**
 * @brief Stop and reset global timer clock prescallers
 * @return None
 */
void Timer_Disable_Presc();


/**
 * @brief Get timer hardware type
 * 
 * @param TIMx where x can be (0..5) to select the timer peripheral.
 * @return timer_type_t
 */
timer_type_t Timer_GetType(timer_t *TIMx);


/**
 * @brief Initialize configuration struct content
 * 
 * @param timer_init Configuration struct pointer
 * @return None
 */
void Timer_StructInit(timer_init_t *timer_init);


/**
 * @brief Initializes the timer according to the specified 
 *        parameters in timer_init.
 * 
 * @param TIMx where x can be (0..5) to select the timer peripheral.
 * @param timer_init Configuration struct pointer
 * @return None
 */
void Timer_Init(timer_t *TIMx, timer_init_t *timer_init);


/**
 * @brief DeInitializes the TIM peripheral
 * 
 * @param TIMx where x can be (0..5) to select the timer peripheral.
 * @return None
 */
void Timer_DeInit(timer_t *TIMx);


/**
 * @brief Setup TIM operation mode
 * @note Check the timer hardware type first
 * @ref timer8_mode_t
 * @ref timer16_mode_t
 * @param TIMx where x can be (0..5) to select the timer peripheral.
 * @param mode Can be any value from timer8_mode_t or timer16_mode_t.
 * @return None
 */
void Timer_SetMode(timer_t *TIMx, uint8_t mode);

/**
 * @brief Setup TIM input clock
 * 
 * @param TIMx where x can be (0..5) to select the timer peripheral.
 * @param clock can be anything from timer_clock_t.
 * @return None
 */
void Timer_SetClock(timer_t *TIMx, timer_clock_t clock);


/**
 * @brief Setup TIM output mode of comparator
 * 
 * @param TIMx where x can be (0..5) to select the timer peripheral.
 * @param comp Comparator module
 * @param mode Output mode
 * @return None
 */
void Timer_SetOutputMode(timer_t *TIMx, timer_comp_t comp, timer_out_mode_t mode);

/**
 * @brief Set TIM counter value
 * 
 * @param TIMx where x can be (0..5) to select the timer peripheral.
 * @param value for timer8 hardware the maximum value allowed is 0xFF, for timer16 is 0xFFFF.
 * @return None
 */
void Timer_SetCounter(timer_t *TIMx, uint16_t value);


/**
 * @brief Get TIM counter value
 * 
 * @param TIMx where x can be (0..5) to select the timer peripheral.
 * @return TIM counter value 
 */
uint16_t Timer_GetCounter(timer_t *TIMx);


/**
 * @brief Get TIM comparator value
 * 
 * @param TIMx where x can be (0..5) to select the timer peripheral.
 * @param comp Comparator module
 * @return uint16_t
 */
uint16_t Timer_GetCompare(timer_t *TIMx, timer_comp_t comp);


/**
 * @brief Setup TIM comparator value
 * 
 * @param TIMx where x can be (0..5) to select the timer peripheral.
 * @param comp Comparator module
 * @param value to write into selected comparator
 * @return None
 */
void Timer_SetCompare(timer_t *TIMx, timer_comp_t comp, uint16_t value);


/**
 * @brief Setup TIM input comparator
 * 
 * @param TIMx where x can be (0..5) to select the timer peripheral.
 * @param filter Filter state
 * @param polarity Trigger polarity
 * @return None
 */
void Timer_IC_Config(timer_t *TIMx, timer_ic_filter_t filter, timer_ic_polarity_t polarity);

/**
 * @brief Set TIM input comparator value
 * 
 * @param TIMx where x can be (0..5) to select the timer peripheral.
 * @param value to write into input comparator
 * @return None
 */
void Timer_IC_SetValue(timer_t *TIMx, uint16_t value);

/**
 * @brief Get TIM input comparator value
 * 
 * @param TIMx where x can be (0..5) to select the timer peripheral.
 * @return uint16_t 
 */
uint16_t Timer_IC_GetValue(timer_t *TIMx);


/**
 * @brief Check if TIM is enabled
 * 
 * @param TIMx where x can be (0..5) to select the timer peripheral.
 * @return uint8_t: 0 -> Timer stoped, 1 -> Timer running
 */
uint8_t Timer_IsEnabled(timer_t *TIMx);


/**
 * @brief Enable TIM interrupt request
 * 
 * @param TIMx where x can be (0..5) to select the timer peripheral.
 * @param int_type IRQ type
 * @return None
 */
void Timer_Enable_IRQ(timer_t *TIMx, timer_int_t int_type);


/**
 * @brief Disable TIM interrupt request
 * 
 * @param TIMx where x can be (0..5) to select the timer peripheral.
 * @param int_type IRQ type
 * @return None
 */
void Timer_Disable_IRQ(timer_t *TIMx, timer_int_t int_type);


#ifdef __cplusplus
}
#endif

#endif /* _TIMER_DRIVER_H_ */