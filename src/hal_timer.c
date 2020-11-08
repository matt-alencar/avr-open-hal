/**
 * @file hal_timer.c
 * @author Matheus Alencar Nascimento (matt-alencar)
 * @brief TIM HAL module driver.
 *        This file provides firmware functions to manage the following
 *        functionalities of the Timer (TIM) peripheral:
 *           + Timer initialization / DeInitialization
 *           + Timer interrupt manager (Enable/Disable)
 *           + Timer comparators setup
 *           + Timer output setup
 * 
 **************************************************************************
 * @copyright MIT License.
 * 
 */

#include "hal_timer.h"


static uint8_t Timer_GetID(timer_t *TIMx){
	if(TIMx == TIM0)
		return 0;
#if defined(TIM1)
	else if(TIMx == TIM1)
		return 1;
#endif
#if defined(TIM2)
	else if(TIMx == TIM2)
		return 2;
#endif
#if defined(TIM3)
	else if(TIMx == TIM3)
		return 3;
#endif
#if defined(TIM4)
	else if(TIMx == TIM4)
		return 4;
#endif
#if defined(TIM5)
	else if(TIMx == TIM5)
		return 5;
#endif
	return 0;
}

timer_type_t Timer_GetType(timer_t *TIMx){
	if(TIMx == TIM0)
		return TIMER8_TYPE;
#if defined(TIM1)
	else if(TIMx == TIM1)
		return TIMER16_TYPE;
#endif
#if defined(TIM2)
	else if(TIMx == TIM2)
		return TIMER8_TYPE;
#endif
#if defined(TIM3)
	else if(TIMx == TIM3)
		return TIMER16_TYPE;
#endif
#if defined(TIM4)
	else if(TIMx == TIM4)
		return TIMER16_TYPE;
#endif
#if defined(TIM5)
	else if(TIMx == TIM5)
		return TIMER16_TYPE;
#endif
	return TIMER_TYPE_NOT_DEFINED;
}

void Timer_Enable_Presc(){
	GTCCR = 0;
}

void Timer_Disable_Presc(){
	GTCCR |= TSM;
	GTCCR |= PSRSYNC;
}

uint8_t Timer_IsEnabled(timer_t *TIMx){
	return (TIMx->TCCRB_REG & 0x07) > 0;
}

void Timer_Enable_IRQ(timer_t *TIMx, timer_int_t int_type){
	uint8_t id = Timer_GetID(TIMx);

	TIM_INT_FLAG->TIFR_REG[id] |= _BV(int_type);
	TIM_INT_MASK->TIMSK_REG[id] |= _BV(int_type);
}

void Timer_Disable_IRQ(timer_t *TIMx, timer_int_t int_type){
	uint8_t id = Timer_GetID(TIMx);

	TIM_INT_FLAG->TIFR_REG[id] |= _BV(int_type);
	TIM_INT_MASK->TIMSK_REG[id] &= ~_BV(int_type);
}

void Timer_StructInit(timer_init_t *timer_init){
	timer_init->Clock = TIMER_CLOCK_DISABLE;
	timer_init->Counter = 0;
	timer_init->CompA = 0;
	timer_init->CompB = 0;
	timer_init->CompC = 0;
	timer_init->Mode = TIMER8_MODE_NORMAL;
	timer_init->OutModeA = TIMER_OUTPUT_DISABLE;
	timer_init->OutModeB = TIMER_OUTPUT_DISABLE;
	timer_init->OutModeC = TIMER_OUTPUT_DISABLE;
	timer_init->ICFilter = TIMER_IC_FILER_DISABLE;
	timer_init->ICPolarity = TIMER_IC_POLARITY_FALLING;
}

void Timer_Init(timer_t *TIMx, timer_init_t *timer_init){
	timer_type_t type = Timer_GetType(TIMx);
	uint8_t timer_id = Timer_GetID(TIMx);

	TIM_INT_MASK->TIMSK_REG[timer_id] = 0;
	TIM_INT_FLAG->TIFR_REG[timer_id] = 0xFF;
	
	Timer_SetClock(TIMx, TIMER_CLOCK_DISABLE);

	Timer_SetCounter(TIMx, timer_init->Counter);
	Timer_SetMode(TIMx, timer_init->Mode);
	Timer_SetCompare(TIMx, TIMER_COMP_OCRA, timer_init->CompA);
	Timer_SetCompare(TIMx, TIMER_COMP_OCRB, timer_init->CompB);
	Timer_SetOutputMode(TIMx, TIMER_COMP_OCRA, timer_init->OutModeA);
	Timer_SetOutputMode(TIMx, TIMER_COMP_OCRB, timer_init->OutModeB);

	if(type == TIMER16_TYPE){
		Timer_SetCompare(TIMx, TIMER_COMP_OCRC, timer_init->CompC);
		Timer_SetOutputMode(TIMx, TIMER_COMP_OCRC, timer_init->OutModeC);
		Timer_IC_Config(TIMx, timer_init->ICFilter, timer_init->ICPolarity);
		Timer_IC_SetValue(TIMx, 0);
	}

	Timer_SetClock(TIMx, timer_init->Clock);
}

void Timer_DeInit(timer_t *TIMx){
	timer_type_t type = Timer_GetType(TIMx);
	uint8_t timer_id = Timer_GetID(TIMx);

	TIM_INT_MASK->TIMSK_REG[timer_id] = 0;
	TIM_INT_FLAG->TIFR_REG[timer_id] = 0xFF;

	Timer_SetClock(TIMx, TIMER_CLOCK_DISABLE);

	Timer_SetCounter(TIMx, 0);
	Timer_SetMode(TIMx, TIMER8_MODE_NORMAL);
	Timer_SetCompare(TIMx, TIMER_COMP_OCRA, 0);
	Timer_SetCompare(TIMx, TIMER_COMP_OCRB, 0);
	Timer_SetOutputMode(TIMx, TIMER_COMP_OCRA, TIMER_OUTPUT_DISABLE);
	Timer_SetOutputMode(TIMx, TIMER_COMP_OCRB, TIMER_OUTPUT_DISABLE);

	if(type == TIMER16_TYPE){
		Timer_SetCompare(TIMx, TIMER_COMP_OCRC, 0);
		Timer_SetOutputMode(TIMx, TIMER_COMP_OCRC, TIMER_OUTPUT_DISABLE);
		Timer_IC_Config(TIMx, TIMER_IC_FILER_DISABLE, TIMER_IC_POLARITY_FALLING);
		Timer_IC_SetValue(TIMx, 0);
	}
}


void Timer_SetMode(timer_t *TIMx, uint8_t mode){
	timer_type_t type = Timer_GetType(TIMx);

	if(type == TIMER8_TYPE){
		TIMx->TCCRB_REG &= ~_BV(WGM02);
		TIMx->TCCRA_REG &= ~(_BV(WGM01) | _BV(WGM00));
		TIMx->TCCRA_REG |= (mode & 0x03);
		TIMx->TCCRB_REG |= ((mode & 0x04) << 1);
	}
	else if(type == TIMER16_TYPE){
		TIMx->TCCRB_REG &= ~0x18;
		TIMx->TCCRA_REG &= ~0x03;
		TIMx->TCCRA_REG |= mode & 0x03;
		TIMx->TCCRB_REG |= ((mode & 0x0C) << 1);
	}
}

void Timer_SetClock(timer_t *TIMx, timer_clock_t clock){
	TIMx->TCCRB_REG &= ~0x07;
    TIMx->TCCRB_REG |= clock;
}

void Timer_SetOutputMode(timer_t *TIMx, timer_comp_t comp, timer_out_mode_t mode){
	if(comp == TIMER_COMP_OCRA){
		TIMx->TCCRA_REG &= ~0xC0;
		TIMx->TCCRA_REG |= (mode << 6);
	}
	else if(comp == TIMER_COMP_OCRB){
		TIMx->TCCRA_REG &= ~0x30;
		TIMx->TCCRA_REG |= (mode << 4);
	}
	else if(comp == TIMER_COMP_OCRC){
		TIMx->TCCRA_REG &= ~0x0C;
		TIMx->TCCRA_REG |= (mode << 2);
	}
}

void Timer_SetCounter(timer_t *TIMx, uint16_t value){
	timer_type_t type = Timer_GetType(TIMx);

	if(type == TIMER8_TYPE){
		TIMx->TIMER8_REG.TCNT_REG = (uint8_t)value;
	}
	else if (type == TIMER16_TYPE){
		ATOMIC_BLOCK(ATOMIC_FORCEON){
			TIMx->TIMER16_REG.TCNT_REG = value;
		}
	}
}

uint16_t Timer_GetCounter(timer_t *TIMx){
	timer_type_t type = Timer_GetType(TIMx);
	uint16_t value = 0;

	if(type == TIMER8_TYPE){
		value = TIMx->TIMER8_REG.TCNT_REG;
	}
	else if (type == TIMER16_TYPE){
		ATOMIC_BLOCK(ATOMIC_FORCEON){
			value = TIMx->TIMER16_REG.TCNT_REG;
		}
	}
	return value;
}

uint16_t Timer_GetCompare(timer_t *TIMx, timer_comp_t comp){
	timer_type_t type = Timer_GetType(TIMx);

	if(type == TIMER8_TYPE){
		if(comp == TIMER_COMP_OCRA)
			return TIMx->TIMER8_REG.OCRA_REG;
		else if(comp == TIMER_COMP_OCRB)
			return TIMx->TIMER8_REG.OCRB_REG;
		else
			return 0;
	}
	else if (type == TIMER16_TYPE){
		if(comp == TIMER_COMP_OCRA)
			return TIMx->TIMER16_REG.OCRA_REG;
		else if(comp == TIMER_COMP_OCRB)
			return TIMx->TIMER16_REG.OCRB_REG;
		else if(comp == TIMER_COMP_OCRC)
			return TIMx->TIMER16_REG.OCRC_REG;
		else
			return 0;
	}
	return 0;
}

void Timer_SetCompare(timer_t *TIMx, timer_comp_t comp, uint16_t value){
	timer_type_t type = Timer_GetType(TIMx);

	if(type == TIMER8_TYPE){
		if(comp == TIMER_COMP_OCRA)
			TIMx->TIMER8_REG.OCRA_REG = value;
		else if(comp == TIMER_COMP_OCRB)
			TIMx->TIMER8_REG.OCRB_REG = value;
	}
	else if(type == TIMER16_TYPE){
		if(comp == TIMER_COMP_OCRA)
			TIMx->TIMER16_REG.OCRA_REG = value;
		else if(comp == TIMER_COMP_OCRB)
			TIMx->TIMER16_REG.OCRB_REG = value;
		else if(comp == TIMER_COMP_OCRC)
			TIMx->TIMER16_REG.OCRC_REG = value;
	}
}

void Timer_IC_Config(timer_t *TIMx, timer_ic_filter_t filter, timer_ic_polarity_t polarity){
	timer_type_t type = Timer_GetType(TIMx);

	if(type == TIMER16_TYPE){
		TIMx->TCCRB_REG &= ~(_BV(ICNC1) | _BV(ICES1));
		TIMx->TCCRB_REG |= ((filter << ICNC1) | (polarity << ICES1));
	}
}

void Timer_IC_SetValue(timer_t *TIMx, uint16_t value){
	timer_type_t type = Timer_GetType(TIMx);

	if(type == TIMER16_TYPE){
		ATOMIC_BLOCK(ATOMIC_FORCEON){
			TIMx->TIMER16_REG.ICR_REG = value;
		}
	}
}

uint16_t Timer_IC_GetValue(timer_t *TIMx){
	timer_type_t type = Timer_GetType(TIMx);
	uint16_t value = 0;
	if(type == TIMER16_TYPE){
		ATOMIC_BLOCK(ATOMIC_FORCEON){
			value = TIMx->TIMER16_REG.ICR_REG;
		}
	}
	return value;
}