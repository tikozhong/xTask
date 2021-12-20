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

#include "stdarg.h"
#include "string.h"
#include <ctype.h>
#include "stdlib.h"
#include "stdio.h"

#include "board.h"
#include "wlun.h"

#include "httpServer.h"

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
void taskRT4(void);
static void task0(void);
void task4(void);
void task8(void);
void task16(void);
void task32(void);
void task64(void);
void task128(void);

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
void taskRT4(void){
}

/**
  * @brief tack per 4ms
  * @param none
  * @retval None
  */
void task4(void){
	netDev.loop(&netDev.rsrc, 4);
	console.RxPolling(&console.rsrc);
	rs485.RxPolling(&rs485.rsrc);
}

/**
  * @brief tack per 8ms
  * @param none
  * @retval None
  */
void task8(void){

}

/**
  * @brief tack per 16ms
  * @param none
  * @retval None
  */
void task16(void)
{
	u32 i,j,x,len;
	char buff[MAX_CMD_LEN] = {0};
	char *CMD;	
	s32 argv[16];

	//message from uart
	if(console.RxFetchLine(&console.rsrc, buff, MAX_CMD_LEN)){
		for(i=0;i<MAX_CMD_LEN;i++){
			if(buff[i] == 0)	break;
			if(buff[i] == '(' || buff[i] == ')' || buff[i] == ',')	buff[i] = ' ';
			if(buff[i] >= 'A' && buff[i] <= 'Z')	buff[i] += 32;
		}
		// common command
		if(strncmp(buff, "about ", strlen("about ")) == 0){
			print("+ok@%d.about(\"%s\")\r\n", boardAddr, ABOUT);
		}
		else if(strncmp(buff, "help ", strlen("help ")) == 0){
			printS(COMMON_HELP);
			print("+ok@%d.help()\r\n", boardAddr);
		}
		else if(strncmp(buff, addrPre, strlen(addrPre)) == 0){
			CMD = buff+strlen(addrPre);
			if(brdCmd(CMD, boardAddr, printS, print)){	}
			else{		print("+unknown@%s", buff);	}
		}
		else{	printS485(buff);	}
	}
	//message from rs485
	len = rs485.RxFetchFrame(&rs485.rsrc, (u8*)buff, MAX_CMD_LEN);
	if(len>0){
		copyToWlun(buff);
		if(strncmp(buff, addrPre, strlen(addrPre)) == 0){	//match board address
			CMD = buff+strlen(addrPre);
			if(brdCmd(CMD, boardAddr, printS485, print485)){	}
			else{	print485("+unknown@%s", buff);	}
		}
		else{
			printS(buff);	
			//handler_tcps->send(&handler_tcps->rsrc, (u8*)buff, strlen(buff));
		};
	}
//	if(wlunUsing == 92)	// for beckoff polling
//		wlunPolling(16);
	
//	for(i = 0; i < MAX_HTTPSOCK; i++)	httpServer_run(i);	// HTTP server handler
	
}

//FF-U674-1

/**
  * @brief task per 32ms
  * @param none
  * @retval none
  */
void task32(void){
}

/**
  * @brief tack per 64ms
  * @param none
  * @retval None
  */
void task64(void)
{
}

/**
  * @brief tack per 128ms
  * @param none
  * @retval None
  */
void task128(void){
	HAL_GPIO_TogglePin(RUNNING.GPIOx, RUNNING.GPIO_Pin);
}

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
