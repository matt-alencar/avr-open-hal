#ifndef _USART_DRIVER_H_
#define _USART_DRIVER_H_


#ifdef __cplusplus
extern "C" {
#endif

#include "hal_def.h"

#define USART_CLOCK_DIV2        (F_CPU/2)
#define USART_CLOCK_DIV4        (F_CPU/4)
#define USART_CLOCK_DIV8        (F_CPU/8)
#define USART_CLOCK_DIV16       (F_CPU/16)
#define USART_CLOCK_DIV32       (F_CPU/32)
#define USART_CLOCK_DIV64       (F_CPU/64)
#define USART_CLOCK_DIV128      (F_CPU/128)
#define USART_CLOCK_DIV256      (F_CPU/256)
#define USART_CLOCK_DIV512      (F_CPU/512)

#define USART_PHASE_1EDGE		0
#define USART_PHASE_2EDGE		1

#define USART_POLARITY_HIGH		0
#define USART_POLARITY_LOW		1

#define USART_MODE_ASYNC		0
#define USART_MODE_SYNC_MASTER	1
#define USART_MODE_SYNC_SLAVE	2
#define USART_MODE_SPI_MASTER	3

#define USART_WORDLENGTH_5B		0
#define USART_WORDLENGTH_6B		1
#define USART_WORDLENGTH_7B		2
#define USART_WORDLENGTH_8B		3

#define USART_STOPBITS_1B		0
#define USART_STOPBITS_2B		1

#define USART_PARITY_NONE		0
#define USART_PARITY_EVEN		2
#define USART_PARITY_ODD		3

#define USART_OVERSAMPLING_16	16
#define USART_OVERSAMPLING_8	8

#define USART_MSB_FIRST			0
#define USART_LSB_FIRST			1

#define USART_ERR_NONE				0
#define USART_ERR_NULL_RX_BUFFER	1
#define USART_ERR_PARITY			_BV(UPE0)
#define USART_ERR_OVERFLOW			_BV(DOR0)
#define USART_ERR_FRAME				_BV(FE0)


typedef enum {
	USART_STATE_RESET,
	USART_STATE_READY,
	USART_STATE_BUSY,
	USART_STATE_BUSY_TX,
	USART_STATE_ERROR,
}usart_state_t;


typedef enum {
	USART_ISR_TX_DONE,
	USART_ISR_RX_OVF,
	USART_ISR_RX_BYTE,
	USART_ISR_RX_ERROR
}usart_isr_t;


typedef struct {
	uint8_t Mode;
	uint8_t WordLenght;
	uint8_t StopBits;
	uint8_t OverSampling;
	uint8_t Parity;
	uint32_t BaudRate;
	uint8_t CLKPhase;
	uint8_t CLKPolarity;
	uint8_t BitOrder;
}usart_init_t;


typedef struct _usart_handler {
	usart_t *Instance;
	usart_init_t Init;
	volatile usart_state_t State;
	volatile uint8_t ErrCode;
	hal_lock_t Lock;

	/* TX Buffer */
	uint8_t *TxBuffPtr;
	uint16_t TxBuffSize;
	volatile uint16_t TxIndex;

	/* RX Circular Buffer */
	uint8_t *RxBufferPtr;
	uint16_t RxBuffSize;
	volatile uint16_t RxWriteIndex;
	uint16_t RxReadIndex;

	/* Driver Callbacks */
	void (*TxCpltCallback)(struct _usart_handler *handler);
	void (*RxBuffOvfCallback)(struct _usart_handler *handler);
	void (*RxByteCallback)(struct _usart_handler *handler);
	void (*RxErrorCallback)(struct _usart_handler *handler);
}usart_handler_t;

typedef void (*usart_callback_t)(usart_handler_t *handler);

void USART_IRQTxHandler(usart_handler_t *handler);
void USART_IRQRxHandler(usart_handler_t *handler);

hal_status_t USART_Init(usart_handler_t *handler);
void USART_DeInit(usart_handler_t *handler);

void USART_SetRxBuff(usart_handler_t *handler, uint8_t *pBuff, uint16_t Size);

void USART_ResetRxBuffer(usart_handler_t *handler);
hal_status_t USART_Transmit(usart_handler_t *handler, uint8_t *pData, uint16_t Size);
hal_status_t USART_Receive(usart_handler_t *handler, uint8_t *pData, uint16_t Size);
hal_status_t USART_ReceiveByte(usart_handler_t *handler, uint8_t *pData);
hal_status_t USART_Peek(usart_handler_t *handler, uint8_t *pData);

uint8_t USART_GetRxBytes(usart_handler_t *handler);

usart_state_t USART_GetState(usart_handler_t *handler);
uint8_t USART_GetError(usart_handler_t *handler);

hal_status_t USART_RegisterCallback(usart_handler_t *handler, usart_isr_t isr_type, usart_callback_t callback);
hal_status_t USART_UnRegisterCallback(usart_handler_t *handler, usart_isr_t isr_type);

#ifdef __cplusplus
}
#endif

#endif /* _USART_DRIVER_H_ */