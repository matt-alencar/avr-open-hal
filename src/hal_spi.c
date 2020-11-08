#include "hal_spi.h"


static hal_status_t SPI_SetupGPIO(spi_handler_t *handler){
	hal_status_t retCode = HAL_OK;

	if(handler->Instance == SPI0){
		if(handler->Init.Mode == SPI_MODE_SLAVE){
			GPIO_PinMode(SPI0_SS_GPIO, SPI0_SS_PIN, GPIO_MODE_INPUT_PULLUP);
			GPIO_PinMode(SPI0_SCK_GPIO, SPI0_SCK_PIN, GPIO_MODE_INPUT_PULLUP);
			GPIO_PinMode(SPI0_MOSI_GPIO, SPI0_MOSI_PIN, GPIO_MODE_INPUT_PULLUP);
			GPIO_PinMode(SPI0_MISO_GPIO, SPI0_MISO_PIN, GPIO_MODE_OUTPUT);
		}
		else if(handler->Init.Mode == SPI_MODE_MASTER){
			GPIO_SetPin(SPI0_SS_GPIO, SPI0_SS_PIN);
			GPIO_PinMode(SPI0_SS_GPIO, SPI0_SS_PIN, GPIO_MODE_OUTPUT);
			GPIO_PinMode(SPI0_SCK_GPIO, SPI0_SCK_PIN, GPIO_MODE_OUTPUT);
			GPIO_PinMode(SPI0_MOSI_GPIO, SPI0_MOSI_PIN, GPIO_MODE_OUTPUT);
			GPIO_PinMode(SPI0_MISO_GPIO, SPI0_MISO_PIN, GPIO_MODE_INPUT_PULLUP);
		}
		else {
			retCode = HAL_ERROR;
		}
	}
	else {
		retCode = HAL_ERROR;
	}

	return retCode;
}


void SPI_IRQHandler(spi_handler_t *handler){
	// uint8_t error = handler->Instance->SPSR_REG & _BV(WCOL);
	uint8_t recv_data = handler->Instance->SPDR_REG;
	// uint8_t mode = handler->Instance->SPCR_REG & _BV(MSTR);

	if(handler->State == SPI_STATE_ABORT){
		handler->State = SPI_STATE_READY;
	}
	else if(handler->State == SPI_STATE_BUSY_RX){
		(*handler->RxBuffPtr) = recv_data;
		handler->RxBuffPtr++;
		handler->RxCount--;

		if(handler->RxCount == 0){
			handler->State = SPI_STATE_READY;
			if(handler->RxCpltCallback != NULL)
				handler->RxCpltCallback(handler);
		}
		else {
			handler->Instance->SPDR_REG = 0;	// Send dummy byte on next transmit
		}
	}
	else if (handler->State == SPI_STATE_BUSY_TX_RX){
		if(handler->TxCount != 0){
			handler->Instance->SPDR_REG = (*++handler->TxBuffPtr);
			handler->TxCount--;
		}

		(*handler->RxBuffPtr) = recv_data;
		handler->RxBuffPtr++;
		handler->RxCount--;

		if(handler->RxCount == 0){
			handler->State = SPI_STATE_READY;
			if(handler->TxRxCpltCallback != NULL)
				handler->TxRxCpltCallback(handler);
		}
	}
	else if(handler->State == SPI_STATE_BUSY_TX){
		handler->Instance->SPDR_REG = (*++handler->TxBuffPtr);
		handler->TxCount--;

		if(handler->TxCount == 0){
			handler->State = SPI_STATE_READY;
			if(handler->TxCpltCallback != NULL)
				handler->TxCpltCallback(handler);
		}
	}
}

hal_status_t SPI_Init(spi_handler_t *handler){
	handler->State = SPI_STATE_BUSY;
	hal_status_t errCode = HAL_OK;

	if(handler->Instance == NULL){
		return HAL_ERROR;
	}

	__HAL_LOCK(handler);

	errCode = SPI_SetupGPIO(handler);
	if(errCode == HAL_ERROR)
		goto error;
	
	uint8_t spcr_tmp = _BV(SPIE) | _BV(SPE);
	spcr_tmp |= (handler->Init.BitOrder << DORD);
	spcr_tmp |= (handler->Init.Mode << MSTR);
	spcr_tmp |= (handler->Init.CLKPolarity << CPOL);
	spcr_tmp |= (handler->Init.CLKPhase << CPHA);
	spcr_tmp |= (handler->Init.ClockPresc & 0x03);
	
	handler->Instance->SPCR_REG = spcr_tmp;
	handler->Instance->SPSR_REG = ((handler->Init.ClockPresc & 0x04) >> 2);

	handler->ErrorCode = 0;
	handler->State = SPI_STATE_READY;

	error:
	__HAL_UNLOCK(handler);
	return errCode;
}

void SPI_DeInit(spi_handler_t *handler){
	handler->State = SPI_STATE_BUSY;
	handler->Instance->SPCR_REG = 0;
	handler->Instance->SPSR_REG = 0;
	handler->State = SPI_STATE_RESET;
	handler->ErrorCode = 0;
	__HAL_UNLOCK(handler);
}

spi_state_t SPI_GetState(spi_handler_t *handler){
	return handler->State;
}

uint8_t SPI_GetError(spi_handler_t *handler){
	return handler->ErrorCode;
}

hal_status_t SPI_TransmitReceive(spi_handler_t *handler, uint8_t *pTxData, uint8_t *pRxData, uint8_t Size){
	hal_status_t errCode = HAL_OK;

	__HAL_LOCK(handler);
	if(pTxData == NULL || pRxData == NULL || Size == 0){
		errCode = HAL_ERROR;
		goto error;
	}
	if(handler->State != SPI_STATE_READY){
		errCode = HAL_BUSY;
		goto error;
	}

	handler->TxBuffPtr = pTxData;
	handler->TxBuffSize = Size;
	handler->TxCount = Size-1;

	handler->RxBuffPtr = pRxData;
	handler->RxBuffSize = Size;
	handler->RxCount = Size;

	handler->State = SPI_STATE_BUSY_TX_RX;
	handler->ErrorCode = 0;
	handler->Instance->SPCR_REG |= (_BV(SPIE) | _BV(SPE));
	handler->Instance->SPDR_REG = *(pTxData); // Fire up transmission

	error:
	__HAL_UNLOCK(handler);
	return errCode;
}

hal_status_t SPI_Transmit(spi_handler_t *handler, uint8_t *pData, uint8_t Size){
	hal_status_t errCode = HAL_OK;

	__HAL_LOCK(handler);
	if(pData == NULL || Size == 0){
		errCode = HAL_ERROR;
		goto error;
	}
	if(handler->State != SPI_STATE_READY){
		errCode = HAL_BUSY;
		goto error;
	}

	handler->TxBuffPtr = pData;
	handler->TxBuffSize = Size;
	handler->TxCount = Size-1;

	handler->RxBuffPtr = NULL;
	handler->RxBuffSize = 0;
	handler->RxCount = 0;

	handler->State = SPI_STATE_BUSY_TX;
	handler->ErrorCode = 0;
	handler->Instance->SPCR_REG |= (_BV(SPIE) | _BV(SPE));
	handler->Instance->SPDR_REG = *(pData); // Fire up transmission

	error:
	__HAL_UNLOCK(handler);
	return errCode;
}

hal_status_t SPI_Receive(spi_handler_t *handler, uint8_t *pData, uint8_t Size){
	hal_status_t errCode = HAL_OK;

	__HAL_LOCK(handler);
	if(pData == NULL || Size == 0){
		errCode = HAL_ERROR;
		goto error;
	}
	if(handler->State != SPI_STATE_READY){
		errCode = HAL_BUSY;
		goto error;
	}

	handler->RxBuffPtr = pData;
	handler->RxBuffSize = Size;
	handler->RxCount = Size;

	handler->TxBuffPtr = NULL;
	handler->TxBuffSize = 0;
	handler->TxCount = 0;

	handler->State = SPI_STATE_BUSY_RX;
	handler->ErrorCode = 0;
	handler->Instance->SPCR_REG |= (_BV(SPIE) | _BV(SPE));
	handler->Instance->SPDR_REG = 0; // Fire up transmission

	error:
	__HAL_UNLOCK(handler);
	return errCode;
}

hal_status_t SPI_Abort(spi_handler_t *handler){
	if(handler->State == SPI_STATE_BUSY_RX || handler->State == SPI_STATE_BUSY_TX || handler->State == SPI_STATE_BUSY_TX_RX){
		handler->State = SPI_STATE_ABORT;
		uint8_t timeout = 10;
		do {
			_delay_us(100);
			if(--timeout == 0)
				break;
		}while(handler->State != SPI_STATE_READY);
		handler->State = SPI_STATE_READY;
		return HAL_OK;
	}
	return HAL_ERROR;
}

void SPI_RegisterCallback(spi_handler_t *handler, spi_callback_id_t CallbackID, SPI_Callback_t Callback){
	switch (CallbackID) {
		case SPI_TX_COMPLETE_CB_ID:
			handler->TxCpltCallback = Callback;
		break;

		case SPI_RX_COMPLETE_CB_ID:
			handler->RxCpltCallback = Callback;
		break;

		case SPI_TXRX_COMPLETE_CB_ID:
			handler->TxRxCpltCallback = Callback;
		break;
		
		default:
			break;
	}
}

void SPI_UnRegisterCallback(spi_handler_t *handler, spi_callback_id_t CallbackID){
	switch (CallbackID) {
		case SPI_TX_COMPLETE_CB_ID:
			handler->TxCpltCallback = NULL;
		break;

		case SPI_RX_COMPLETE_CB_ID:
			handler->RxCpltCallback = NULL;
		break;

		case SPI_TXRX_COMPLETE_CB_ID:
			handler->TxRxCpltCallback = NULL;
		break;
		
		default:
			break;
	}
}