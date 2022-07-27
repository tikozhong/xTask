/******************** (C) COPYRIGHT 2015 INCUBECN *****************************
* File Name          : task.c
* Author             : Tiko Zhong
* Date First Issued  : 11/18/2016
* Description        : 
*                      
********************************************************************************
* History:
* 11/18/2016: V0.0	
*******************************************************************************/
/* Includes ------------------------------------------------------------------*/
#include "task.h"
#include <stdarg.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <stdio.h>
#include "board.h"

/* Public variables ---------------------------------------------------------*/
/* Private typedef -----------------------------------------------------------*/

/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/

//extern UdpDev_t* handler_udp;

/**********************************************
*  PINs Define
**********************************************/

/* Private function prototypes -----------------------------------------------*/
static u16 tickRT4,tick4,tick8,tick16,tick32,tick64,tick128;
static void taskRT4(void);
static void task0(void);
static void task4(void);
static void task8(void);
static void task16(void);
static void task32(void);
static void task64(void);
static void task128(void);

void taskIrq(void){
	if((++tickRT4&0x03) == 0)	taskRT4();
	tick4++;
	tick8++;
	tick16++;
	tick32++;
	tick64++;
	tick128++;
}
/**
  * @brief polling task
  * @param none
  * @retval None
  */
void taskPolling(void){
	if(initalDone == 0)	return;
	task0();
	if(tick4>=4)		{task4();tick4 = 0;}
	if(tick8>=8)		{task8();tick8 = 0;}
	if(tick16>=16)		{task16();tick16 = 0;}
	if(tick32>=32)		{task32();tick32 = 0;}
	if(tick64>=64)		{task64();tick64 = 0;}
	if(tick128>=128)	{task128();tick128 = 0;}
}

/**
  * @brief quickly task
  * @param none
  * @retval None
  */
static void task0(void){
	console.TxPolling(&console.rsrc);
	rs485.TxPolling(&rs485.rsrc);
}

/**
  * @brief real time task
  * @param none
  * @retval None
  */
static void taskRT4(void){
}

/**
  * @brief tack per 4ms
  * @param none
  * @retval None
  */
static void task4(void){
	console.RxPolling(&console.rsrc);
	rs485.RxPolling(&rs485.rsrc);
}

/**
  * @brief tack per 8ms
  * @param none
  * @retval None
  */
static void task8(void){
}

/**
  * @brief tack per 16ms
  * @param none
  * @retval None
  */
static void commandFormat(char* buff, u16 len){
	u16 i;
	for(i=0;i<len;i++){
		if(buff[i] == 0)	break;
		if(buff[i] == '(' || buff[i] == ')' || buff[i] == ',')	buff[i] = ' ';
		if(buff[i] >= 'A' && buff[i] <= 'Z')	buff[i] += 32;
	}
}

static u8 doCommand(char* buff, u16 len,
		void (*xprintS)(const char* MSG),
		void (*xprint)(const char* FORMAT_ORG, ...),
		void (*forward)(const char* MSG)){
	s32 i;
	char *CMD;
	if(buff==NULL)	return 0;
	//message from uart
	if(sscanf(buff, "%d.", &i)<=0){
		if(strncmp(buff, "help", strlen("help")) == 0){
			printHelp(boardAddr,xprint);
		}
		else if(strncmp(buff, "about", strlen("about")) == 0){
			xprint("+ok@about(%d,\"%s\")\r\n", boardAddr, ABOUT);
		}
		else{
			xprint("+unknown@%s", buff);
		}
	}
	else if(sscanf(buff, "%d.", &i)==1){
		if(i == boardAddr){
			commandFormat(buff, len);
			CMD = (char*)buff + strlen(addrPre);
			if(brdCmd(CMD, boardAddr, xprint)){	}
			else if(inputCmd(&inputDev, CMD, boardAddr, xprint)){	}
			else if(outputCmd(&outputDev, CMD, boardAddr, xprint)){	}
			else{		xprint("+unknown@%s", buff);	}
		}
		else{
			if(forward){
				forward((char*)buff);
			}
		}
	}
	return 1;
}

#define MAX_LINE_LEN	MAX_CMD_LEN
static void task16(void)
{
	u32 len;
	char buff[MAX_LINE_LEN] = {0};
	char *CMD;

	HAL_IWDG_Refresh(&hiwdg);
	//message from uart
	if(fetchLineFromRingBuffer(&console.rsrc.rxRB, (char*)buff, MAX_LINE_LEN)){
		doCommand(buff, MAX_LINE_LEN, printS, print, printS485);
	}

	//message from rs485
	memset(buff,0,MAX_LINE_LEN);
	len = rs485.RxFetchFrame(&rs485.rsrc, (u8*)buff, MAX_LINE_LEN);
	if(len>0){
		if(buff[0] == '+'){
			printS(buff);	// message begins with '+', it means message is from bus
		}
		else{
			doCommand(buff, MAX_LINE_LEN, printS485, print485, NULL);
		}
	}

	inputDev.Polling(&inputDev.rsrc, 16);
}

/**
  * @brief task per 32ms
  * @param none
  * @retval none
  */
static void task32(void){
}

/**
  * @brief tack per 64ms
  * @param none
  * @retval None
  */
static void task64(void){
}

/**
  * @brief tack per 128ms
  * @param none
  * @retval None
  */
static void task128(void){
	HAL_GPIO_TogglePin(RUNNING.GPIOx, RUNNING.GPIO_Pin);
}

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
