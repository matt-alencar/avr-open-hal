/**
 * @file hal_gpio.c
 * @author Matheus Alencar Nascimento (matt-alencar)
 * @brief GPIO HAL module driver.
 *        This file provides firmware functions to manage the following 
 *        functionalities of the General Purpose Input/Output (GPIO) peripheral:
 *          + Initialization and de-initialization functions
 *          + IO operation functions
 *          + Interrupts and flags management
 * 
 *********************************************************************************
 * @copyright MIT License.
 * 
 */

#include "hal_gpio.h"


void GPIO_Init(gpio_t* GPIOx, gpio_init_t* GPIO_Init){
    GPIO_PinMode(GPIOx, GPIO_Init->Pin, GPIO_Init->Mode);
    if(GPIO_Init->Mode == GPIO_MODE_OUTPUT)
        GPIO_WritePin(GPIOx, GPIO_Init->Pin, GPIO_Init->State);
}

void GPIO_DeInit(gpio_t *GPIOx, gpio_pin_t GPIO_Pin){
    GPIO_PinMode(GPIOx, GPIO_Pin, GPIO_MODE_INPUT);
    GPIO_ResetPin(GPIOx, GPIO_Pin);
}

void GPIO_PinMode(gpio_t* GPIOx, gpio_pin_t GPIO_Pin, gpio_mode_t GPIO_Mode){
    switch (GPIO_Mode){
        case GPIO_MODE_INPUT:
            GPIOx->DDR_REG &= ~GPIO_Pin;
            GPIOx->PORT_REG &= ~GPIO_Pin;
            break;
        
        case GPIO_MODE_INPUT_PULLUP:
            GPIOx->DDR_REG &= ~GPIO_Pin;
            GPIOx->PORT_REG |= GPIO_Pin;
            break;

        case GPIO_MODE_OUTPUT:
            GPIOx->DDR_REG |= GPIO_Pin;
            break;

        default:
            break;
    }
}

inline gpio_state_t GPIO_ReadPin(gpio_t* GPIOx, gpio_pin_t GPIO_Pin){
    if(GPIOx->PIN_REG & GPIO_Pin)
        return GPIO_STATE_HIGH;
    else
        return GPIO_STATE_LOW;
}

inline uint8_t GPIO_Read(gpio_t* GPIOx){
    return GPIOx->PIN_REG;
}

inline void GPIO_SetPin(gpio_t* GPIOx, gpio_pin_t GPIO_Pin){
    GPIOx->PORT_REG |= GPIO_Pin;
}

inline void GPIO_ResetPin(gpio_t* GPIOx, gpio_pin_t GPIO_Pin){
    GPIOx->PORT_REG &= ~GPIO_Pin;
}

inline void GPIO_TogglePin(gpio_t* GPIOx, gpio_pin_t GPIO_Pin){
    GPIOx->PORT_REG ^= GPIO_Pin;
}

inline void GPIO_WritePin(gpio_t* GPIOx, gpio_pin_t GPIO_Pin, gpio_state_t PinState){
    if(PinState)
        GPIO_SetPin(GPIOx, GPIO_Pin);
    else
        GPIO_ResetPin(GPIOx, GPIO_Pin);
}

inline void GPIO_Write(gpio_t* GPIOx, uint8_t Value){
    GPIOx->PORT_REG = Value;
}

inline void GPIO_Enable_Pullups(){
    MCUCR &= ~_BV(PUD);
}

inline void GPIO_Disable_Pullups(){
    MCUCR |= _BV(PUD);
}

void GPIO_Enable_PinChange_IQR(gpio_int_change_pin_t PCI_Pin){
    uint8_t block_id = 0;
    uint8_t offset = 0;
    if(PCI_Pin >= GPIO_PIN_INT_CHANGE_8 && PCI_Pin < GPIO_PIN_INT_CHANGE_16){
        offset = 8;
        block_id = 1;
    }
    else if(PCI_Pin >= GPIO_PIN_INT_CHANGE_16){
        offset = 16;
        block_id = 2;
    }
    GPIO_INT_MASK->PCMSK_REG[block_id] |= _BV(PCI_Pin - offset);
    // *((uint32_t*) GPIO_INT_MASK->PCMSK_REG) |= _BV(PCI_Pin);

    PCIFR |= _BV(block_id); // CLEAN INTERRUPT FLAG
    PCICR |= _BV(block_id); // ENABLE INTERRUPT
}

void GPIO_Disable_PinChange_IQR(gpio_int_change_pin_t PCI_Pin){
    uint8_t block_id = 0;
    uint8_t offset = 0;

    if(PCI_Pin >= GPIO_PIN_INT_CHANGE_8 && PCI_Pin < GPIO_PIN_INT_CHANGE_16){
        offset = 8;
        block_id = 1;
    }
    else if(PCI_Pin >= GPIO_PIN_INT_CHANGE_16){
        offset = 16;
        block_id = 2;
    }

    GPIO_INT_MASK->PCMSK_REG[block_id] &= ~_BV(PCI_Pin - offset);
    // *((uint32_t*) GPIO_INT_MASK->PCMSK_REG) &= ~_BV(PCI_Pin);

    if(GPIO_INT_MASK->PCMSK_REG[block_id] == 0){
        PCIFR |= _BV(block_id);     // CLEAN INTERRUPT FLAG
        PCICR &= ~_BV(block_id);    // DISABLE INTERRUPT
    }
}

void GPIO_Enable_EXTI_IQR(gpio_extint_pin_t EXTI_Pin, gpio_extint_mode_t Mode){
    if(EXTI_Pin <= GPIO_EXTINT_PIN_INT3){
        EICRA |= (Mode << (EXTI_Pin*2));
    }
    else if(EXTI_Pin > GPIO_EXTINT_PIN_INT3){
        EICRB |= (Mode << ((EXTI_Pin-4)*2));
    }
    EIFR |= _BV(EXTI_Pin);   // CLEAN INTERRUPT FLAG
    EIMSK |= _BV(EXTI_Pin);  // ENABLE INTERRUPT
}

void GPIO_Disable_EXTI_IQR(gpio_extint_pin_t EXTI_Pin){
    if(EXTI_Pin <= GPIO_EXTINT_PIN_INT3){
        EICRA &= ~(3 << (EXTI_Pin*2));
    }
    else if(EXTI_Pin > GPIO_EXTINT_PIN_INT3){
        EICRB &= ~(3 << ((EXTI_Pin-4)*2));
    }
    EIFR |= _BV(EXTI_Pin);   // CLEAN INTERRUPT FLAG
    EIMSK &= ~_BV(EXTI_Pin); // DISABLE INTERRUPT
}