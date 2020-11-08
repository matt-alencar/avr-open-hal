#include "./main.h"


void Spi_TestMasterFSM(){
	
	static uint8_t buffer[] = {0x00, 0x01, 0x02};
	static uint8_t state = 0;
	uint8_t recv[3];
	uint8_t i = 0;

	switch(state){
        /* Start TXN */
		case 0:
			if(Spi_State(SPI_ID_0) == SPI_STATE_IDLE){
				Dio_SetPin(DIO_ID_D, DIO_PIN_4);
				Spi_WriteWord(SPI_ID_0, 0x4000);
				Spi_WriteDWord(SPI_ID_0, 0xFFF7FFFF);
				Spi_WriteBlock(SPI_ID_0, buffer, sizeof(buffer));
				Spi_Flush(SPI_ID_0);
				state = 1;
			}
		break;
		
        /* Wait TXN complete */
		case 1:
			if (Spi_State(SPI_ID_0) == SPI_STATE_IDLE)
				state = 2;
		break;

        /* Compare received data from buffer (to work connect MOSI to MISO) */
		case 2:
			if(Spi_ReadWord(SPI_ID_0) == 0x4000){
				if(Spi_ReadDWord(SPI_ID_0) == 0xFFF7FFFF){
					while(Spi_Available(SPI_ID_0) && i < 3){
						recv[i++] = Spi_ReadByte(SPI_ID_0);
					}
					if(memcmp(buffer, recv, 3) == 0){
						state = 4;
						break;
					}
				}
			}
			state = 3;
		break;

        /* Compare error, test fail */
		case 3:
		
		break;

        /* Toggle D3 forever (Master test passed) */
		case 4:
			Dio_ResetPin(DIO_ID_D, DIO_PIN_4);
		break;
	}
}

int main(){
    Dio_WritePinMode(DIO_ID_D, DIO_PIN_4, DIO_MODE_OUTPUT);
    Spi_Init(SPI_ID_0, SPI_MODE_MASTER, SPI_CLOCK_DIV128, SPI_PL_MODE0, SPI_LSB_FIRST);

    sei();

	while(1){
		Spi_TestMasterFSM();
	}
}