/**
 * @file hal_adc.h
 * @author Matheus Alencar Nascimento (matt-alencar)
 * @brief Header file containing functions prototypes of ADC HAL library.
 **************************************************************************
 * @copyright MIT License.
 * 
 */

#ifndef _ADC_DRIVER_H_
#define _ADC_DRIVER_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "hal_def.h"

/**
 * @brief Define channel slot size that will be used by ADC handler struct
 * @note This parameter is user defined, it depends on how many ADC inputs will be used
 */
#ifndef ADC_SLOTS_SIZE
	#define ADC_SLOTS_SIZE 8
#endif


/**
 * @brief ADC state machine: ADC states definition
 */
typedef enum {
	ADC_STATE_RESET,			/*!< ADC not yet initialized or disabled */
	ADC_STATE_READY,			/*!< ADC peripheral ready for use */
	ADC_STATE_BUSY_INTERNAL,	/*!< ADC is busy to internal process (initialization, calibration) */
	ADC_STATE_BUSY				/*!< A conversion is ongoing or can occur by auto trigger event */
}adc_state_t;

/**
 * @brief ADC Voltage Ref
 */
typedef enum {
	ADC_REF_AREF,		/*!< AREF pin is used for ADC voltage reference */
	ADC_REF_AVCC,		/*!< AVCC pin is used for ADC voltage reference */
	ADC_REF_1V1,		/*!< Internal 1.1V is used for ADC voltage reference */
	ADC_REF_2V56		/*!< Internal 2.56V is used for ADC voltage reference */
}adc_ref_t;

/**
 * @brief ADC Data Alignment
 */
typedef enum {
	ADC_ALIGN_RIGHT,
	ADC_ALIGN_LEFT,
}adc_align_t;

/**
 * @brief ADC Clock Prescaller
 */
typedef enum {
	ADC_PRESC_DIV2 = 1,
	ADC_PRESC_DIV4,
	ADC_PRESC_DIV8,
	ADC_PRESC_DIV16,
	ADC_PRESC_DIV32,
	ADC_PRESC_DIV64,
	ADC_PRESC_DIV128,
}adc_presc_t;

/**
 * @brief ADC Single Ended Input Channels
 */
typedef enum {
	ADC_CH0,
	ADC_CH1,
	ADC_CH2,
	ADC_CH3,
	ADC_CH4,
	ADC_CH5,
	ADC_CH6,
	ADC_CH7,
	ADC_CH8 = 0x10,
	ADC_CH9,
	ADC_CH10,
	ADC_CH11,
	ADC_CH12,
	ADC_CH13,
	ADC_CH14,
	ADC_CH15,
	ADC_CH_1V1 = 0x1E,
	ADC_CH_GND,
}adc_single_ended_channel_t;

/**
 * @brief ADC Differential Inputs Channels
 */
typedef enum {
	ADC_CH0P_CH0N_10X = 0x08,
	ADC_CH1P_CH0N_10X,
	ADC_CH0P_CH0N_200X,
	ADC_CH1P_CH0N_200X,
	ADC_CH2P_CH2N_10X,
	ADC_CH3P_CH2N_10X,
	ADC_CH2P_CH2N_200X,
	ADC_CH3P_CH2N_200X,
	ADC_CH0P_CH1N_1X,
	ADC_CH1P_CH1N_1X,
	ADC_CH2P_CH1N_1X,
	ADC_CH3P_CH1N_1X,
	ADC_CH4P_CH1N_1X,
	ADC_CH5P_CH1N_1X,
	ADC_CH6P_CH1N_1X,
	ADC_CH7P_CH1N_1X,
	ADC_CH0P_CH2N_1X,
	ADC_CH1P_CH2N_1X,
	ADC_CH2P_CH2N_1X,
	ADC_CH3P_CH2N_1X,
	ADC_CH4P_CH2N_1X,
	ADC_CH5P_CH2N_1X,
	ADC_CH8P_CH8N_10X = 0x28,
	ADC_CH9P_CH8N_10X,
	ADC_CH8P_CH8N_200X,
	ADC_CH9P_CH8N_200X,
	ADC_CH10P_CH10N_10X,
	ADC_CH11P_CH10N_10X,
	ADC_CH10P_CH10N_200X,
	ADC_CH11P_CH10N_200X,
	ADC_CH8P_CH9N_1X,
	ADC_CH9P_CH9N_1X,
	ADC_CH10P_CH9N_1X,
	ADC_CH11P_CH9N_1X,
	ADC_CH12P_CH9N_1X,
	ADC_CH13P_CH9N_1X,
	ADC_CH14P_CH9N_1X,
	ADC_CH15P_CH9N_1X,
	ADC_CH8P_CH10N_1X,
	ADC_CH9P_CH10N_1X,
	ADC_CH10P_CH10N_1X,
	ADC_CH11P_CH10N_1X,
	ADC_CH12P_CH10N_1X,
	ADC_CH13P_CH10N_1X,
}adc_diff_channel_t;


/**
 * @brief ADC Auto Trigger State
 */
typedef enum {
	ADC_AUTO_TRIG_OFF,
	ADC_AUTO_TRIG_ON,
}adc_auto_trigger_state_t;


/**
 * @brief ADC Auto Trigger Mode
 */
typedef enum {
	ADC_TRIG_FREERUN,
	ADC_TRIG_ANALOG_COMP,
	ADC_TRIG_EXTI0,
	ADC_TRIG_TIM0_COMP_A,
	ADC_TRIG_TIM0_OVF,
	ADC_TRIG_TIM1_COMP_B,
	ADC_TRIG_TIM1_OVF,
	ADC_TRIG_TIM1_INPUT_COMP,
}adc_trigger_t;


/**
 * @brief ADC Slot Struct definition
 */
typedef struct {
	uint8_t Channel;			/*!< ADC channel MUX value */
	uint16_t ConvResult;		/*!< Last Conversion value */
	uint8_t NewResultFlag;		/*!< New conversion result flag */
}adc_slot_t;

/**
 * @brief Structure definition of ADC initialization
 */
typedef struct {
	adc_ref_t Reference;						/*!< ADC voltage reference */
	adc_presc_t ClockPrescaler;					/*!< ADC clock rrescaller */
	adc_align_t DataAlign;						/*!< ADC Result alignment */
	adc_trigger_t AutoTrigMode;					/*!< Auto trigger mode */
	adc_auto_trigger_state_t AutoTrigState;		/*!< Auto trigger state */
}adc_init_t;

/**
 * @brief ADC handle Structure definition
 */
typedef struct _adc_handler {
	adc_t *Instance;														/*!< Register base address */
	adc_init_t Init;														/*!< ADC required parameters */
	hal_lock_t Lock;														/*!< ADC locking object */
	volatile uint8_t InCalibration;											/*!< ADC InCalibration State */
	volatile adc_state_t State;												/*!< ADC State */
	volatile adc_slot_t ChannelSlot[ADC_SLOTS_SIZE];						/*!< Slot Channel Array */
	uint8_t SlotId;															/*!< Selected slot channel ID */
	void (*ConvCpltCallback)(struct _adc_handler *handler, uint8_t SlotID);	/*!< ADC conversion complete callback */
}adc_handler_t;


/**
 * @brief ADC Conversion Complete Callback TypeDef
 */
typedef void (*ConvCpltCallback_t)(adc_handler_t *handler, uint8_t SlotID);


/**
 * @brief ADC IRQ Handler function
 * @note Should be called on ADC IRQ subroutine
 * 
 * @param handler ADC Handler Pointer
 */
void ADC_IRQHandler(adc_handler_t *handler);


/**
 * @brief Initializes the ADC peripheral according to parameters specified 
 *        in structure "adc_init_t"
 * 
 * @param handler ADC Handler Pointer
 * @return HAL Status
 */
hal_status_t ADC_Init(adc_handler_t *handler);


/**
 * @brief Deinitialize the ADC peripheral registers to their default reset
 * 
 * @param handler ADC Handler Pointer
 * @return HAL Status
 */
hal_status_t ADC_DeInit(adc_handler_t *handler);


/**
 * @brief Return the ADC state
 * 
 * @see adc_state_t
 * 
 * @param handler ADC Handler Pointer
 * @return ADC State
 */
adc_state_t ADC_GetState(adc_handler_t *handler);


/**
 * @brief Setup internal slot array item with a specific ADC channel
 * 
 * @see ADC_SLOTS_SIZE
 * @see adc_single_ended_channel_t
 * @see adc_diff_channel_t
 * 
 * @param handler ADC Handler Pointer
 * @param SlotId Slot ID, should be smaller than ADC_SLOTS_SIZE
 * @param ADChannel ADC MUX channel, should be anything on "adc_diff_channel_t" or "adc_single_ended_channel_t"
 * @return HAL Status
 */
hal_status_t ADC_ConfigChannelSlot(adc_handler_t *handler, uint8_t SlotId, uint8_t ADChannel);

/**
 * @brief Configure ADC MUX with selected slot id and prepare for next conversion
 * 
 * @see ADC_ConfigChannelSlot function
 * @see ADC_SLOTS_SIZE
 * 
 * @param handler ADC Handler Pointer
 * @param SlotId Slot ID, should be smaller than ADC_SLOTS_SIZE
 * @return HAL Status
 */
hal_status_t ADC_SelectChannelSlot(adc_handler_t *handler, uint8_t SlotId);

/**
 * @brief Return last conversion result from slot id
 * 
 * @see ADC_SLOTS_SIZE
 * 
 * @param handler ADC Handler Pointer
 * @param SlotId Slot ID, should be smaller than ADC_SLOTS_SIZE
 * @param[out] pResult Pointer for uint16_t variable
 * @return HAL Status: HAL_OK if result is available, otherwise HAL_ERROR
 */
hal_status_t ADC_GetChannelSlotValue(adc_handler_t *handler, uint8_t SlotId, uint16_t *pResult);


/**
 * @brief Discard next conversion, used for peripheral initialization
 * 
 * @param handler ADC Handler Pointer
 */
void ADC_SetCalibration(adc_handler_t *handler);


/**
 * @brief Enable ADC auto trigger
 * 
 * @see adc_trigger_t
 * 
 * @param handler ADC Handler Pointer
 * @param trigger Trigger mode: Options are available on "adc_trigger_t"
 * @return HAL Status 
 */
hal_status_t ADC_EnableAutoTrigger(adc_handler_t *handler, adc_trigger_t trigger);


/**
 * @brief Disable ADC auto trigger
 * 
 * @param handler ADC Handler Pointer
 * @return HAL Status
 */
hal_status_t ADC_DisableAutoTrigger(adc_handler_t *handler);


/**
 * @brief Start ADC conversion
 * @note After conversion is over the result will be saved on last selected slot id
 * 
 * @see ADC_ConfigChannelSlot
 * @see ADC_SelectChannelSlot
 * 
 * @param handler ADC Handler Pointer
 * @return HAL Status
 */
hal_status_t ADC_StartConv(adc_handler_t *handler);


/**
 * @brief Register user conversion complete callback
 * 
 * @see ConvCpltCallback_t
 * 
 * @param handler ADC Handler Pointer
 * @param pCallback Pointer to the Callback function
 */
void ADC_RegisterCallback(adc_handler_t *handler, ConvCpltCallback_t pCallback);


/**
 * @brief Unregister conversion complete callback
 * 
 * @param handler ADC Handler Pointer
 */
void ADC_UnRegisterCallback(adc_handler_t *handler);


#ifdef __cplusplus
}
#endif

#endif /* _ADC_DRIVER_H_ */