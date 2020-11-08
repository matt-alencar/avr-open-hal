#ifndef _SPI_DRIVER_H_
#define _SPI_DRIVER_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "hal_def.h"
#include "hal_gpio.h"
#include <util/delay.h>


/**
 * @brief SPI Clock Polarity
 * 
 */
typedef enum {
    SPI_POLARITY_LOW,
    SPI_POLARITY_HIGH 
}spi_polarity_t;


/**
 * @brief SPI Clock Phase
 * 
 */
typedef enum {
    SPI_PHASE_1EDGE,
    SPI_PHASE_2EDGE
}spi_phase_t;


/**
 * @brief SPI Clock Divisor
 * 
 */
typedef enum{
    SPI_CLOCK_DIV4,
    SPI_CLOCK_DIV16,
    SPI_CLOCK_DIV64,
    SPI_CLOCK_DIV128,
    SPI_CLOCK_DIV2,
    SPI_CLOCK_DIV8,
    SPI_CLOCK_DIV32,
}spi_clock_t;


/**
 * @brief SPI Operation Mode
 * 
 */
typedef enum {
    SPI_MODE_SLAVE,
    SPI_MODE_MASTER,
}spi_mode_t;


/**
 * @brief SPI State
 * 
 */
typedef enum {
    SPI_STATE_RESET,
    SPI_STATE_READY,
    SPI_STATE_BUSY,
    SPI_STATE_BUSY_TX,
    SPI_STATE_BUSY_RX,
    SPI_STATE_BUSY_TX_RX,
    SPI_STATE_ERROR,
    SPI_STATE_ABORT,
}spi_state_t;

/**
 * @brief SPI Bit Transfer Order
 * 
 */
typedef enum {
    SPI_MSB_FIRST,
    SPI_LSB_FIRST,
}spi_bit_order_t;


/**
 * @brief SPI Callback IDs
 * 
 */
typedef enum {
    SPI_TX_COMPLETE_CB_ID,
    SPI_RX_COMPLETE_CB_ID,
    SPI_TXRX_COMPLETE_CB_ID
}spi_callback_id_t;


/**
 * @brief SPI Init Struct
 * 
 */
typedef struct {
    spi_mode_t Mode;
    spi_bit_order_t BitOrder;
    spi_polarity_t CLKPolarity;
    spi_phase_t CLKPhase;
    spi_clock_t ClockPresc;
}spi_init_t;


/**
 * @brief SPI Handler Struct
 * 
 */
typedef struct _spi_handler {
    spi_t *Instance;
	spi_init_t Init;

    volatile uint8_t *TxBuffPtr;
    uint8_t TxBuffSize;
    volatile uint8_t TxCount;

    volatile uint8_t *RxBuffPtr;
    uint8_t RxBuffSize;
    volatile uint8_t RxCount;

    void (*TxCpltCallback)(struct _spi_handler *handler);
    void (*RxCpltCallback)(struct _spi_handler *handler);
    void (*TxRxCpltCallback)(struct _spi_handler *handler);

    hal_lock_t Lock;
    volatile spi_state_t State;
    volatile uint8_t ErrorCode;
}spi_handler_t;


typedef void (*SPI_Callback_t)(spi_handler_t *handler);

void SPI_IRQHandler(spi_handler_t *handler);

hal_status_t SPI_Init(spi_handler_t *handler);
void SPI_DeInit(spi_handler_t *handler);

spi_state_t SPI_GetState(spi_handler_t *handler);
uint8_t SPI_GetError(spi_handler_t *handler);

hal_status_t SPI_TransmitReceive(spi_handler_t *handler, uint8_t *pTxData, uint8_t *pRxData, uint8_t Size);
hal_status_t SPI_Receive(spi_handler_t *handler, uint8_t *pData, uint8_t Size);
hal_status_t SPI_Transmit(spi_handler_t *handler, uint8_t *pData, uint8_t Size);
hal_status_t SPI_Abort(spi_handler_t *handler);

void SPI_RegisterCallback(spi_handler_t *handler, spi_callback_id_t CallbackID, SPI_Callback_t Callback);
void SPI_UnRegisterCallback(spi_handler_t *handler, spi_callback_id_t CallbackID);

#ifdef __cplusplus
}
#endif

#endif /* _SPI_DRIVER_H_ */