/**
 * @file hal_adc.c
 * @author Matheus Alencar Nascimento (matt-alencar)
 * @brief This file provides firmware functions to manage the following 
 *        functionalities of the Analog to Digital Convertor (ADC) peripheral:
 *           + Initialization and de-initialization functions
 *             ++ Initialization and Configuration of ADC
 *           + Operation functions
 *             ++ Start Conversion
 *           + Control functions
 *             ++ Channel slot configuration
 *             ++ Select channel slot and switch ADC MUX
 *           + State functions
 *             ++ ADC state machine management
 *             ++ Interrupts and flags management
 * 
 **************************************************************************
 * @copyright MIT License.
 * 
 */

#include "hal_adc.h"


void ADC_IRQHandler(adc_handler_t *handler){
	uint16_t result = handler->Instance->ADC_REG;
	handler->State = ADC_STATE_READY;
	
	if(handler->InCalibration){
		handler->InCalibration = 0; // Drop result
	}
	else{
		handler->ChannelSlot[handler->SlotId].ConvResult = result;
		handler->ChannelSlot[handler->SlotId].NewResultFlag = 1;

		if(handler->ConvCpltCallback != NULL){
			handler->ConvCpltCallback(handler, handler->SlotId);
		}
	}
}

hal_status_t ADC_Init(adc_handler_t *handler){
	__HAL_LOCK(handler);

	handler->Instance->ADCSRB_REG = handler->Init.AutoTrigMode;
	handler->Instance->ADMUX_REG = (handler->Init.Reference << REFS0) | (handler->Init.DataAlign << ADLAR);
	handler->Instance->ADCSRA_REG = _BV(ADEN) | (handler->Init.AutoTrigState << ADATE) | _BV(ADIE) | handler->Init.ClockPrescaler;
	handler->SlotId = 0;
	handler->State = ADC_STATE_BUSY_INTERNAL;
	handler->InCalibration = 1;
	handler->Instance->ADCSRA_REG |= _BV(ADSC); // Start calibration process

	__HAL_UNLOCK(handler);
	return HAL_OK;
}

hal_status_t ADC_DeInit(adc_handler_t *handler){
	__HAL_LOCK(handler);

	while(handler->State != ADC_STATE_READY);

	handler->Instance->ADCSRA_REG = 0;
	handler->Instance->ADCSRB_REG = 0;
	handler->Instance->ADMUX_REG = 0;
	handler->State = ADC_STATE_RESET;
	handler->SlotId = 0;
	
	__HAL_UNLOCK(handler);
	return HAL_OK;
}

adc_state_t ADC_GetState(adc_handler_t *handler){
	return handler->State;
}

hal_status_t ADC_ConfigChannelSlot(adc_handler_t *handler, uint8_t SlotId, uint8_t ADChannel){
	if(SlotId >= ADC_SLOTS_SIZE){
		return HAL_ERROR;
	}
	handler->ChannelSlot[SlotId].Channel = ADChannel;
	handler->ChannelSlot[SlotId].NewResultFlag = 0;
	return HAL_OK;
}

hal_status_t ADC_SelectChannelSlot(adc_handler_t *handler, uint8_t SlotId){
	/*
	 *	We are changing the MUX register blindly without knowing if a conversion
	 *	have already started, thus, we can't ensure if next conversion is based
	 *	on the old or the new settings if auto trigger in free running is turn on
	**/
	
	if(SlotId >= ADC_SLOTS_SIZE){
		return HAL_ERROR;
	}
	if(handler->State != ADC_STATE_READY){
		return HAL_BUSY;
	}
	__HAL_LOCK(handler);
	handler->SlotId = SlotId;
	handler->Instance->ADMUX_REG &= ~(0x1F);
	handler->Instance->ADCSRB_REG &= ~_BV(MUX5);
	handler->Instance->ADMUX_REG |= (handler->ChannelSlot[SlotId].Channel & 0x0F);
	handler->Instance->ADCSRB_REG |= (handler->ChannelSlot[SlotId].Channel & _BV(MUX5) ? _BV(MUX5) : 0x00);	
	__HAL_UNLOCK(handler);
	return HAL_OK;
}

hal_status_t ADC_GetChannelSlotValue(adc_handler_t *handler, uint8_t SlotId, uint16_t *pResult){
	if(SlotId >= ADC_SLOTS_SIZE){
		return HAL_ERROR;
	}
	if(handler->ChannelSlot[SlotId].NewResultFlag){
		handler->ChannelSlot[SlotId].NewResultFlag = 0;
		*pResult = handler->ChannelSlot[SlotId].ConvResult;
		return HAL_OK;
	}
	return HAL_ERROR;
}

void ADC_SetCalibration(adc_handler_t *handler){
	handler->InCalibration = 1;
}

hal_status_t ADC_EnableAutoTrigger(adc_handler_t *handler, adc_trigger_t trigger){
	__HAL_LOCK(handler);
	handler->Init.AutoTrigState = ADC_AUTO_TRIG_ON;
	handler->Init.AutoTrigMode = trigger;
	handler->Instance->ADCSRB_REG &= ~(0x07);
	handler->Instance->ADCSRB_REG |= handler->Init.AutoTrigMode;
	handler->Instance->ADCSRA_REG |= (handler->Init.AutoTrigState << ADATE);
	__HAL_UNLOCK(handler);
	return HAL_OK;
}

hal_status_t ADC_DisableAutoTrigger(adc_handler_t *handler){
	__HAL_LOCK(handler);
	handler->Init.AutoTrigState = ADC_AUTO_TRIG_OFF;
	handler->Instance->ADCSRA_REG &= ~_BV(ADATE);
	__HAL_UNLOCK(handler);
	return HAL_OK;
}

hal_status_t ADC_StartConv(adc_handler_t *handler){
	if(handler->State != ADC_STATE_READY){
		return HAL_BUSY;
	}
	__HAL_LOCK(handler);
	handler->State = ADC_STATE_BUSY;
	handler->Instance->ADCSRA_REG |= _BV(ADSC);
	__HAL_UNLOCK(handler);
	return HAL_OK;
}


void ADC_RegisterCallback(adc_handler_t *handler, ConvCpltCallback_t pCallback){
	handler->ConvCpltCallback = pCallback;
}

void ADC_UnRegisterCallback(adc_handler_t *handler){
	handler->ConvCpltCallback = NULL;
}