#include "main.h"

#define SPI_REG_SIZE 8
volatile uint8_t spi_addr = 0;
volatile uint8_t spi_data[SPI_REG_SIZE] = {7, 6, 5, 4, 3, 2, 1, 0};
volatile uint8_t spi_timeout = 0;


void Spi_TimeOut(){
	spi_timeout = 1;
	Spi_WriteSlave(SPI_ID_0, 0x00); // Prepare zeros to be sended on next transaction
}


void Spi_SlaveHandler(uint8_t data){
	static uint8_t state = 0;

	if(spi_timeout){
		spi_timeout = 0;
		state = 0;
	}
	else {
		Wdt_Reset();
	}

	switch(state){

		/* Initial state: Receive the register address */
		case 0:
			spi_addr = data;
			uint8_t mode = spi_addr & 0x80;
			spi_addr &= 0x7F;
			if(spi_addr < SPI_REG_SIZE){
				/* Write register mode */
				if(mode){
					Spi_WriteSlave(SPI_ID_0, 0x00);
					state = 1;
				}
				/* Read register mode */
				else {
					Spi_WriteSlave(SPI_ID_0, spi_data[spi_addr++]);
					state = 2;
				}
			}
			else {
				Spi_WriteSlave(SPI_ID_0, 0x00);
				state = 3;
			}
		break;

		/* Write sequence */
		case 1:
			spi_data[spi_addr++] = data;
			Spi_WriteSlave(SPI_ID_0, 0x00);
			if(spi_addr >= SPI_REG_SIZE)
				state = 3;
		break;

		/* Read sequence */
		case 2:
			if(spi_addr >= SPI_REG_SIZE){
				Spi_WriteSlave(SPI_ID_0, 0x00);
				state = 3;
			}
			else
				Spi_WriteSlave(SPI_ID_0, spi_data[spi_addr++]);
		break;

		/* Address out of boundaries */
		case 3:
			Spi_WriteSlave(SPI_ID_0, 0x00); // Send zeros
		break;
	}
}

int main(){
	Spi_Init(SPI_ID_0, SPI_MODE_SLAVE, SPI_CLOCK_DIV4, SPI_PL_MODE0, SPI_MSB_FIRST);
	Spi_RegisterSlaveRxISR(SPI_ID_0, Spi_SlaveHandler);
	Dio_WritePinMode(DIO_ID_D, DIO_PIN_4, DIO_MODE_OUTPUT);

	Wdt_Init(WDT_MODE_INT, WDT_PRESC_2K);
	Wdt_RegisterISR(Spi_TimeOut);
    sei();

	while(1){
		Dio_TogglePin(DIO_ID_D, DIO_PIN_4);
	}
}