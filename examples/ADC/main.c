#include <avr/io.h>
#include <stdint.h>
#include <util/delay.h>
#include "dio.h"
#include "adc.h"
#include "twi.h"
#include <string.h>

#define ADC_CHANNELS 2

typedef struct {
	uint16_t adc_ch[ADC_CHANNELS];
	uint8_t button;
}twi_txn_t;

static volatile twi_txn_t twi_buffer;

int main(){
	sei();
    twi_config_t TWI_CFG;
	TWI_CFG.Clock = TWI_CLOCK_NORMAL;
	TWI_CFG.Mode = TWI_MODE_SLAVE;
	TWI_CFG.SlaveAddr = 0x50;
	TWI_CFG.SlaveGeneralCall = 0;
	TWI_CFG.SlaveBuffer = (uint8_t*)&twi_buffer;
	TWI_CFG.SlaveBufferSize = sizeof(twi_txn_t);

	Twi_Init(TWI_ID_0, &TWI_CFG);

	Dio_WritePinMode(DIO_ID_D, DIO_PIN_4, DIO_MODE_OUTPUT);
	Dio_WritePinMode(DIO_ID_C, DIO_PIN_0, DIO_MODE_INPUT_PULLUP);
    
    Adc_Init(ADC_ID_0, ADC_REF_AVCC, ADC_PRESC_DIV128, ADC_ALIGN_RIGHT);
	Adc_Calibration(ADC_ID_0);
	while(Adc_State(ADC_ID_0) == ADC_STATE_BUSY); // Wait calibration end

    Adc_WriteSlot(ADC_ID_0, 0, ADC_CH6);
	Adc_WriteSlot(ADC_ID_0, 1, ADC_CH7);
	// Adc_WriteSlot(ADC_ID_0, 2, ADC_CH2);
	// Adc_WriteSlot(ADC_ID_0, 3, ADC_CH3);

	Adc_EnableAutoTrigger(ADC_ID_0, ADC_TRIG_ADC_DONE);
	// ADC0->DIDR0_REG = _BV(0) | _BV(1);

	uint8_t slot = 0;
    while(1){
		// if(Adc_State(0) == ADC_STATE_IDLE){
		// 	Adc_Start(0, slot);
		// 	slot = !slot;
		// }
		twi_buffer.button = Dio_ReadPin(DIO_ID_C, DIO_PIN_0);

		switch(slot){
			case 0:
				if(Adc_Available(ADC_ID_0, slot)){
					twi_buffer.adc_ch[slot] = Adc_Read(ADC_ID_0, slot);
					Adc_ChangeSlot(ADC_ID_0, 1);
					slot = 1;
				}
			break;

			case 1:
				if(Adc_Available(ADC_ID_0, slot)){
					twi_buffer.adc_ch[slot] = Adc_Read(ADC_ID_0, slot);
					Adc_ChangeSlot(ADC_ID_0, 0);
					slot = 0;
				}
			break;
		}
    }
    return 0;
}