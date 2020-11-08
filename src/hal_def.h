#ifndef _HAL_DEF_
#define _HAL_DEF_

#ifdef __cplusplus
extern "C" {
#endif

#include "hal_device.h"
#include <stddef.h>
#include <stdbool.h>

#define clockCyclesPerMicrosecond() (F_CPU / 1000000L)
#define clockCyclesToMicroseconds(a) ((a) / clockCyclesPerMicrosecond())
#define microsecondsToClockCycles(a) ((a) * clockCyclesPerMicrosecond())

/** 
  * @brief  HAL Status structures definition  
  */  
typedef enum {
	HAL_OK       = 0x00U,
	HAL_ERROR    = 0x01U,
	HAL_BUSY     = 0x02U
}hal_status_t;

/** 
  * @brief  HAL Lock structures definition  
  */
typedef enum {
	HAL_UNLOCKED = 0x00U,
	HAL_LOCKED   = 0x01U
}hal_lock_t;

#define __HAL_LOCK(__HANDLE__)                                             \
								do{                                        \
									if((__HANDLE__)->Lock == HAL_LOCKED)   \
									{                                      \
										return HAL_BUSY;                   \
									}                                      \
									else                                   \
									{                                      \
										(__HANDLE__)->Lock = HAL_LOCKED;   \
									}                                      \
								}while (0)

#define __HAL_UNLOCK(__HANDLE__)                                       \
								do{                                    \
									(__HANDLE__)->Lock = HAL_UNLOCKED; \
								}while (0)


#ifdef __cplusplus
}
#endif

#endif	/* _HAL_DEF_ */