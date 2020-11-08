#ifndef _TWI_DRIVER_H_
#define _TWI_DRIVER_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "hal_def.h"


#define TWI_GENERALCALL_DISABLE	0
#define TWI_GENERALCALL_ENABLE	_BV(TWGCE)

typedef enum {
	TWI_STATE_RESET,
	TWI_STATE_READY,
	TWI_STATE_MRX,
	TWI_STATE_MTX,
	TWI_STATE_SRX,
	TWI_STATE_STX
}twi_state_t;

typedef enum {
	TWI_MASTER_TX_COMPLETE_CB_ID,
	TWI_MASTER_RX_COMPLETE_CB_ID,
	TWI_SLAVE_TX_COMPLETE_CB_ID,
	TWI_SLAVE_RX_COMPLETE_CB_ID,
	TWI_ERROR_CB_ID
}twi_callback_id_t;

typedef enum {
	TWI_MODE_MASTER,
	TWI_MODE_SLAVE
}twi_mode_t;

typedef enum {
	TWI_CLOCK_SLOW,
	TWI_CLOCK_NORMAL,
	TWI_CLOCK_FAST
}twi_clock_t;

typedef enum {
	TWI_ERR_NONE,
	TWI_ERR_SLA_NACK,
	TWI_ERR_DATA_NACK,
	TWI_ERR_ARB_LOST,
	TWI_ERR_BUS,
}twi_error_t;

typedef struct {
	twi_mode_t Mode;
	twi_clock_t Clock;
	uint8_t OwnAddress;
	uint8_t OwnAddressMask;
	uint8_t GeneralCallMode;
}twi_init_t;

typedef struct _twi_handler {
	twi_init_t Init;
	twi_t *Instance;
	hal_lock_t Lock;

	volatile uint8_t ErrCode;
	volatile twi_state_t State;
	
	uint8_t Slarw;
	volatile uint8_t SendStop;
	volatile uint8_t InRepStart;

	uint8_t *TxRxBuffPtr;
	volatile uint8_t TxRxBuffIndex;
	uint8_t TxRxBuffSize;

	void (*MasterTxCpltCallback)(struct _twi_handler *handler);
	void (*MasterRxCpltCallback)(struct _twi_handler *handler);
	void (*SlaveTxCpltCallback)(struct _twi_handler *handler);
	void (*SlaveRxCpltCallback)(struct _twi_handler *handler);
	void (*AddrCallback)(struct _twi_handler *handler, uint8_t TransferDirection);
	void (*ErrorCallback)(struct _twi_handler *handler);
}twi_handler_t;


typedef void (*TWI_Callback_t)(twi_handler_t *handler);
typedef void (*TWI_AddrCallback_t)(twi_handler_t *handler, uint8_t TransferDirection);

void TWI_IRQHandler(twi_handler_t *handler);

hal_status_t TWI_Init(twi_handler_t *handler);
hal_status_t TWI_DeInit(twi_handler_t *handler);

twi_error_t TWI_GetError(twi_handler_t *handler);
twi_state_t TWI_GetState(twi_handler_t *handler);

hal_status_t TWI_MasterTransmit(twi_handler_t *handler, uint8_t DeviceAddr, uint8_t *pData, uint8_t Size, uint8_t SendStop);
hal_status_t TWI_MasterReceive(twi_handler_t *handler, uint8_t DeviceAddr, uint8_t *pData, uint8_t Size, uint8_t SendStop);

hal_status_t TWI_SlaveTransmit(twi_handler_t *handler, uint8_t *pData, uint8_t Size);
hal_status_t TWI_SlaveReceive(twi_handler_t *handler, uint8_t *pData, uint8_t Size);

void TWI_RegisterCallback(twi_handler_t *handler, twi_callback_id_t CallbackID, TWI_Callback_t Callback);
void TWI_UnRegisterCallback(twi_handler_t *handler, twi_callback_id_t CallbackID);

void TWI_RegisterAddrCallback(twi_handler_t *handler, TWI_AddrCallback_t AddrCallback);
void TWI_UnRegisterAddrCallback(twi_handler_t *handler);

#ifdef __cplusplus
}
#endif

#endif /* _TWI_DRIVER_H_ */