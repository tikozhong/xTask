/**********************************************************
filename: task.h
**********************************************************/

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef _TASK_H
#define _TASK_H
#include "misc.h"

/* output variables for extern function --------------------------------------*/
extern void taskIrq(void);			//put in 1ms irq callback
extern void taskPolling(void);	//put in main loop

#endif

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
