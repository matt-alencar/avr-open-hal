#include "./main.h"

#define DATA_SIZE 14

typedef struct {
	uint32_t Counter;
	uint8_t Data[DATA_SIZE];
}twi_txn_t;

static volatile twi_txn_t twi_buffer;

/* 
 * Function called when master stop sending data to slave
 */
void onSlaveRX(uint8_t index, uint8_t size){
	if(index == 0 && size == 4){
		Dio_TogglePin(DIO_ID_D, DIO_PIN_4);
	}
}


int main(){

	twi_config_t TWI_CFG;
	TWI_CFG.Clock = TWI_CLOCK_NORMAL;
	TWI_CFG.Mode = TWI_MODE_SLAVE;
	TWI_CFG.SlaveAddr = 0x50;
	TWI_CFG.SlaveGeneralCall = 0;
	TWI_CFG.SlaveBuffer = (uint8_t*)&twi_buffer;
	TWI_CFG.SlaveBufferSize = sizeof(twi_txn_t);

	if(Twi_Init(TWI_ID_0, &TWI_CFG) != R_SUCCESS){
		while(1);
	}
	
	Twi_RegisterSlaveRxISR(TWI_ID_0, onSlaveRX);
	Dio_WritePinMode(DIO_ID_D, DIO_PIN_4, DIO_MODE_OUTPUT);
    
    sei();

	while(1){
		twi_buffer.Counter++;
		_delay_ms(1);
		// Dio_TogglePin(DIO_ID_D, DIO_PIN_4);
	}
}