#ifndef _CORE_ATMEGA2560_H_
#define _CORE_ATMEGA2560_H_

#include <avr/io.h>
#include <stdint.h>

/** @addtogroup Peripheral_registers_structures
 * @{
 */

#define PCMSK_SIZE  3
#define TIMSK_SIZE  6
#define TIFR_SIZE   6

/** 
 * @brief USART Registers
 */

typedef struct {
	volatile uint8_t UCSRA_REG; /**< USART Control and Status Register A */
	volatile uint8_t UCSRB_REG; /**< USART Control and Status Register B */
	volatile uint8_t UCSRC_REG; /**< USART Control and Status Register C */
	uint8_t RESERVED;           /**< Reserved */
	volatile uint16_t UBRR_REG; /**< USART Baud Rate Register */
	volatile uint8_t UDR_REG;   /**< USART I/O Data Register */
}usart_t;

/** 
 * @brief TWI Registers
 */

typedef struct {
	volatile uint8_t TWBR_REG;  /**< TWI Bit Rate Register */
	volatile uint8_t TWSR_REG;  /**< TWI Status Register */
	volatile uint8_t TWAR_REG;  /**< TWI (Slave) Address Register */
	volatile uint8_t TWDR_REG;  /**< TWI Data Register */
	volatile uint8_t TWCR_REG;  /**< TWI Control Register */
	volatile uint8_t TWAMR_REG; /**< TWI (Slave) Address Mask Register */
}twi_t;

/** 
 * @brief SPI Registers
*/

typedef struct {
	volatile uint8_t SPCR_REG;  /**< SPI Control Register */
	volatile uint8_t SPSR_REG;  /**< SPI Status Register */
	volatile uint8_t SPDR_REG;  /**< SPI Data Register */
}spi_t;

/** 
 * @brief GPIO Pin Change Mask Register
 */

typedef struct {
	volatile uint8_t PCMSK_REG[PCMSK_SIZE]; /**< Pin Change Mask Register Bank */
}gpio_int_mask_t;

/** 
 * @brief ADC Registers
 */

typedef struct {
	volatile uint16_t ADC_REG;      /**< ADC Data Register */
	volatile uint8_t ADCSRA_REG;    /**< ADC Control and Status Register A */
	volatile uint8_t ADCSRB_REG;    /**< ADC Control and Status Register B */
	volatile uint8_t ADMUX_REG;     /**< ADC Multiplexer Selection Register */
    volatile uint8_t DIDR2_REG;     /**< Digital Input Disable Register 2 */
	volatile uint8_t DIDR0_REG;     /**< Digital Input Disable Register 0 */
	volatile uint8_t DIDR1_REG;     /**< Digital Input Disable Register 1 */
}adc_t;

/** 
 * @brief Timers Interrupt Mask Register
 */

typedef struct {
	volatile uint8_t TIMSK_REG[TIMSK_SIZE]; /**< Interrupt Mask Register Bank */
}timer_int_mask_t;

/** 
 * @brief Timers Interrupt Flag Register
 */

typedef struct {
	volatile uint8_t TIFR_REG[TIFR_SIZE];   /**< Interrupt Flag Register Bank */
}timer_int_flag_t;

/** 
 * @brief GPIO Registers
 */

typedef struct {
	volatile uint8_t PIN_REG;   /**< Input Pins Address */
	volatile uint8_t DDR_REG;   /**< Data Direction Register */
	volatile uint8_t PORT_REG;  /**< Port Data Register */
}gpio_t;


/** 
 * @brief Timer 8-bit Register Bank
 */

typedef struct{
	volatile uint8_t TCNT_REG;  /**< Timer Counter Value Register */
	volatile uint8_t OCRA_REG;  /**< Timer Output Compare Register A */
	volatile uint8_t OCRB_REG;  /**< Timer Output Compare Register B */
}timer8_reg_bank_t;

/** 
 * @brief Timer 16-bit Register Bank
 */

typedef struct {
	volatile uint8_t TCCRC_REG; /**< Timer Control Register C */	
	uint8_t RESERVED;           /**< Reserved */
	volatile uint16_t TCNT_REG; /**< Timer Counter Value Register */
	volatile uint16_t ICR_REG;  /**< Timer Input Capture Register */
	volatile uint16_t OCRA_REG; /**< Timer Output Compare Register A */
	volatile uint16_t OCRB_REG; /**< Timer Output Compare Register B */
    volatile uint16_t OCRC_REG; /**< Timer Output Compare Register C */
}timer16_reg_bank_t;

/** 
 * @brief Generic Timer Register
 */

typedef struct {
	volatile uint8_t TCCRA_REG; /**< Timer Control Register A */
	volatile uint8_t TCCRB_REG; /**< Timer Control Register B */
	union {
		timer16_reg_bank_t TIMER16_REG;
		timer8_reg_bank_t TIMER8_REG;
	};
}timer_t;

/**
 * @}
 */

/** @addtogroup Peripheral_memory_map
 * @{
 */

#define USART0_BASE         &UCSR0A /**< USART0 BASE */
#define USART1_BASE         &UCSR1A /**< USART1 BASE */
#define USART2_BASE         &UCSR2A /**< USART2 BASE */
#define USART3_BASE         &UCSR3A /**< USART3 BASE */
#define TWI0_BASE           &TWBR   /**< TWI0 BASE */
#define SPI0_BASE           &SPCR   /**< SPI BASE */
#define TIM0_BASE           &TCCR0A /**< TIM0 BASE */
#define TIM1_BASE           &TCCR1A /**< TIM1 BASE */
#define TIM2_BASE           &TCCR2A /**< TIM2 BASE */
#define TIM3_BASE           &TCCR3A /**< TIM3 BASE */
#define TIM4_BASE           &TCCR4A /**< TIM4 BASE */
#define TIM5_BASE           &TCCR5A /**< TIM5 BASE */
#define TIM_INT_MASK_BASE   &TIMSK0 /**< TIM INT MASK BASE */
#define TIM_INT_FLAG_BASE   &TIFR0  /**< TIM INT FLAG BASE */
#define GPIOA_BASE          &PINA   /**< GPIOA BASE */
#define GPIOB_BASE          &PINB   /**< GPIOB BASE */
#define GPIOC_BASE          &PINC   /**< GPIOC BASE */
#define GPIOD_BASE          &PIND   /**< GPIOD BASE */
#define GPIOE_BASE          &PINE   /**< GPIOE BASE */
#define GPIOF_BASE          &PINF   /**< GPIOF BASE */
#define GPIOG_BASE          &PING   /**< GPIOG BASE */
#define GPIOH_BASE          &PINH   /**< GPIOH BASE */
#define GPIOJ_BASE          &PINJ   /**< GPIOJ BASE */
#define GPIOK_BASE          &PINK   /**< GPIOK BASE */
#define GPIOL_BASE          &PINL   /**< GPIOL BASE */
#define GPIO_INT_MASK_BASE  &PCMSK0 /**< GPIO INT MASK BASE */
#define ADC0_BASE			&ADCL	/**< ADC0 BASE */

/**
 * @}
 */

/** @addtogroup Peripheral_declaration
 * @{
 */

#define ADC0			((adc_t*) ADC0_BASE)
#define USART0          ((usart_t*) USART0_BASE)
#define USART1          ((usart_t*) USART1_BASE)
#define USART2          ((usart_t*) USART2_BASE)
#define USART3          ((usart_t*) USART3_BASE)
#define SPI0            ((spi_t*) SPI0_BASE)
#define TWI0            ((twi_t*) TWI0_BASE)
#define TIM0            ((timer_t*) TIM0_BASE)
#define TIM2            ((timer_t*) TIM2_BASE)
#define TIM1            ((timer_t*) TIM1_BASE)
#define TIM3            ((timer_t*) TIM3_BASE)
#define TIM4            ((timer_t*) TIM4_BASE)
#define TIM5            ((timer_t*) TIM5_BASE)
#define TIM_INT_FLAG    ((timer_int_flag_t*) TIM_INT_FLAG_BASE)
#define TIM_INT_MASK    ((timer_int_mask_t*) TIM_INT_MASK_BASE)
#define GPIOA           ((gpio_t*) GPIOA_BASE)
#define GPIOB           ((gpio_t*) GPIOB_BASE)
#define GPIOC           ((gpio_t*) GPIOC_BASE)
#define GPIOD           ((gpio_t*) GPIOD_BASE)
#define GPIOE           ((gpio_t*) GPIOE_BASE)
#define GPIOF           ((gpio_t*) GPIOF_BASE)
#define GPIOG           ((gpio_t*) GPIOG_BASE)
#define GPIOH           ((gpio_t*) GPIOH_BASE)
#define GPIOJ           ((gpio_t*) GPIOJ_BASE)
#define GPIOK           ((gpio_t*) GPIOK_BASE)
#define GPIOL           ((gpio_t*) GPIOL_BASE)
#define GPIO_INT_MASK   ((gpio_int_mask_t*) GPIO_INT_MASK_BASE)


#define SPI0_SS_PIN		_BV(0)
#define SPI0_SS_GPIO	GPIOB
#define SPI0_SCK_PIN	_BV(1)
#define SPI0_SCK_GPIO	GPIOB
#define SPI0_MOSI_PIN	_BV(2)
#define SPI0_MOSI_GPIO	GPIOB
#define SPI0_MISO_PIN	_BV(3)
#define SPI0_MISO_GPIO	GPIOB

#define TWI0_SCL_PIN	_BV(0)
#define TWI0_SCL_GPIO	GPIOD
#define TWI0_SDA_PIN	_BV(1)
#define TWI0_SDA_GPIO	GPIOD

#define USART0_RX_PIN	_BV(0)
#define USART0_RX_GPIO	GPIOE
#define USART0_TX_PIN	_BV(1)
#define USART0_TX_GPIO	GPIOE
#define USART0_XCK_PIN	_BV(2)
#define USART0_XCK_GPIO	GPIOE

#define USART1_RX_PIN	_BV(2)
#define USART1_RX_GPIO	GPIOD
#define USART1_TX_PIN	_BV(3)
#define USART1_TX_GPIO	GPIOD
#define USART1_XCK_PIN	_BV(5)
#define USART1_XCK_GPIO	GPIOD

#define USART2_RX_PIN	_BV(0)
#define USART2_RX_GPIO	GPIOH
#define USART2_TX_PIN	_BV(1)
#define USART2_TX_GPIO	GPIOH
#define USART2_XCK_PIN	_BV(2)
#define USART2_XCK_GPIO	GPIOH

#define USART3_RX_PIN	_BV(0)
#define USART3_RX_GPIO	GPIOJ
#define USART3_TX_PIN	_BV(1)
#define USART3_TX_GPIO	GPIOJ
#define USART3_XCK_PIN	_BV(2)
#define USART3_XCK_GPIO	GPIOJ

#define TIM0_OC0A_PIN	_BV(7)
#define TIM0_OC0A_GPIO	GPIOB
#define TIM0_OC0B_PIN	_BV(5)
#define TIM0_OC0B_GPIO	GPIOG
#define TIM0_T0_PIN		_BV(7)
#define TIM0_T0_GPIO	GPIOD

#define TIM1_OC1A_PIN	_BV(5)
#define TIM1_OC1A_GPIO	GPIOB
#define TIM1_OC1B_PIN	_BV(6)
#define TIM1_OC1B_GPIO	GPIOB
#define TIM1_OC1C_PIN	_BV(7)
#define TIM1_OC1C_GPIO	GPIOB
#define TIM1_ICP1_PIN	_BV(4)
#define TIM1_ICP1_GPIO	GPIOD
#define TIM1_T1_PIN		_BV(6)
#define TIM1_T1_GPIO	GPIOD

#define TIM2_OC2A_PIN	_BV(4)
#define TIM2_OC2A_GPIO	GPIOB
#define TIM2_OC2B_PIN	_BV(6)
#define TIM2_OC2B_GPIO	GPIOH
#define TIM2_TOSC1_PIN	_BV(4)
#define TIM2_TOSC1_GPIO	GPIOG
#define TIM2_TOSC2_PIN	_BV(3)
#define TIM2_TOSC2_GPIO	GPIOG

#define TIM3_OC3A_PIN	_BV(3)
#define TIM3_OC3A_GPIO	GPIOE
#define TIM3_OC3B_PIN	_BV(4)
#define TIM3_OC3B_GPIO	GPIOE
#define TIM3_OC3C_PIN	_BV(5)
#define TIM3_OC3C_GPIO	GPIOE
#define TIM3_ICP3_PIN	_BV(7)
#define TIM3_ICP3_GPIO	GPIOE
#define TIM3_T3_PIN		_BV(6)
#define TIM3_T3_GPIO	GPIOE

#define TIM4_OC4A_PIN	_BV(3)
#define TIM4_OC4A_GPIO	GPIOH
#define TIM4_OC4B_PIN	_BV(4)
#define TIM4_OC4B_GPIO	GPIOH
#define TIM4_OC4C_PIN	_BV(5)
#define TIM4_OC4C_GPIO	GPIOH
#define TIM4_ICP4_PIN	_BV(0)
#define TIM4_ICP4_GPIO	GPIOL
#define TIM4_T4_PIN		_BV(7)
#define TIM4_T4_GPIO	GPIOH

#define TIM5_OC5A_PIN	_BV(3)
#define TIM5_OC5A_GPIO	GPIOL
#define TIM5_OC5B_PIN	_BV(4)
#define TIM5_OC5B_GPIO	GPIOL
#define TIM5_OC5C_PIN	_BV(5)
#define TIM5_OC5C_GPIO	GPIOL
#define TIM5_ICP5_PIN	_BV(1)
#define TIM5_ICP5_GPIO	GPIOL
#define TIM5_T5_PIN		_BV(2)
#define TIM5_T5_GPIO	GPIOL

#define INT0_PIN		_BV(0)
#define INT0_GPIO		GPIOD
#define INT1_PIN		_BV(1)
#define INT1_GPIO		GPIOD
#define INT2_PIN		_BV(2)
#define INT2_GPIO		GPIOD
#define INT3_PIN		_BV(3)
#define INT3_GPIO		GPIOD
#define INT4_PIN		_BV(4)
#define INT4_GPIO		GPIOE
#define INT5_PIN		_BV(5)
#define INT5_GPIO		GPIOE
#define INT6_PIN		_BV(6)
#define INT6_GPIO		GPIOE
#define INT7_PIN		_BV(7)
#define INT7_GPIO		GPIOE

#define ADC0_PIN		_BV(0)
#define ADC0_GPIO		GPIOF
#define ADC1_PIN		_BV(1)
#define ADC1_GPIO		GPIOF
#define ADC2_PIN		_BV(2)
#define ADC2_GPIO		GPIOF
#define ADC3_PIN		_BV(3)
#define ADC3_GPIO		GPIOF
#define ADC4_PIN		_BV(4)
#define ADC4_GPIO		GPIOF
#define ADC5_PIN		_BV(5)
#define ADC5_GPIO		GPIOF
#define ADC6_PIN		_BV(6)
#define ADC6_GPIO		GPIOF
#define ADC7_PIN		_BV(7)
#define ADC7_GPIO		GPIOF
#define ADC8_PIN		_BV(0)
#define ADC8_GPIO		GPIOK
#define ADC9_PIN		_BV(1)
#define ADC9_GPIO		GPIOK
#define ADC10_PIN		_BV(2)
#define ADC10_GPIO		GPIOK
#define ADC11_PIN		_BV(3)
#define ADC11_GPIO		GPIOK
#define ADC12_PIN		_BV(4)
#define ADC12_GPIO		GPIOK
#define ADC13_PIN		_BV(5)
#define ADC13_GPIO		GPIOK
#define ADC14_PIN		_BV(6)
#define ADC14_GPIO		GPIOK
#define ADC15_PIN		_BV(7)
#define ADC15_GPIO		GPIOK

#define PCI0_PIN		_BV(0)
#define PCI0_GPIO		GPIOB
#define PCI1_PIN		_BV(1)
#define PCI1_GPIO		GPIOB
#define PCI2_PIN		_BV(2)
#define PCI2_GPIO		GPIOB
#define PCI3_PIN		_BV(3)
#define PCI3_GPIO		GPIOB
#define PCI4_PIN		_BV(4)
#define PCI4_GPIO		GPIOB
#define PCI5_PIN		_BV(5)
#define PCI5_GPIO		GPIOB
#define PCI6_PIN		_BV(6)
#define PCI6_GPIO		GPIOB
#define PCI7_PIN		_BV(7)
#define PCI7_GPIO		GPIOB
#define PCI8_PIN		_BV(0)
#define PCI8_GPIO		GPIOE
#define PCI9_PIN		_BV(0)
#define PCI9_GPIO		GPIOJ
#define PCI10_PIN		_BV(1)
#define PCI10_GPIO		GPIOJ
#define PCI11_PIN		_BV(2)
#define PCI11_GPIO		GPIOJ
#define PCI12_PIN		_BV(3)
#define PCI12_GPIO		GPIOJ
#define PCI13_PIN		_BV(4)
#define PCI13_GPIO		GPIOJ
#define PCI14_PIN		_BV(5)
#define PCI14_GPIO		GPIOJ
#define PCI15_PIN		_BV(6)
#define PCI15_GPIO		GPIOJ
#define PCI16_PIN		_BV(0)
#define PCI16_GPIO		GPIOK
#define PCI17_PIN		_BV(1)
#define PCI17_GPIO		GPIOK
#define PCI18_PIN		_BV(2)
#define PCI18_GPIO		GPIOK
#define PCI19_PIN		_BV(3)
#define PCI19_GPIO		GPIOK
#define PCI20_PIN		_BV(4)
#define PCI20_GPIO		GPIOK
#define PCI21_PIN		_BV(5)
#define PCI21_GPIO		GPIOK
#define PCI22_PIN		_BV(6)
#define PCI22_GPIO		GPIOK
#define PCI23_PIN		_BV(7)
#define PCI23_GPIO		GPIOK

/**
 * @}
 */


#endif