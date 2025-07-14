/**
 *******************************************************************************
 * @file  i2c/i2c_slave_int/source/main.c
 * @brief Main program of I2C for the Device Driver Library.
 @verbatim
   Change Logs:
   Date             Author          Notes
   2022-03-31       CDT             First version
 @endverbatim
 *******************************************************************************
 * Copyright (C) 2022, Xiaohua Semiconductor Co., Ltd. All rights reserved.
 *
 * This software component is licensed by XHSC under BSD 3-Clause license
 * (the "License"); You may not use this file except in compliance with the
 * License. You may obtain a copy of the License at:
 *                    opensource.org/licenses/BSD-3-Clause
 *
 *******************************************************************************
 */

/*******************************************************************************
 * Include files
 ******************************************************************************/
 #include <Arduino.h>
#include "led.h"
#include "slave_i2c.h"
#include "mcu_config.h"
#include "crc.h"

#include "nm_message.h"
 /**
 * @addtogroup I2C_slave_Int
 * @{
 */

/*******************************************************************************
 * Local type definitions ('typedef')
 ******************************************************************************/

/*******************************************************************************
 * Local pre-processor symbols/macros ('#define')
 ******************************************************************************/

/*******************************************************************************
 * Global variable definitions (declared in header file with 'extern')
 ******************************************************************************/
uint8_t Soft_Reset_Flag;
uint8_t POWER_OFF_FLAG;

PannelMessage PMessage;
/*******************************************************************************
 * Local function prototypes ('static')
 ******************************************************************************/

/*******************************************************************************
 * Local variable definitions ('static')
 ******************************************************************************/
int Pannel_DecodeNMMessage(stc_i2c_communication_t *pSlaveI2CInfo, pPannelMessage pPM)
{
	unsigned char ch = 0;
	int res = 0;

	if (NULL == pSlaveI2CInfo)
		return -1;

	while (1)
	{
		if (pSlaveI2CInfo->u32RecvDataWriteIndex != pSlaveI2CInfo->u32RecvDataReadIndex)
		{
			ch = pSlaveI2CInfo->pRxBuf[pSlaveI2CInfo->u32RecvDataReadIndex];
			res = Pannel_FindNMMessage(ch, pPM, pPM->syn_head);
			if (res < 0)
			{
				memset(pPM, 0, sizeof(PannelMessage));
				break;        ///Links+ 2022-08-09
			}
			else if (pPM->available)
			{
				Slave_I2C_TX_Len = NM_Message_Decode((const char *)pPM->msg);
				if (Slave_I2C_TX_Len)
				{
					Slave_I2C_Mode = MD_TX;
				}
				memset(pPM, 0, sizeof(PannelMessage));
			}

			pPM->syn_head[0] = pPM->syn_head[1];
			pPM->syn_head[1] = pPM->syn_head[2];
			pPM->syn_head[2] = ch;

			if (pSlaveI2CInfo->u32RecvDataReadIndex >= (SLAVE_I2C_RXBUF_LEN - 1))
				pSlaveI2CInfo->u32RecvDataReadIndex = 0;
			else
				pSlaveI2CInfo->u32RecvDataReadIndex++;
		}
		else
		{
			break;
		}
	}

	return 0;
}

int Pannel_FindNMMessage(const char ch, pPannelMessage pPM, const unsigned char *syn)
{
	if ((NULL == pPM) || (NULL == syn))
		return -1;

	if (!pPM->syn_state)
	{
		if ((NM_PROTOCOL_SYN_BYTE3 == ch) && (NM_PROTOCOL_SYN_BYTE2 == syn[2]) && (NM_PROTOCOL_SYN_BYTE1 == syn[1]))
		{
			///Find NM message syn header!
			pPM->syn_state = 1;
			pPM->w_index = 0;
			pPM->msg[pPM->w_index++] = NM_PROTOCOL_SYN_BYTE1; ///0xaa;
			pPM->msg[pPM->w_index++] = NM_PROTOCOL_SYN_BYTE2; ///0x44;
			pPM->msg[pPM->w_index++] = NM_PROTOCOL_SYN_BYTE3; ///0x18;

			return 0;
		}
	}
	else if (pPM->w_index < PANNEL_MSG_MAX_LEN)
	{
		pPM->msg[pPM->w_index++] = ch;
        
		if ((NM_PROTOCOL_MSG_LEN_INDEX_H + 1) == pPM->w_index)
		{
		    pPM->msg_len = pPM->msg[NM_PROTOCOL_MSG_LEN_INDEX_L] | ((int)pPM->msg[NM_PROTOCOL_MSG_LEN_INDEX_H] << 8);
			if ((pPM->msg_len + NM_PROTOCOL_HEADER_LEN + NM_PROTOCOL_CRC_LEN) > PANNEL_MSG_MAX_LEN)
			{
				///NM message length ERR
				return -1;
			}
		}
		else if (pPM->w_index == (pPM->msg_len + NM_PROTOCOL_HEADER_LEN + NM_PROTOCOL_CRC_LEN))
		{
			pPM->available = 1;
			///Find NM message success!
			return 0;
		}
	}
	else
	{
		///ERROR: The NM message length over limit!
		return -1;
	}

	return 0;
}

static unsigned int Calc_NM_MsgCRC(const char *Msg, unsigned int len)
{
	unsigned long crc = 0xFFFFFFFF;
	int n;

	if (NULL == Msg)
		return 1;

	for (n = 0; n < len; n++)
	{
		crc = ulCrcTable[(crc ^ Msg[n]) & 0xff] ^ (crc >> 8);
	}

	return crc ^ 0xFFFFFFFF;
}

static int NM_Message_CRC_Check(const char *pMsg)
{
    int ret = 0;
	unsigned int len = 0;
	unsigned int crc = 0;
	unsigned int msg_crc = 0;

	if (NULL == pMsg)
		return -1;

	len = pMsg[NM_PROTOCOL_MSG_LEN_INDEX_L] | ((int)pMsg[NM_PROTOCOL_MSG_LEN_INDEX_H] << 8);
	len = len + NM_PROTOCOL_HEADER_LEN;

	crc = Calc_NM_MsgCRC(pMsg, len);
	msg_crc = (((int)pMsg[len + 3] << 24) | ((int)pMsg[len + 2] << 16) | ((int)pMsg[len + 1] << 8) | pMsg[len]);

	if (crc == msg_crc)
		ret = 0;
	else
		ret = -1;

	return ret;
}

int NM_Message_Decode(const char *pMsg)
{
	unsigned short msg_id = 0;
	int ret = 0;

	if (NULL == pMsg)
		return 0;

	if (NM_Message_CRC_Check(pMsg))
	{
			return ret;
	}

	msg_id = *((unsigned short *)(&pMsg[NM_PROTOCOL_MSG_ID_INDEX_L]));

	switch (msg_id)
	{
		case NM_PANNEL_INFO_ID:
			ret = NM_Message_Info_Pack(pMsg);
			printf("Info Pack\n");
			break;
		case NM_PANNEL_CTRL_ID:
			ret = NM_Message_Ctrl_Pack(pMsg);
		printf("Ctrl Pack\n");
			break;
		case NM_PANNEL_HOST_ID:
			ret = NM_Message_Host_Pack(pMsg);
		printf("Host Pack\n");
			break;
		case NM_PANNEL_RST_ID:
			ret = NM_Message_Reset_Pack(pMsg);
		    break;
		case NM_PANNEL_POWER_ID:
			ret = NM_Message_PowerOff_Pack(pMsg);
			printf("Power Pack\n");
		    break;
		default:
			break;
	}

	return ret;
}

int NM_Message_Info_Pack(const char *pMsg)
{
	unsigned char *msg = NULL;
	unsigned int crc = 0;
	unsigned short len = 0;

	if (NULL == pMsg)
	  return 0;

	memset(u8SlaveI2CTxBuf, 0, SLAVE_I2C_TXBUF_LEN);
	msg = u8SlaveI2CTxBuf;

	len = NM_PROTOCOL_PINFO_MSG_LEN;
	msg[0] = NM_PROTOCOL_SYN_BYTE1;
    msg[1] = NM_PROTOCOL_SYN_BYTE2;
	msg[2] = NM_PROTOCOL_SYN_BYTE3;
	msg[3] = NM_PROTOCOL_HEADER_LEN;
	msg[NM_PROTOCOL_MSG_ID_INDEX_L] = pMsg[NM_PROTOCOL_MSG_ID_INDEX_L];
	msg[NM_PROTOCOL_MSG_ID_INDEX_H] = pMsg[NM_PROTOCOL_MSG_ID_INDEX_H];
	msg[NM_PROTOCOL_MSG_LEN_INDEX_L] = (char)(len & 0x00FF);
	msg[NM_PROTOCOL_MSG_LEN_INDEX_H] = (char)((len >> 8) & 0x00FF);
	msg[NM_PROTOCOL_MSG_SENDER_INDEX] = NM_PROTOCOL_MSG_SENDER_PANNEL;
	msg[NM_PROTOCOL_MSG_TYPE_INDEX] = NM_MSG_QUERY_RES_TYPE;
	memcpy(&msg[NM_PROTOCOL_PHV_OFFSET], HARDWARE_VERSION, strlen(HARDWARE_VERSION));
	memcpy(&msg[NM_PROTOCOL_PFV_OFFSET], SOFTWARE_VERSION, strlen(SOFTWARE_VERSION));

	if (batteryState.batteryLevelPercent < 101)
	{
		memcpy(&msg[NM_PROTOCOL_PBL_OFFSET], &batteryState.batteryLevelPercent, 2);
	}
	///msg[NM_PROTOCOL_PBT_OFFSET] = BAT_Parameter.TEMP;
	memcpy(&msg[NM_PROTOCOL_PBT_OFFSET], &batteryState.batteryTempC, 2);
	///msg[NM_PROTOCOL_PBV_OFFSET] = BAT_Parameter.VOLTAGE;
	memcpy(&msg[NM_PROTOCOL_PBV_OFFSET], &batteryState.batteryVoltage, 2);
	///msg[NM_PROTOCOL_PPC_OFFSET] = 1;
	msg[NM_PROTOCOL_PSS_OFFSET] = DisplayPannelParameter.sound_status;
	
	crc = Calc_NM_MsgCRC((const char *)msg, (NM_PROTOCOL_PINFO_MSG_PACK_LEN - NM_PROTOCOL_CRC_LEN));

	msg[NM_PROTOCOL_PINFO_MSG_PACK_LEN - 1] = (char)((crc >> 24) & 0x000000FF);
	msg[NM_PROTOCOL_PINFO_MSG_PACK_LEN - 2] = (char)((crc >> 16) & 0x000000FF);
	msg[NM_PROTOCOL_PINFO_MSG_PACK_LEN - 3] = (char)((crc >> 8) & 0x000000FF);
	msg[NM_PROTOCOL_PINFO_MSG_PACK_LEN - 4] = (char)(crc & 0x000000FF);

	return NM_PROTOCOL_PINFO_MSG_PACK_LEN;	
}

int NM_Message_Ctrl_Pack(const char *pMsg)
{
	unsigned char *msg = NULL;
	unsigned int crc = 0;
	unsigned short len = 0;

	if (NULL == pMsg)
	  return 0;

	memset(u8SlaveI2CTxBuf, 0, SLAVE_I2C_TXBUF_LEN);
	msg = u8SlaveI2CTxBuf;

	len = NM_PROTOCOL_PCTRL_MSG_LEN;
	msg[0] = NM_PROTOCOL_SYN_BYTE1;
    msg[1] = NM_PROTOCOL_SYN_BYTE2;
	msg[2] = NM_PROTOCOL_SYN_BYTE3;
	msg[3] = NM_PROTOCOL_HEADER_LEN;
	msg[NM_PROTOCOL_MSG_ID_INDEX_L] = pMsg[NM_PROTOCOL_MSG_ID_INDEX_L];
	msg[NM_PROTOCOL_MSG_ID_INDEX_H] = pMsg[NM_PROTOCOL_MSG_ID_INDEX_H];
	msg[NM_PROTOCOL_MSG_LEN_INDEX_L] = (char)(len & 0x00FF);
	msg[NM_PROTOCOL_MSG_LEN_INDEX_H] = (char)((len >> 8) & 0x00FF);
	msg[NM_PROTOCOL_MSG_SENDER_INDEX] = NM_PROTOCOL_MSG_SENDER_PANNEL;
	msg[NM_PROTOCOL_MSG_TYPE_INDEX] = NM_MSG_QUERY_RES_TYPE;

	msg[NM_PROTOCOL_PRE_OFFSET] = DisplayPannelParameter.reset_flag;
	msg[NM_PROTOCOL_PPC_OFFSET] = DisplayPannelParameter.poweroff_flag;
	
	if (DisplayPannelParameter.poweroff_flag)
		POWER_OFF_FLAG = 1;
	else
		POWER_OFF_FLAG = 0;
	
	msg[NM_PROTOCOL_PRC_OFFSET] = DisplayPannelParameter.record_flag;
	msg[NM_PROTOCOL_PRO_OFFSET] = DisplayPannelParameter.record_op;
	DisplayPannelParameter.record_op = 0;
	
	msg[NM_PROTOCOL_PEP_OFFSET] = DisplayPannelParameter.usb_power_flag;
	
	crc = Calc_NM_MsgCRC((const char *)msg, (NM_PROTOCOL_PCTRL_MSG_PACK_LEN - NM_PROTOCOL_CRC_LEN));

	msg[NM_PROTOCOL_PCTRL_MSG_PACK_LEN - 1] = (char)((crc >> 24) & 0x000000FF);
	msg[NM_PROTOCOL_PCTRL_MSG_PACK_LEN - 2] = (char)((crc >> 16) & 0x000000FF);
	msg[NM_PROTOCOL_PCTRL_MSG_PACK_LEN - 3] = (char)((crc >> 8) & 0x000000FF);
	msg[NM_PROTOCOL_PCTRL_MSG_PACK_LEN - 4] = (char)(crc & 0x000000FF);

	return NM_PROTOCOL_PCTRL_MSG_PACK_LEN;	
}

int NM_Message_Host_Pack(const char *pMsg)
{
	unsigned char *msg = NULL;
	unsigned int crc = 0;
	unsigned short len = 0;
	unsigned char state = 0;
	
	if (NULL == pMsg)
	  return 0;

	memset(u8SlaveI2CTxBuf, 0, SLAVE_I2C_TXBUF_LEN);
	msg = u8SlaveI2CTxBuf;

	state = pMsg[NM_PROTOCOL_HEADER_LEN];
	
	if (!DisplayPannelParameter.record_op)
	{
		if (state)
		{
			DisplayPannelParameter.record_flag = 1;
			Led_Function_switch(HIGH);
		}
		else
		{
			DisplayPannelParameter.record_flag = 0;
			Led_Function_switch(LOW);
		}
	}
	
	state = pMsg[NM_PROTOCOL_HNS_OFFSET];

	if (state)
	{
		Led_Function_switch(HIGH);
	}
	else
	{
		if (!DisplayPannelParameter.record_flag)
			Led_Function_switch(LOW);
		else
			Led_Function_switch(HIGH);
	}
	
	len = NM_PROTOCOL_HOST_MSG_LEN;
	msg[0] = NM_PROTOCOL_SYN_BYTE1;
    msg[1] = NM_PROTOCOL_SYN_BYTE2;
	msg[2] = NM_PROTOCOL_SYN_BYTE3;
	msg[3] = NM_PROTOCOL_HEADER_LEN;
	msg[NM_PROTOCOL_MSG_ID_INDEX_L] = pMsg[NM_PROTOCOL_MSG_ID_INDEX_L];
	msg[NM_PROTOCOL_MSG_ID_INDEX_H] = pMsg[NM_PROTOCOL_MSG_ID_INDEX_H];
	msg[NM_PROTOCOL_MSG_LEN_INDEX_L] = (char)(len & 0x00FF);
	msg[NM_PROTOCOL_MSG_LEN_INDEX_H] = (char)((len >> 8) & 0x00FF);
	msg[NM_PROTOCOL_MSG_SENDER_INDEX] = NM_PROTOCOL_MSG_SENDER_PANNEL;
	msg[NM_PROTOCOL_MSG_TYPE_INDEX] = NM_MSG_SET_RES_TYPE;

	msg[NM_PROTOCOL_HEADER_LEN] = 1;
	
	crc = Calc_NM_MsgCRC((const char *)msg, (NM_PROTOCOL_HOST_MSG_PACK_LEN - NM_PROTOCOL_CRC_LEN));

	msg[NM_PROTOCOL_HOST_MSG_PACK_LEN - 1] = (char)((crc >> 24) & 0x000000FF);
	msg[NM_PROTOCOL_HOST_MSG_PACK_LEN - 2] = (char)((crc >> 16) & 0x000000FF);
	msg[NM_PROTOCOL_HOST_MSG_PACK_LEN - 3] = (char)((crc >> 8) & 0x000000FF);
	msg[NM_PROTOCOL_HOST_MSG_PACK_LEN - 4] = (char)(crc & 0x000000FF);

	return NM_PROTOCOL_HOST_MSG_PACK_LEN;	
}

int NM_Message_Reset_Pack(const char *pMsg)
{
	unsigned char *msg = NULL;
	unsigned int crc = 0;
	unsigned short len = 0;

	if (NULL == pMsg)
	  return 0;

	memset(u8SlaveI2CTxBuf, 0, SLAVE_I2C_TXBUF_LEN);
	msg = u8SlaveI2CTxBuf;

	Soft_Reset_Flag = pMsg[NM_PROTOCOL_HEADER_LEN];
		
	len = NM_PROTOCOL_RST_RESP_MSG_LEN;
	msg[0] = NM_PROTOCOL_SYN_BYTE1;
  msg[1] = NM_PROTOCOL_SYN_BYTE2;
	msg[2] = NM_PROTOCOL_SYN_BYTE3;
	msg[3] = NM_PROTOCOL_HEADER_LEN;
	msg[NM_PROTOCOL_MSG_ID_INDEX_L] = pMsg[NM_PROTOCOL_MSG_ID_INDEX_L];
	msg[NM_PROTOCOL_MSG_ID_INDEX_H] = pMsg[NM_PROTOCOL_MSG_ID_INDEX_H];
	msg[NM_PROTOCOL_MSG_LEN_INDEX_L] = (char)(len & 0x00FF);
	msg[NM_PROTOCOL_MSG_LEN_INDEX_H] = (char)((len >> 8) & 0x00FF);
	msg[NM_PROTOCOL_MSG_SENDER_INDEX] = NM_PROTOCOL_MSG_SENDER_PANNEL;
	msg[NM_PROTOCOL_MSG_TYPE_INDEX] = NM_MSG_SET_RES_TYPE;

	msg[NM_PROTOCOL_HEADER_LEN] = 1;
	
	crc = Calc_NM_MsgCRC((const char *)msg, (NM_PROTOCOL_RST_RESP_MSG_PACK_LEN - NM_PROTOCOL_CRC_LEN));

	msg[NM_PROTOCOL_RST_RESP_MSG_PACK_LEN - 1] = (char)((crc >> 24) & 0x000000FF);
	msg[NM_PROTOCOL_RST_RESP_MSG_PACK_LEN - 2] = (char)((crc >> 16) & 0x000000FF);
	msg[NM_PROTOCOL_RST_RESP_MSG_PACK_LEN - 3] = (char)((crc >> 8) & 0x000000FF);
	msg[NM_PROTOCOL_RST_RESP_MSG_PACK_LEN - 4] = (char)(crc & 0x000000FF);

	return NM_PROTOCOL_RST_RESP_MSG_PACK_LEN;	
}

int NM_Message_PowerOff_Pack(const char *pMsg)
{
	unsigned char *msg = NULL;
	unsigned int crc = 0;
	unsigned short len = 0;

	if (NULL == pMsg)
	  return 0;

	memset(u8SlaveI2CTxBuf, 0, SLAVE_I2C_TXBUF_LEN);
	msg = u8SlaveI2CTxBuf;

	if (pMsg[NM_PROTOCOL_HEADER_LEN])
	{
		DisplayPannelParameter.poweroff_flag = 1;
		POWER_OFF_FLAG = 8;
	}
	
	len = NM_PROTOCOL_POWER_RESP_MSG_LEN;
	msg[0] = NM_PROTOCOL_SYN_BYTE1;
    msg[1] = NM_PROTOCOL_SYN_BYTE2;
	msg[2] = NM_PROTOCOL_SYN_BYTE3;
	msg[3] = NM_PROTOCOL_HEADER_LEN;
	msg[NM_PROTOCOL_MSG_ID_INDEX_L] = pMsg[NM_PROTOCOL_MSG_ID_INDEX_L];
	msg[NM_PROTOCOL_MSG_ID_INDEX_H] = pMsg[NM_PROTOCOL_MSG_ID_INDEX_H];
	msg[NM_PROTOCOL_MSG_LEN_INDEX_L] = (char)(len & 0x00FF);
	msg[NM_PROTOCOL_MSG_LEN_INDEX_H] = (char)((len >> 8) & 0x00FF);
	msg[NM_PROTOCOL_MSG_SENDER_INDEX] = NM_PROTOCOL_MSG_SENDER_PANNEL;
	msg[NM_PROTOCOL_MSG_TYPE_INDEX] = NM_MSG_SET_RES_TYPE;

	msg[NM_PROTOCOL_HEADER_LEN] = 1;
	
	crc = Calc_NM_MsgCRC((const char *)msg, (NM_PROTOCOL_POWER_RESP_MSG_PACK_LEN - NM_PROTOCOL_CRC_LEN));

	msg[NM_PROTOCOL_POWER_RESP_MSG_PACK_LEN - 1] = (char)((crc >> 24) & 0x000000FF);
	msg[NM_PROTOCOL_POWER_RESP_MSG_PACK_LEN - 2] = (char)((crc >> 16) & 0x000000FF);
	msg[NM_PROTOCOL_POWER_RESP_MSG_PACK_LEN - 3] = (char)((crc >> 8) & 0x000000FF);
	msg[NM_PROTOCOL_POWER_RESP_MSG_PACK_LEN - 4] = (char)(crc & 0x000000FF);

	return NM_PROTOCOL_POWER_RESP_MSG_PACK_LEN;	
}
