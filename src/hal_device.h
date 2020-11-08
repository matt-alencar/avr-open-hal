#ifndef _HAL_DEVICE_H_
#define _HAL_DEVICE_H_

#ifdef __cplusplus
extern "C" {
#endif

// #include <inttypes.h>
#include <avr/io.h>
#include <stdint.h>
#include <util/atomic.h>


#if defined (__AVR_ATmega2560__)
    #include "cores/mega2560.h"
#else
    #error "AVR-HAL Incompatible Device"
#endif


#define SET_BIT(REG, BIT)     ((REG) |= (BIT))
#define CLEAR_BIT(REG, BIT)   ((REG) &= ~(BIT))
#define READ_BIT(REG, BIT)    ((REG) & (BIT))
#define CLEAR_REG(REG)        ((REG) = (0x0))
#define WRITE_REG(REG, VAL)   ((REG) = (VAL))
#define READ_REG(REG)         ((REG))
#define MODIFY_REG(REG, CLEARMASK, SETMASK)  WRITE_REG((REG), (((READ_REG(REG)) & (~(CLEARMASK))) | (SETMASK)))
#define BIT_VAL(VAL)     (_BV(VAL))


#ifdef __cplusplus
}
#endif

#endif /* _HAL_DEVICE_H_ */