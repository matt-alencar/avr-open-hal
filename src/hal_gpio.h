/**
 * @file hal_gpio.h
 * @author Matheus Alencar Nascimento (matt-alencar)
 * @brief Header file of GPIO HAL module.
 **************************************************************************
 * @copyright MIT License.
 * 
 */

#ifndef _GPIO_DRIVER_H_
#define _GPIO_DRIVER_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "hal_def.h"


/**
 * @brief GPIO pins
 */
typedef enum {
	GPIO_PIN_0 = 0x01,
	GPIO_PIN_1 = 0x02,
	GPIO_PIN_2 = 0x04,
	GPIO_PIN_3 = 0x08,
	GPIO_PIN_4 = 0x10,
	GPIO_PIN_5 = 0x20,
	GPIO_PIN_6 = 0x40,
	GPIO_PIN_7 = 0x80,
	GPIO_PIN_ALL = 0xFF
}gpio_pin_t;


/**
 * @brief GPIO pin mode
 */
typedef enum {
	GPIO_MODE_INPUT = 0,
	GPIO_MODE_INPUT_PULLUP,
	GPIO_MODE_OUTPUT
}gpio_mode_t;


/**
 * @brief GPIO pin state
 */
typedef enum {
	GPIO_STATE_LOW = 0,
	GPIO_STATE_HIGH
}gpio_state_t;


/**
 * @brief Change interrupt pins available
 */
typedef enum {
	GPIO_PIN_INT_CHANGE_0,
	GPIO_PIN_INT_CHANGE_1,
	GPIO_PIN_INT_CHANGE_2,
	GPIO_PIN_INT_CHANGE_3,
	GPIO_PIN_INT_CHANGE_4,
	GPIO_PIN_INT_CHANGE_5,
	GPIO_PIN_INT_CHANGE_6,
	GPIO_PIN_INT_CHANGE_7,
	GPIO_PIN_INT_CHANGE_8,
	GPIO_PIN_INT_CHANGE_9,
	GPIO_PIN_INT_CHANGE_10,
	GPIO_PIN_INT_CHANGE_11,
	GPIO_PIN_INT_CHANGE_12,
	GPIO_PIN_INT_CHANGE_13,
	GPIO_PIN_INT_CHANGE_14,
	GPIO_PIN_INT_CHANGE_15,
	GPIO_PIN_INT_CHANGE_16,
	GPIO_PIN_INT_CHANGE_17,
	GPIO_PIN_INT_CHANGE_18,
	GPIO_PIN_INT_CHANGE_19,
	GPIO_PIN_INT_CHANGE_20,
	GPIO_PIN_INT_CHANGE_21,
	GPIO_PIN_INT_CHANGE_22,
	GPIO_PIN_INT_CHANGE_23,
	GPIO_PIN_INT_CHANGE_MAX
}gpio_int_change_pin_t;


/**
 * 
 * @brief EXTINT pin interrupt trigger
 */
typedef enum {
	GPIO_EXTINT_MODE_LOW = 0,
	GPIO_EXTINT_MODE_CHANGE,
	GPIO_EXTINT_MODE_FALLING,
	GPIO_EXTINT_MODE_RISING
}gpio_extint_mode_t;


/**
 * @brief EXTINT pins
 */
typedef enum {
	GPIO_EXTINT_PIN_INT0 = 0,
	GPIO_EXTINT_PIN_INT1,
	GPIO_EXTINT_PIN_INT2,
	GPIO_EXTINT_PIN_INT3,
	GPIO_EXTINT_PIN_INT4,
	GPIO_EXTINT_PIN_INT5,
	GPIO_EXTINT_PIN_INT6,
	GPIO_EXTINT_PIN_INT7,
	GPIO_EXTINT_PIN_MAX
}gpio_extint_pin_t;


/** 
 * @brief GPIO Init structure definition
 */
typedef struct {
	gpio_pin_t Pin;			/*!< Specifies the GPIO pins to be configured. This parameter can be any value of @ref gpio_pin_t */
	gpio_mode_t Mode;		/*!< Specifies the operating mode for the selected pins. This parameter can be a value of @ref gpio_mode_t */
	gpio_state_t State;		/*!< Specifies the state for the selected pins. This parameter can be a value of @ref gpio_state_t */
}gpio_init_t;


/**
 * @brief  Initialize the GPIOx peripheral according to the specified parameters in the gpio_init_t
 * @param  GPIOx where x can be (A..F) to select the GPIO peripheral.
 * @param  GPIO_Init pointer to a gpio_init_t structure that contains
 *         the configuration information for the specified GPIO peripheral.
 * @return None
 */
void GPIO_Init(gpio_t *GPIOx, gpio_init_t *GPIO_Init);


/**
 * @brief De-initialize the GPIOx peripheral registers to their default reset values.
 * 
 * @param GPIOx where x can be (A..F) to select the GPIO peripheral.
 * @param GPIO_Pin specifies the port bit to be written.
 *        This parameter can be GPIO_PIN_x where x can be (0..7) or GPIO_PIN_ALL for all pins.
 */
void GPIO_DeInit(gpio_t *GPIOx, gpio_pin_t GPIO_Pin);


/**
 * @brief Setup GPIO pin mode
 * @param GPIOx where x can be (A..F) to select the GPIO peripheral.
 * @param GPIO_Pin specifies the port bit to setup mode.
 *        This parameter can be GPIO_PIN_x where x can be (0..7) or GPIO_PIN_ALL for all pins.
 * @param GPIO_Mode specifies the operation mode.
 *        This parameter can be a value of @ref gpio_mode_t.
 * @return None
 */
void GPIO_PinMode(gpio_t *GPIOx, gpio_pin_t GPIO_Pin, gpio_mode_t GPIO_Mode);


/**
 * @brief Set pin to HIGH state
 * 
 * @param GPIOx where x can be (A..F) to select the GPIO peripheral.
 * @param GPIO_Pin specifies the port bit.
 *     	  This parameter can be GPIO_PIN_x where x can be (0..7) or GPIO_PIN_ALL for all pins.
 * @return None
 */
void GPIO_SetPin(gpio_t *GPIOx, gpio_pin_t GPIO_Pin);


/**
 * @brief Set pin to LOW state
 * 
 * @param GPIOx where x can be (A..F) to select the GPIO peripheral.
 * @param GPIO_Pin specifies the port bit.
 *     	  This parameter can be GPIO_PIN_x where x can be (0..7) or GPIO_PIN_ALL for all pins.
 * @return None
 */
void GPIO_ResetPin(gpio_t *GPIOx, gpio_pin_t GPIO_Pin);


/**
 * @brief Toggle pin state
 * 
 * @param GPIOx where x can be (A..F) to select the GPIO peripheral.
 * @param GPIO_Pin specifies the port bit.
 *     	  This parameter can be GPIO_PIN_x where x can be (0..7) or GPIO_PIN_ALL for all pins.
 * @return None
 */
void GPIO_TogglePin(gpio_t *GPIOx, gpio_pin_t GPIO_Pin);


/**
 * @brief Set pin level
 * 
 * @param GPIOx where x can be (A..F) to select the GPIO peripheral.
 * @param GPIO_Pin specifies the port bit.
 *     	  This parameter can be GPIO_PIN_x where x can be (0..7) or GPIO_PIN_ALL for all pins.
 * @param PinState specifies the state (HIGH or LOW).
 *        This parameter can be a value of @ref gpio_state_t
 * @return None
 */
void GPIO_WritePin(gpio_t *GPIOx, gpio_pin_t GPIO_Pin, gpio_state_t PinState);


/**
 * @brief Read the specified input port pin.
 * 
 * @param GPIOx where x can be (A..F) to select the GPIO peripheral.
 * @param GPIO_Pin specifies the port bit.
 *     	  This parameter can be GPIO_PIN_x where x can be (0..7) or GPIO_PIN_ALL for all pins.
 * @return The input port pin value. 
 */
gpio_state_t GPIO_ReadPin(gpio_t *GPIOx, gpio_pin_t GPIO_Pin);


/**
 * @brief Write value direct to GPIO output register (PORT).
 * @note This operation will override the logic value of all pins on GPIOx.
 * @param GPIO where x can be (A..F) to select the GPIO peripheral.
 * @param Value specifies the port bit to turn on/off.
 * @return None
 */
void GPIO_Write(gpio_t *GPIO, uint8_t Value);


/**
 * @brief Read value direct from GPIO input register (PIN).
 * 
 * @param GPIOx where x can be (A..F) to select the GPIO peripheral.
 * @return GPIOx input register value.
 */
uint8_t GPIO_Read(gpio_t *GPIOx);


/**
 * @brief Enable global GPIOx pullups
 * @note This function will only affect the pins that is configured as input with pullup.
 * @return None
 */
void GPIO_Enable_Pullups();


/**
 * @brief Disable global GPIOx pullups
 * @note This function will only affect the pins that is configured as input with pullup.
 * @return None
 */
void GPIO_Disable_Pullups();


/**
 * @brief Enable interrupt request from pin change state
 * 
 * @param PCI_Pin specifies the PCINT pin @ref gpio_int_change_pin_t
 * @return None
 */
void GPIO_Enable_PinChange_IQR(gpio_int_change_pin_t PCI_Pin);


/**
 * @brief Disable interrupt request from pin change state
 * 
 * @param PCI_Pin specifies the PCINT pin @ref gpio_int_change_pin_t
 * @return None
 */
void GPIO_Disable_PinChange_IQR(gpio_int_change_pin_t PCI_Pin);


/**
 * @brief Enable interrupt request from EXTI pin
 * 
 * @param EXTI_Pin specifies the EXTI pin @ref gpio_extint_pin_t
 * @param Mode specifies the interrupt trigger mode.
 *        This parameter can be a value of @ref gpio_extint_mode_t
 * @return None
 */
void GPIO_Enable_EXTI_IQR(gpio_extint_pin_t EXTI_Pin, gpio_extint_mode_t Mode);


/**
 * @brief Disable interrupt request from EXTI pin
 * 
 * @param EXTI_Pin specifies the EXTI pin @ref gpio_extint_pin_t
 * @return None
 */
void GPIO_Disable_EXTI_IQR(gpio_extint_pin_t EXTI_Pin);


#ifdef __cplusplus
}
#endif

#endif /* _GPIO_DRIVER_H_ */