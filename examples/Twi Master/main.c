#include "./main.h"

#define DATA_SIZE 14

int main(){
	twi_config_t TWI_CFG;
	TWI_CFG.Clock = TWI_CLOCK_NORMAL;
	TWI_CFG.Mode = TWI_MODE_MASTER;

	if(Twi_Init(TWI_ID_0, &TWI_CFG) != R_SUCCESS){
		while(1);
	}

	Dio_WritePinMode(DIO_ID_D, DIO_PIN_4, DIO_MODE_OUTPUT);
    sei();
	
	const uint8_t SLADDR = 0x50;
	uint16_t memAddr = 0x00;
	uint8_t memData[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13};
	uint8_t state = 0;
	while(1){
		switch(state){
			case 0:
				Twi_BeginTxn(TWI_ID_0, SLADDR);
				// Twi_WriteBuffer(TWI_ID_0, (const void*)&memAddr, 2);
				Twi_WriteWord(TWI_ID_0, memAddr);
				Twi_WriteBlock(TWI_ID_0, &memData, DATA_SIZE);
				Twi_Flush(TWI_ID_0, 1);
				Dio_SetPin(DIO_ID_D, DIO_PIN_4);
				while(Twi_State(TWI_ID_0) != TWI_STATE_IDLE);
				state = 1;
			break;
			
			case 1:
				memset(Twi_MasterBuffer(TWI_ID_0), 0, TWI_BUFFER_LENGTH);
				_delay_ms(10);
				Twi_BeginTxn(TWI_ID_0, SLADDR);
				Twi_WriteWord(TWI_ID_0, memAddr);
				Twi_Flush(TWI_ID_0, 0);
				while(Twi_State(TWI_ID_0) != TWI_STATE_IDLE);
				state = 2;
			break;

			case 2:
				Twi_ReadTxn(TWI_ID_0, SLADDR, DATA_SIZE, 1);
				while(Twi_State(TWI_ID_0) != TWI_STATE_IDLE);

				uint8_t *buff = Twi_MasterBuffer(TWI_ID_0);
				if(memcmp(memData, buff, Twi_Available(TWI_ID_0)) != 0){
					Dio_ResetPin(DIO_ID_D, DIO_PIN_4);
				}
				state = 4;
			break;

			case 4:
			break;
		}

	}
}