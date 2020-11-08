#include "hal_usart.h"
#include "hal_gpio.h"
#include <math.h>
#include <stdlib.h>
#include <string.h>


static uint16_t USART_CalcUBRR(uint32_t value, uint8_t div){
	return (0x0FFF & (uint16_t)lrint(((F_CPU/div)/(double)value)-1));
}

static void USART_SetupGPIO(usart_handler_t *handler){
	
	if(handler->Instance == USART0){
		if(handler->Init.Mode == USART_MODE_SPI_MASTER || handler->Init.Mode == USART_MODE_SYNC_MASTER){
			GPIO_PinMode(USART0_XCK_GPIO, USART0_XCK_PIN, GPIO_MODE_OUTPUT);
		}
		else if(handler->Init.Mode == USART_MODE_SYNC_SLAVE){
			GPIO_PinMode(USART0_XCK_GPIO, USART0_XCK_PIN, GPIO_MODE_INPUT);
		}
		GPIO_SetPin(USART0_RX_GPIO, USART0_RX_PIN); // Setup internal pull-up
	}
	else if(handler->Instance == USART1){
		if(handler->Init.Mode == USART_MODE_SPI_MASTER || handler->Init.Mode == USART_MODE_SYNC_MASTER){
			GPIO_PinMode(USART1_XCK_GPIO, USART1_XCK_PIN, GPIO_MODE_OUTPUT);
		}
		else if(handler->Init.Mode == USART_MODE_SYNC_SLAVE){
			GPIO_PinMode(USART1_XCK_GPIO, USART1_XCK_PIN, GPIO_MODE_INPUT);
		}
		GPIO_SetPin(USART1_RX_GPIO, USART1_RX_PIN); // Setup internal pull-up
	}
	else if(handler->Instance == USART2){
		if(handler->Init.Mode == USART_MODE_SPI_MASTER || handler->Init.Mode == USART_MODE_SYNC_MASTER){
			GPIO_PinMode(USART2_XCK_GPIO, USART2_XCK_PIN, GPIO_MODE_OUTPUT);
		}
		else if(handler->Init.Mode == USART_MODE_SYNC_SLAVE){
			GPIO_PinMode(USART2_XCK_GPIO, USART2_XCK_PIN, GPIO_MODE_INPUT);
		}
		GPIO_SetPin(USART2_RX_GPIO, USART2_RX_PIN); // Setup internal pull-up
	}
	else if(handler->Instance == USART3){
		if(handler->Init.Mode == USART_MODE_SPI_MASTER || handler->Init.Mode == USART_MODE_SYNC_MASTER){
			GPIO_PinMode(USART3_XCK_GPIO, USART3_XCK_PIN, GPIO_MODE_OUTPUT);
		}
		else if(handler->Init.Mode == USART_MODE_SYNC_SLAVE){
			GPIO_PinMode(USART3_XCK_GPIO, USART3_XCK_PIN, GPIO_MODE_INPUT);
		}
		GPIO_SetPin(USART3_RX_GPIO, USART3_RX_PIN); // Setup internal pull-up
	}
}


void USART_IRQTxHandler(usart_handler_t *handler){
	if(handler->State == USART_STATE_BUSY_TX){
		if(handler->TxIndex < handler->TxBuffSize) {
			handler->Instance->UDR_REG = handler->TxBuffPtr[handler->TxIndex++];
		}
		else {
			handler->State = USART_STATE_READY;
			if(handler->TxCpltCallback != NULL)
				handler->TxCpltCallback(handler);
		}
	}
}

void USART_IRQRxHandler(usart_handler_t *handler){
	uint8_t hw_flags = handler->Instance->UCSRA_REG;
	uint8_t op_mode = handler->Instance->UCSRC_REG & 0xC0;
	uint8_t err_flags = op_mode != 0xC0 ? (hw_flags & (_BV(UPE0) | _BV(FE0) | _BV(DOR0))) : USART_ERR_NONE;
	handler->ErrCode = err_flags;
	uint8_t rx_data = handler->Instance->UDR_REG;

	// Call RX error handler
	if(err_flags > USART_ERR_NONE){
		if(handler->RxErrorCallback != NULL)
			handler->RxErrorCallback(handler);
	}
	else {

		if(handler->RxByteCallback != NULL){
			handler->RxByteCallback(handler);
		}

		if(handler->RxBufferPtr != NULL){
			uint8_t next = (handler->RxWriteIndex + 1) % handler->RxBuffSize;

			// Overflow condition
			if(next == handler->RxReadIndex){
				if(handler->RxBuffOvfCallback != NULL)
					handler->RxBuffOvfCallback(handler);
			}

			handler->RxBufferPtr[handler->RxWriteIndex] = rx_data;
			handler->RxWriteIndex = next;
		}
		else {
			handler->ErrCode = USART_ERR_NULL_RX_BUFFER;
		}
	}
}


hal_status_t USART_Init(usart_handler_t *handler){
	hal_status_t ret_code = HAL_OK;
	handler->State = USART_STATE_BUSY;
	__HAL_LOCK(handler);

	if(handler->Init.Mode == USART_MODE_SPI_MASTER){
		handler->Instance->UBRR_REG = 0;
		handler->Instance->UCSRA_REG = 0;
		handler->Instance->UCSRC_REG = (_BV(UMSEL01) | _BV(UMSEL00) | (handler->Init.BitOrder << 2) | handler->Init.CLKPhase << 1 | handler->Init.CLKPolarity);
		handler->Instance->UBRR_REG = USART_CalcUBRR(handler->Init.BaudRate, 2);
	}
	else {
		uint8_t mode = (handler->Init.Mode == USART_MODE_SYNC_MASTER || handler->Init.Mode == USART_MODE_SYNC_SLAVE);
		handler->Instance->UBRR_REG = USART_CalcUBRR(handler->Init.BaudRate, handler->Init.OverSampling);
		handler->Instance->UCSRA_REG = handler->Init.OverSampling == USART_OVERSAMPLING_8 ? _BV(U2X0) : 0x00;
		handler->Instance->UCSRC_REG = ((handler->Init.Parity << 4) | (handler->Init.StopBits << 3) | (handler->Init.WordLenght << 1));
		handler->Instance->UCSRC_REG |= (mode ? _BV(UMSEL00) : 0);
		handler->Instance->UCSRB_REG = (_BV(RXCIE0) | _BV(TXCIE0) | _BV(RXEN0) | _BV(TXEN0));
	}

	USART_SetupGPIO(handler);

	handler->ErrCode = USART_ERR_NONE;
	handler->TxIndex = 0;
	handler->RxReadIndex = 0;
	handler->RxWriteIndex = 0;
	handler->State = USART_STATE_READY;
	__HAL_UNLOCK(handler);
	return ret_code;
}

void USART_DeInit(usart_handler_t *handler){
	handler->State = USART_STATE_BUSY;
	while(!(handler->Instance->UCSRA_REG & _BV(UDRE0))); // Wait last byte transmission
	handler->Instance->UCSRB_REG = 0;
	handler->Instance->UCSRA_REG = _BV(TXC0);
	handler->Instance->UCSRC_REG = 0;
	handler->Instance->UBRR_REG = 0;
	handler->State = USART_STATE_RESET;
}

void USART_SetRxBuff(usart_handler_t *handler, uint8_t *pBuff, uint16_t Size){
	handler->Instance->UCSRB_REG &= ~_BV(RXCIE0); // disable RX interrupts for prevent corruption
	handler->RxReadIndex = 0;
	handler->RxWriteIndex = 0;
	handler->RxBufferPtr = pBuff;
	handler->RxBuffSize = Size;
	handler->Instance->UCSRB_REG |= _BV(RXCIE0);
}

usart_state_t USART_GetState(usart_handler_t *handler){
	return handler->State;
}

uint8_t USART_GetError(usart_handler_t *handler){
	return handler->ErrCode;
}

hal_status_t USART_Transmit(usart_handler_t *handler, uint8_t *pData, uint16_t Size){
	if(pData == NULL || Size == 0){
		return HAL_ERROR;
	}
	
	if(handler->State == USART_STATE_BUSY || handler->State == USART_STATE_BUSY_TX){
		return HAL_BUSY;
	}

	handler->TxBuffPtr = pData;
	handler->TxBuffSize = Size;
	handler->TxIndex = 0;
	handler->State = USART_STATE_BUSY_TX;
	handler->Instance->UDR_REG = *(handler->TxBuffPtr); // Fire up transmission
	handler->TxIndex++;
	return HAL_OK;
}

void USART_ResetRxBuffer(usart_handler_t *handler){
	handler->Instance->UCSRB_REG &= ~_BV(RXCIE0); // disable RX interrupts for prevent corruption
	handler->RxReadIndex = 0;
	handler->RxWriteIndex = 0;
	handler->Instance->UCSRB_REG |= _BV(RXCIE0);
}

uint8_t USART_GetRxBytes(usart_handler_t *handler){
	if(handler->RxWriteIndex >= handler->RxReadIndex)
		return handler->RxWriteIndex - handler->RxReadIndex;
	else
		return handler->RxBuffSize + handler->RxWriteIndex - handler->RxReadIndex;
}

hal_status_t USART_ReceiveByte(usart_handler_t *handler, uint8_t *pData){
	if(pData == NULL)
		return HAL_ERROR;
	
	if(handler->RxWriteIndex == handler->RxReadIndex) // Empty buffer
		return HAL_ERROR;

	uint8_t next = (handler->RxReadIndex + 1) % handler->RxBuffSize;

	*pData = handler->RxBufferPtr[handler->RxReadIndex];
	handler->RxReadIndex = next;
	return HAL_OK;
}

hal_status_t USART_Receive(usart_handler_t *handler, uint8_t *pData, uint16_t Size){
	// Invalid inputs
	if(pData == NULL || Size == 0){
		return HAL_ERROR;
	}
	
	uint8_t rxbytes = USART_GetRxBytes(handler);

	if(Size > rxbytes || Size > handler->RxBuffSize) // Empty buffer
		return HAL_ERROR;
	
	uint8_t i, next=0;
	for(i=0; i<Size; i++){
		next = (handler->RxReadIndex + 1) % handler->RxBuffSize;
		*pData++ = handler->RxBufferPtr[handler->RxReadIndex];
		handler->RxReadIndex = next;
	}
	
	return HAL_OK;
}

hal_status_t USART_Peek(usart_handler_t *handler, uint8_t *pData){
	// Invalid inputs
	if(pData == NULL){
		return HAL_ERROR;
	}
	// Empty buffer
	if(handler->RxWriteIndex == handler->RxReadIndex){
		return HAL_ERROR;
	}

	*pData = handler->RxBufferPtr[handler->RxReadIndex];
	
	return HAL_OK;
}

hal_status_t USART_RegisterCallback(usart_handler_t *handler, usart_isr_t isr_type, usart_callback_t callback){
	switch(isr_type){
		case USART_ISR_TX_DONE:
			handler->TxCpltCallback = callback;
		break;

		case USART_ISR_RX_OVF:
			handler->RxBuffOvfCallback = callback;
		break;

		case USART_ISR_RX_ERROR:
			handler->RxErrorCallback = callback;
		break;

		case USART_ISR_RX_BYTE:
			handler->RxByteCallback = callback;
		break;

		default:
			return HAL_ERROR;
	}

	return HAL_OK;
}

hal_status_t USART_UnRegisterCallback(usart_handler_t *handler, usart_isr_t isr_type){
	switch(isr_type){
		case USART_ISR_TX_DONE:
			handler->TxCpltCallback = NULL;
		break;

		case USART_ISR_RX_OVF:
			handler->RxBuffOvfCallback = NULL;
		break;

		case USART_ISR_RX_ERROR:
			handler->RxErrorCallback = NULL;
		break;

		case USART_ISR_RX_BYTE:
			handler->RxByteCallback = NULL;
		break;

		default:
			return HAL_ERROR;
	}

	return HAL_OK;
}