#include <util/twi.h>
#include "hal_twi.h"
#include "hal_gpio.h"


/* PRIVATE FUNCTIONS */
static void TWI_Reply(twi_handler_t *handler, uint8_t ack);
static void TWI_Stop(twi_handler_t *handler);
static void TWI_ReleaseBus(twi_handler_t *handler);
static void TWI_StartTxn(twi_handler_t *handler);
static uint8_t TWI_GetTWBR(twi_clock_t clock);
static void TWI_SetupGPIO(twi_handler_t *handler);
/* END OF PRIVATE FUNCTIONS */


void TWI_IRQHandler(twi_handler_t *handler){
	uint8_t twi_status = handler->Instance->TWSR_REG & TW_STATUS_MASK;
	switch(twi_status){
		/* All Master */
		case TW_START:					// sent start condition
		case TW_REP_START:				// sent repeated start condition
			// copy device address and r/w bit to output register and ack
			handler->Instance->TWDR_REG = handler->Slarw;
			TWI_Reply(handler, 1);
			break;

		/* Master Transmitter */
		case TW_MT_SLA_ACK:				// slave receiver acked address
		case TW_MT_DATA_ACK:			// slave receiver acked data
			// if there is data to send, send it, otherwise stop
			if(handler->TxRxBuffIndex < handler->TxRxBuffSize){
				// copy data to output register and ack
				TWI0->TWDR_REG = handler->TxRxBuffPtr[handler->TxRxBuffIndex++];
				TWI_Reply(handler, 1);
			}
			else{
				if(handler->SendStop)
					TWI_Stop(handler);
				else {
					handler->Instance->TWCR_REG = (_BV(TWINT) | _BV(TWSTA)| _BV(TWEN));
					handler->InRepStart = 1;
					handler->State = TWI_STATE_READY;
				}
				if(handler->MasterTxCpltCallback != NULL)
					handler->MasterTxCpltCallback(handler);
			}
			break;
		case TW_MT_SLA_NACK:			// address sent, nack received
			handler->ErrCode = TW_MT_SLA_NACK;
			TWI_Stop(handler);

			if(handler->ErrorCallback != NULL){
				handler->ErrorCallback(handler);
			}
			
			break;
		case TW_MT_DATA_NACK:			// data sent, nack received
			handler->ErrCode = TW_MT_DATA_NACK;
			TWI_Stop(handler);

			if(handler->ErrorCallback != NULL){
				handler->ErrorCallback(handler);
			}

			break;
		case TW_MT_ARB_LOST:			// lost bus arbitration
			handler->ErrCode = TW_MT_ARB_LOST;
			TWI_ReleaseBus(handler);
			
			if(handler->ErrorCallback != NULL){
				handler->ErrorCallback(handler);
			}

			break;

		/* Master Receiver */
		case TW_MR_DATA_ACK:			// data received, ack sent
			// put byte into buffer
			handler->TxRxBuffPtr[handler->TxRxBuffIndex++] = TWI0->TWDR_REG;
		case TW_MR_SLA_ACK:				// address sent, ack received
			// ack if more bytes are expected, otherwise nack
			if(handler->TxRxBuffIndex < handler->TxRxBuffSize){
				TWI_Reply(handler, 1);
			}else{
				TWI_Reply(handler, 0);
			}
			break;
		case TW_MR_DATA_NACK:			// data received, nack sent
			// put final byte into buffer
			handler->TxRxBuffPtr[handler->TxRxBuffIndex++] = TWI0->TWDR_REG;
			
			handler->TxRxBuffSize = handler->TxRxBuffIndex;
			handler->TxRxBuffIndex = 0;

			if(handler->SendStop)
				TWI_Stop(handler);
			else {
				handler->Instance->TWCR_REG = (_BV(TWINT) | _BV(TWSTA)| _BV(TWEN));
				handler->InRepStart = 1;
				handler->State = TWI_STATE_READY;
			}
			break;
		case TW_MR_SLA_NACK:			// address sent, nack received
			handler->ErrCode = TW_MR_SLA_NACK;
			TWI_Stop(handler);
			if(handler->ErrorCallback != NULL){
				handler->ErrorCallback(handler);
			}
			break;
		// TW_MR_ARB_LOST handled by TW_MT_ARB_LOST case

		/* Slave Receiver */
		case TW_SR_SLA_ACK:				// addressed, returned ack
		case TW_SR_GCALL_ACK:			// addressed generally, returned ack
		case TW_SR_ARB_LOST_SLA_ACK:	// lost arbitration, returned ack
		case TW_SR_ARB_LOST_GCALL_ACK:	// lost arbitration, returned ack
			// enter slave receiver mode
			handler->State = TWI_STATE_SRX;
			handler->TxRxBuffIndex = 0;
			handler->TxRxBuffSize = 0;
			handler->TxRxBuffPtr = NULL;

			if(handler->AddrCallback != NULL){
				handler->AddrCallback(handler, TW_WRITE);
				TWI_Reply(handler, 1);	// REPLY ACK -> HARDWARE IS READY FOR RECEPTION
			}
			else {
				TWI_Reply(handler, 0);	// REPLY NACK -> HARDWARE IS NOT READY FOR RECEPTION
			}
			
			break;
		
		case TW_SR_DATA_ACK:			// data received, returned ack
		case TW_SR_GCALL_DATA_ACK:		// data received generally, returned ack

			if(handler->TxRxBuffIndex < handler->TxRxBuffSize){
				handler->TxRxBuffPtr[handler->TxRxBuffIndex++] = handler->Instance->TWDR_REG;
			}

			if(handler->TxRxBuffIndex < handler->TxRxBuffSize){
				TWI_Reply(handler, 1);	// Send ACK for future incoming byte
			}
			else {
				TWI_Reply(handler, 0);	// Otherwise send NACK for future incoming byte
			}
			
			break;

		case TW_SR_STOP:				// stop or repeated start condition received
		case TW_SR_DATA_NACK:			// data received, returned nack
		case TW_SR_GCALL_DATA_NACK:		// data received generally, returned nack
			// The master must stop communication if any of these condition happen
			// ack future responses and leave slave receiver state
			TWI_ReleaseBus(handler);
			if(handler->SlaveRxCpltCallback != NULL){
				handler->SlaveRxCpltCallback(handler);
			}

			break;

		/* Slave Transmitter */
		case TW_ST_SLA_ACK:				// addressed, returned ack
			handler->State = TWI_STATE_STX;
			handler->TxRxBuffIndex = 0;
			handler->TxRxBuffSize = 0;
			handler->TxRxBuffPtr = NULL;

			if(handler->AddrCallback != NULL){
				handler->AddrCallback(handler, TW_READ);
			}

		case TW_ST_ARB_LOST_SLA_ACK:	// arbitration lost, returned ack
		case TW_ST_DATA_ACK:			// byte sent, ack returned
			// enter slave transmitter mode

			if(handler->TxRxBuffIndex < handler->TxRxBuffSize)
				TWI0->TWDR_REG = handler->TxRxBuffPtr[handler->TxRxBuffIndex++];
			else
				TWI0->TWDR_REG = 0xFF; // Send dummy byte

			// Send response for future transactions
			if(handler->TxRxBuffIndex < handler->TxRxBuffSize)
				TWI_Reply(handler, 1);	// Send ACK for future incoming byte
			else
				TWI_Reply(handler, 0);	// Otherwise send NACK for future incoming byte

			break;
		
		case TW_ST_DATA_NACK:			// received nack, we are done 
		case TW_ST_LAST_DATA:			// received ack, but we are done already!
			// The master must stop communication if any of these condition happen
			// ack future responses and leave slave receiver state
			TWI_ReleaseBus(handler);

			if(handler->SlaveTxCpltCallback != NULL){
				handler->SlaveTxCpltCallback(handler);
			}

			break;

		/* All */
		case TW_NO_INFO:   // no state information
			break;
		case TW_BUS_ERROR: // bus error, illegal stop/start
			handler->ErrCode = TW_BUS_ERROR;
			TWI_Stop(handler);

			if(handler->ErrorCallback != NULL){
				handler->ErrorCallback(handler);
			}

			break;
	}
}


static void TWI_SetupGPIO(twi_handler_t *handler){
	if(handler->Instance == TWI0){
		GPIO_PinMode(TWI0_SCL_GPIO, TWI0_SCL_PIN, GPIO_MODE_INPUT);
		GPIO_PinMode(TWI0_SDA_GPIO, TWI0_SDA_PIN, GPIO_MODE_INPUT);
	}
}

static uint8_t TWI_GetTWBR(twi_clock_t clock){
	const uint32_t TWI_FREQ[] = {50000UL, 100000UL, 400000UL};
	uint8_t value = ((F_CPU / TWI_FREQ[clock]) - 16) / 2;
	return value;
}

static void TWI_Reply(twi_handler_t *handler, uint8_t ack){
	// transmit master read ready signal, with or without ack
	if(ack){
		handler->Instance->TWCR_REG = _BV(TWEN) | _BV(TWIE) | _BV(TWINT) | _BV(TWEA);
	}else{
		handler->Instance->TWCR_REG = _BV(TWEN) | _BV(TWIE) | _BV(TWINT);
	}
}

static void TWI_Stop(twi_handler_t *handler){
	// send stop condition
	handler->Instance->TWCR_REG = _BV(TWEN) | _BV(TWIE) | _BV(TWEA) | _BV(TWINT) | _BV(TWSTO);

	// wait for stop condition to be exectued on bus
	// TWINT is not set after a stop condition!
	while(handler->Instance->TWCR_REG & _BV(TWSTO)){
		continue;
	}

	handler->State = TWI_STATE_READY;
}

static void TWI_ReleaseBus(twi_handler_t *handler){
	// release bus
	handler->Instance->TWCR_REG = _BV(TWEN) | _BV(TWIE) | _BV(TWEA) | _BV(TWINT);
	// update twi state
	handler->State = TWI_STATE_READY;
}

static void TWI_StartTxn(twi_handler_t *handler){
	if(handler->InRepStart) {
		handler->InRepStart = 0;
		do{
			handler->Instance->TWDR_REG = handler->Slarw;
		}while(handler->Instance->TWCR_REG & _BV(TWWC));
		// enable INTs, but not START
		handler->Instance->TWCR_REG = _BV(TWINT) | _BV(TWEA) | _BV(TWEN) | _BV(TWIE);
	}
	else
		handler->Instance->TWCR_REG = (_BV(TWINT) | _BV(TWEA) | _BV(TWEN) | _BV(TWIE) | _BV(TWSTA));
}

hal_status_t TWI_Init(twi_handler_t *handler){
	if(handler == NULL){
		return HAL_ERROR;
	}

	hal_status_t retCode = HAL_OK;
	__HAL_LOCK(handler);

	if(handler->State == TWI_STATE_RESET || handler->State == TWI_STATE_READY){
		handler->State = TWI_STATE_READY;
		handler->ErrCode = TW_NO_INFO;
		handler->SendStop = 1;
		if(handler->Init.Mode == TWI_MODE_SLAVE){
			handler->Instance->TWAR_REG = (handler->Init.OwnAddress << 1) | handler->Init.GeneralCallMode;
			handler->Instance->TWAMR_REG = (handler->Init.OwnAddressMask << 1);
		}
		else {
			handler->Instance->TWAR_REG = 0xFE;
			handler->Instance->TWAMR_REG = 0x00;
		}

		TWI_SetupGPIO(handler);

		handler->Instance->TWSR_REG &= ~(_BV(TWPS0) | _BV(TWPS1)); // Set clock prescaller to 1
		handler->Instance->TWBR_REG = TWI_GetTWBR(handler->Init.Clock);
		// enable twi module, acks, and twi interrupt
		handler->Instance->TWCR_REG = _BV(TWEN) | _BV(TWIE) | _BV(TWEA);
	}
	else{
		retCode = HAL_BUSY;
	}

	__HAL_UNLOCK(handler);
	return retCode;
}

hal_status_t TWI_DeInit(twi_handler_t *handler){
	__HAL_LOCK(handler);
	// block until bus is idle
	while(handler->State != TWI_STATE_READY){
		continue;
	}

	handler->Instance->TWCR_REG = 0;
	handler->ErrCode = TW_NO_INFO;
	handler->State = TWI_STATE_READY;
	handler->Instance->TWAR_REG = 0xFE;
	handler->Instance->TWAMR_REG = 0x00;
	
	__HAL_UNLOCK(handler);
	return HAL_OK;
}

twi_error_t TWI_GetError(twi_handler_t *handler){
	switch (handler->ErrCode){
		case TW_NO_INFO:
			return TWI_ERR_NONE;

		case TW_MT_ARB_LOST:
			return TWI_ERR_ARB_LOST;

		case TW_MT_DATA_NACK:
			return TWI_ERR_DATA_NACK;

		case TW_MT_SLA_NACK:
		case TW_MR_SLA_NACK:
			return TWI_ERR_SLA_NACK;

		case TW_BUS_ERROR:
			return TWI_ERR_BUS;

		default:
			return TWI_ERR_NONE;
	}
}

twi_state_t TWI_GetState(twi_handler_t *handler){
	return handler->State;
}

hal_status_t TWI_MasterTransmit(twi_handler_t *handler, uint8_t DeviceAddr, uint8_t *pData, uint8_t Size, uint8_t SendStop){
	if(pData == NULL || Size == 0){
		return HAL_ERROR;
	}
	
	hal_status_t retCode = HAL_OK;
	__HAL_LOCK(handler);
	
	if(handler->State == TWI_STATE_READY){
		handler->State = TWI_STATE_MTX;
		handler->ErrCode = TW_NO_INFO;
		handler->SendStop = SendStop;
		handler->Slarw = ((DeviceAddr << 1) | TW_WRITE);
		handler->TxRxBuffIndex = 0;
		handler->TxRxBuffPtr = pData;
		handler->TxRxBuffSize = Size;
		TWI_StartTxn(handler);
	}
	else {
		retCode = HAL_BUSY;
	}

	__HAL_UNLOCK(handler);
	return retCode;
}

hal_status_t TWI_MasterReceive(twi_handler_t *handler, uint8_t DeviceAddr, uint8_t *pData, uint8_t Size, uint8_t SendStop){
	if(pData == NULL || Size == 0){
		return HAL_ERROR;
	}
	
	hal_status_t retCode = HAL_OK;
	__HAL_LOCK(handler);
	
	if(handler->State == TWI_STATE_READY){
		handler->State = TWI_STATE_MRX;
		handler->ErrCode = TW_NO_INFO;
		handler->SendStop = SendStop;
		handler->Slarw = ((DeviceAddr << 1) | TW_READ);
		handler->TxRxBuffPtr = pData;
		handler->TxRxBuffIndex = 0;
		handler->TxRxBuffSize = Size-1; // This is not intuitive, read on...
		// On receive, the previously configured ACK/NACK setting is transmitted in
		// response to the received byte before the interrupt is signalled. 
		// Therefor we must actually set NACK when the _next_ to last byte is
		// received, causing that NACK to be sent in response to receiving the last
		// expected byte of data.
		TWI_StartTxn(handler);
	}
	else {
		retCode = HAL_BUSY;
	}

	__HAL_UNLOCK(handler);
	return retCode;
}

hal_status_t TWI_SlaveTransmit(twi_handler_t *handler, uint8_t *pData, uint8_t Size){
	__HAL_LOCK(handler);
	hal_status_t retCode = HAL_OK;
	
	if(handler->State == TWI_STATE_STX){
		handler->TxRxBuffPtr = pData;
		handler->TxRxBuffIndex = 0;
		handler->TxRxBuffSize = Size;
	}
	else{
		/* Wrong usage of SlaveTransmit function */
		retCode = HAL_ERROR;
	}

	__HAL_UNLOCK(handler);
	return retCode;
}

hal_status_t TWI_SlaveReceive(twi_handler_t *handler, uint8_t *pData, uint8_t Size){
	__HAL_LOCK(handler);
	hal_status_t retCode = HAL_OK;
	
	if(handler->State == TWI_STATE_SRX){
		handler->TxRxBuffPtr = pData;
		handler->TxRxBuffIndex = 0;
		handler->TxRxBuffSize = Size;
	}
	else{
		/* Wrong usage of TWI_SlaveReceive function */
		retCode = HAL_ERROR;
	}

	__HAL_UNLOCK(handler);
	return retCode;
}

void TWI_RegisterCallback(twi_handler_t *handler, twi_callback_id_t CallbackID, TWI_Callback_t Callback){
	switch (CallbackID) {
		case TWI_MASTER_TX_COMPLETE_CB_ID:
			handler->MasterTxCpltCallback = Callback;
			break;

		case TWI_MASTER_RX_COMPLETE_CB_ID:
			handler->MasterRxCpltCallback = Callback;
			break;
		
		case TWI_SLAVE_TX_COMPLETE_CB_ID:
			handler->SlaveTxCpltCallback = Callback;
			break;
		

		case TWI_SLAVE_RX_COMPLETE_CB_ID:
			handler->SlaveRxCpltCallback = Callback;
			break;

		case TWI_ERROR_CB_ID:
			handler->ErrorCallback = Callback;
			break;

		default:
			break;
	}
}

void TWI_UnRegisterCallback(twi_handler_t *handler, twi_callback_id_t CallbackID){
	switch (CallbackID) {
		case TWI_MASTER_TX_COMPLETE_CB_ID:
			handler->MasterTxCpltCallback = NULL;
			break;

		case TWI_MASTER_RX_COMPLETE_CB_ID:
			handler->MasterRxCpltCallback = NULL;
			break;
		
		case TWI_SLAVE_TX_COMPLETE_CB_ID:
			handler->SlaveTxCpltCallback = NULL;
			break;
		

		case TWI_SLAVE_RX_COMPLETE_CB_ID:
			handler->SlaveRxCpltCallback = NULL;
			break;

		case TWI_ERROR_CB_ID:
			handler->ErrorCallback = NULL;
			break;

		default:
			break;
	}
}

void TWI_RegisterAddrCallback(twi_handler_t *handler, TWI_AddrCallback_t AddrCallback){
	handler->AddrCallback = AddrCallback;
}

void TWI_UnRegisterAddrCallback(twi_handler_t *handler){
	handler->AddrCallback = NULL;
}
