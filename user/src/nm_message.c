//
// Created by gtc on 25-7-31.
//

#include <stdio.h>
#include "nm_message.h"
#include "crc.h"
#include "mcu_config.h"

PanelMessage PMessage;

int Panel_DecodeNMMessage(stc_i2c_communication_t *pSlaveI2CInfo, pPanelMessage pPM) {
  unsigned char ch = 0;
  int res = 0;

  if (NULL == pSlaveI2CInfo)
    return -1;

  while (1) {
    if (pSlaveI2CInfo->u32RecvDataWriteIndex !=
        pSlaveI2CInfo->u32RecvDataReadIndex) {
      ch = pSlaveI2CInfo->pRxBuf[pSlaveI2CInfo->u32RecvDataReadIndex];
      res = Panel_FindNMMessage(ch, pPM, pPM->syn_head);
      if (res < 0) {
        memset(pPM, 0, sizeof(PanelMessage));
        break; /// Links+ 2022-08-09
      } else if (pPM->available) {
        Slave_I2C_TX_Len = NM_Message_Decode((const char *)pPM->msg);
        if (Slave_I2C_TX_Len) {
          Slave_I2C_Mode = MD_TX;
        }
        memset(pPM, 0, sizeof(PanelMessage));
      }

      pPM->syn_head[0] = pPM->syn_head[1];
      pPM->syn_head[1] = pPM->syn_head[2];
      pPM->syn_head[2] = ch;

      if (pSlaveI2CInfo->u32RecvDataReadIndex >= (SLAVE_I2C_RX_BUF_LEN - 1))
        pSlaveI2CInfo->u32RecvDataReadIndex = 0;
      else
        pSlaveI2CInfo->u32RecvDataReadIndex++;
    } else {
      break;
    }
  }

  return 0;
}

int Panel_FindNMMessage(const char ch, pPanelMessage pPM, const unsigned char *syn) {
  if ((NULL == pPM) || (NULL == syn))
    return -1;

  if (!pPM->syn_state) {
    if ((NM_PROTOCOL_SYN_BYTE3 == ch) && (NM_PROTOCOL_SYN_BYTE2 == syn[2]) &&
        (NM_PROTOCOL_SYN_BYTE1 == syn[1])) {
      /// Find NM message syn header!
      pPM->syn_state = 1;
      pPM->w_index = 0;
      pPM->msg[pPM->w_index++] = NM_PROTOCOL_SYN_BYTE1; /// 0xaa;
      pPM->msg[pPM->w_index++] = NM_PROTOCOL_SYN_BYTE2; /// 0x44;
      pPM->msg[pPM->w_index++] = NM_PROTOCOL_SYN_BYTE3; /// 0x18;

      return 0;
    }
  } else if (pPM->w_index < PANEL_MSG_MAX_LEN) {
    pPM->msg[pPM->w_index++] = ch;

    if ((NM_PROTOCOL_MSG_LEN_INDEX_H + 1) == pPM->w_index) {
      pPM->msg_len = pPM->msg[NM_PROTOCOL_MSG_LEN_INDEX_L] |
                     ((int)pPM->msg[NM_PROTOCOL_MSG_LEN_INDEX_H] << 8);
      if ((pPM->msg_len + NM_PROTOCOL_HEADER_LEN + NM_PROTOCOL_CRC_LEN) >
          PANEL_MSG_MAX_LEN) {
        /// NM message length ERR
        return -1;
      }
    } else if (pPM->w_index ==
               (pPM->msg_len + NM_PROTOCOL_HEADER_LEN + NM_PROTOCOL_CRC_LEN)) {
      pPM->available = 1;
      /// Find NM message success!
      return 0;
    }
  } else {
    /// ERROR: The NM message length over limit!
    return -1;
  }

  return 0;
}

static unsigned int Calc_NM_MsgCRC(const char *Msg, const unsigned int len) {
  unsigned long crc = 0xFFFFFFFF;

  for (int n = 0; n < len; n++) {
    crc = ulCrcTable[(crc ^ Msg[n]) & 0xff] ^ (crc >> 8);
  }

  return crc ^ 0xFFFFFFFF;
}

static int NM_Message_CRC_Check(const char *pMsg) {
  int ret = 0;
  unsigned int len = 0;
  unsigned int crc = 0;
  unsigned int msg_crc = 0;

  len = pMsg[NM_PROTOCOL_MSG_LEN_INDEX_L] |
        ((int)pMsg[NM_PROTOCOL_MSG_LEN_INDEX_H] << 8);
  len = len + NM_PROTOCOL_HEADER_LEN;

  crc = Calc_NM_MsgCRC(pMsg, len);
  msg_crc = (((int)pMsg[len + 3] << 24) | ((int)pMsg[len + 2] << 16) |
             ((int)pMsg[len + 1] << 8) | pMsg[len]);

  if (crc == msg_crc)
    ret = 0;
  else
    ret = -1;

  return ret;
}


#if 0
int NM_Message_Decode(const char *pMsg) {
  unsigned short msg_id = 0;
  int ret = 0;

  if (NULL == pMsg)
    return 0;

  if (NM_Message_CRC_Check(pMsg)) {
    return ret;
  }

  msg_id = *((unsigned short *)(&pMsg[NM_PROTOCOL_MSG_ID_INDEX_L]));
  switch (msg_id) {
  case NM_PANEL_INFO1_ID:
  case NM_PANEL_INFO2_ID:
  case NM_PANEL_INFO3_ID:
  case NM_PANEL_INFO4_ID:
    ret = NM_Message_Info_Pack(pMsg, msg_id);
    break;

  case NM_PANEL_RST_ID:
    ret = NM_Message_Reset_Pack(pMsg);
    break;

  case NM_PANEL_SET1_ID:
  case NM_PANEL_SET3_ID:
  case NM_PANEL_SET6_ID:
  case NM_PANEL_SET7_ID:
  case NM_PANEL_SET3_1_ID:
    ret = NM_Message_SET_Pack(pMsg, msg_id);
    break;
  default:
    break;
  }

  return ret;
}

int NM_Message_Info_Pack(const char *pMsg, const char NM_PANEL_INFO_ID) {
  unsigned char *msg = NULL;
  unsigned int crc = 0;
  unsigned short len = 0;

  if (NULL == pMsg)
    return 0;

  memset(u8SlaveI2CTxBuf, 0, SLAVE_I2C_TX_BUF_LEN);
  msg = u8SlaveI2CTxBuf;

  switch (NM_PANEL_INFO_ID) {
  case NM_PANEL_INFO1_ID:
    len = NM_PROTOCOL_INFO1_MSG_LEN;
    break;
  case NM_PANEL_INFO2_ID:
    len = NM_PROTOCOL_INFO2_MSG_LEN;
    break;
  case NM_PANEL_INFO3_ID:
    len = NM_PROTOCOL_INFO3_MSG_LEN;
    break;
  case NM_PANEL_INFO4_ID:
    len = NM_PROTOCOL_INFO4_MSG_LEN;
    break;
  default:
    break;
  }

  msg[0] = NM_PROTOCOL_SYN_BYTE1;
  msg[1] = NM_PROTOCOL_SYN_BYTE2;
  msg[2] = NM_PROTOCOL_SYN_BYTE3;
  msg[3] = NM_PROTOCOL_HEADER_LEN;
  msg[NM_PROTOCOL_MSG_ID_INDEX_L] = pMsg[NM_PROTOCOL_MSG_ID_INDEX_L];
  msg[NM_PROTOCOL_MSG_ID_INDEX_H] = pMsg[NM_PROTOCOL_MSG_ID_INDEX_H];
  msg[NM_PROTOCOL_MSG_LEN_INDEX_L] = (char)(len & 0x00FF);
  msg[NM_PROTOCOL_MSG_LEN_INDEX_H] = (char)((len >> 8) & 0x00FF);
  msg[NM_PROTOCOL_MSG_SENDER_INDEX] = NM_PROTOCOL_MSG_SENDER_PANEL;
  msg[NM_PROTOCOL_MSG_TYPE_INDEX] = NM_MSG_QUERY_RES_TYPE;

  switch (NM_PANEL_INFO_ID) {
  case NM_PANEL_INFO1_ID:
    memcpy(&msg[NM_PROTOCOL_HEADER_LEN + 0], HARDWARE_VERSION,
           strlen(HARDWARE_VERSION)); // Ӳ¼þ°汾ºō
    memcpy(&msg[NM_PROTOCOL_HEADER_LEN + 8], SOFTWARE_VERSION,
           strlen(SOFTWARE_VERSION)); // ¹̼þ°汾ºō
    memcpy(&msg[NM_PROTOCOL_HEADER_LEN + 18], &batteryState.Percent,
           2); // µ糘µ灿
    memcpy(&msg[NM_PROTOCOL_HEADER_LEN + 20], &batteryState.Temp,
           2); // µ糘΂¶ȍ
    memcpy(&msg[NM_PROTOCOL_HEADER_LEN + 22], &batteryState.Voltage,
           2); // µ糘µ瑹
    break;
  case NM_PANEL_INFO2_ID:
    msg[NM_PROTOCOL_HEADER_LEN + 0] = systemInfo.sw_reset_flag;    //主机复位控制
    msg[NM_PROTOCOL_HEADER_LEN + 1] = systemInfo.power_off_flag; //主机关机控制
    msg[NM_PROTOCOL_HEADER_LEN + 2] = systemInfo.record_status; //静态记录状态
    msg[NM_PROTOCOL_HEADER_LEN + 3] = systemInfo.record_op;     //静态记录开关
    msg[NM_PROTOCOL_HEADER_LEN + 4] = 0x00;           //充电电源接入
    break;
  case NM_PANEL_INFO3_ID:
    msg[NM_PROTOCOL_HEADER_LEN + 0] = systemInfo.work_mode;
    msg[NM_PROTOCOL_HEADER_LEN + 1] = systemInfo.satellite_number_used;
    msg[NM_PROTOCOL_HEADER_LEN + 2] = systemInfo.coordinate_status;
    msg[NM_PROTOCOL_HEADER_LEN + 3] = systemInfo.gprs_status;
    msg[NM_PROTOCOL_HEADER_LEN + 4] = systemInfo.NtripServer_status;
    msg[NM_PROTOCOL_HEADER_LEN + 5] = systemInfo.NtripClient_status;
    msg[NM_PROTOCOL_HEADER_LEN + 6] = systemInfo.radio_status;
    if (!systemInfo.sync_flag) {
      msg[NM_PROTOCOL_HEADER_LEN + 11] = 0;
      systemInfo.sync_flag = 1;
    }
    else {
      msg[NM_PROTOCOL_HEADER_LEN + 11] = systemInfo.mcu_operation_flag;
    }
    systemInfo.mcu_operation_flag = 0;
    memcpy(&msg[NM_PROTOCOL_HEADER_LEN + 12], &systemInfo.NtripServer_IP, 4);
    memcpy(&msg[NM_PROTOCOL_HEADER_LEN + 16], &systemInfo.NtripServer_Mountpoint, 32);
    memcpy(&msg[NM_PROTOCOL_HEADER_LEN + 48], &systemInfo.NtripClient_IP, 4);
    memcpy(&msg[NM_PROTOCOL_HEADER_LEN + 52], &systemInfo.NtripClient_Mountpoint, 32);
    msg[NM_PROTOCOL_HEADER_LEN + 84] = systemInfo.radio_mode;
    msg[NM_PROTOCOL_HEADER_LEN + 85] = systemInfo.radio_protocol;
    msg[NM_PROTOCOL_HEADER_LEN + 86] = systemInfo.radio_channel;
    break;
  case NM_PANEL_INFO4_ID:
    msg[NM_PROTOCOL_HEADER_LEN + 0] = systemInfo.record_status;
    memcpy(&msg[NM_PROTOCOL_HEADER_LEN + 4], &systemInfo.record_RSC, 4);
    memcpy(&msg[NM_PROTOCOL_HEADER_LEN + 8], &systemInfo.record_name, 16);
    msg[NM_PROTOCOL_HEADER_LEN + 24] = systemInfo.record_type;
    msg[NM_PROTOCOL_HEADER_LEN + 25] = systemInfo.record_spacetime;
    msg[NM_PROTOCOL_HEADER_LEN + 26] = systemInfo.mcu_operation_flag;
    systemInfo.mcu_operation_flag = 0;
    break;
  default:
    break;
  }

  crc = Calc_NM_MsgCRC((const char *)msg, (len + NM_PROTOCOL_HEADER_LEN));
  msg[len + NM_PROTOCOL_HEADER_LEN + NM_PROTOCOL_CRC_LEN - 1] =
      (char)((crc >> 24) & 0x000000FF);
  msg[len + NM_PROTOCOL_HEADER_LEN + NM_PROTOCOL_CRC_LEN - 2] =
      (char)((crc >> 16) & 0x000000FF);
  msg[len + NM_PROTOCOL_HEADER_LEN + NM_PROTOCOL_CRC_LEN - 3] =
      (char)((crc >> 8) & 0x000000FF);
  msg[len + NM_PROTOCOL_HEADER_LEN + NM_PROTOCOL_CRC_LEN - 4] =
      (char)(crc & 0x000000FF);

  return len + NM_PROTOCOL_HEADER_LEN + NM_PROTOCOL_CRC_LEN;
}

char Recv_message = 0;

int NM_Message_SET_Pack(const char *pMsg, const char NM_PANEL_SET_ID) {
  unsigned char *msg = NULL;
  unsigned int crc = 0;
  unsigned short len = 0;

  if (NULL == pMsg)
    return 0;

  memset(u8SlaveI2CTxBuf, 0, SLAVE_I2C_TX_BUF_LEN);
  msg = u8SlaveI2CTxBuf;

  Recv_message = 1;

  switch (NM_PANEL_SET_ID) {
  case NM_PANEL_SET1_ID:
    if (systemInfo.mcu_operation_flag == 0) {
      systemInfo.work_mode = pMsg[NM_PROTOCOL_HEADER_LEN + 0];
      systemInfo.satellite_number_used = pMsg[NM_PROTOCOL_HEADER_LEN + 1];
      systemInfo.coordinate_status = pMsg[NM_PROTOCOL_HEADER_LEN + 2];
      //	gnss_status=1;
      systemInfo.gprs_status = pMsg[NM_PROTOCOL_HEADER_LEN + 3];
      systemInfo.NtripServer_status = pMsg[NM_PROTOCOL_HEADER_LEN + 4];
      systemInfo.NtripClient_status = pMsg[NM_PROTOCOL_HEADER_LEN + 5];
      systemInfo.radio_status = pMsg[NM_PROTOCOL_HEADER_LEN + 6];

      systemInfo.satellite_number_track = pMsg[NM_PROTOCOL_HEADER_LEN + 7];
      systemInfo.ExternalPower = pMsg[NM_PROTOCOL_HEADER_LEN + 8];
      systemInfo.linux_operation_flag = pMsg[NM_PROTOCOL_HEADER_LEN + 11];
      memcpy(&systemInfo.NtripServer_IP, &pMsg[NM_PROTOCOL_HEADER_LEN + 12], 4);
      memcpy(&systemInfo.NtripServer_Mountpoint, &pMsg[NM_PROTOCOL_HEADER_LEN + 16], 32);
      memcpy(&systemInfo.NtripClient_IP, &pMsg[NM_PROTOCOL_HEADER_LEN + 48], 4);
      memcpy(&systemInfo.NtripClient_Mountpoint, &pMsg[NM_PROTOCOL_HEADER_LEN + 52], 32);
      systemInfo.radio_mode = pMsg[NM_PROTOCOL_HEADER_LEN + 84];
      systemInfo.radio_protocol = pMsg[NM_PROTOCOL_HEADER_LEN + 85];
      systemInfo.radio_channel = pMsg[NM_PROTOCOL_HEADER_LEN + 86];
      if (pMsg[NM_PROTOCOL_HEADER_LEN + 87])
        systemInfo.power_off_flag = 1;
    }
    break;
  case NM_PANEL_SET3_ID:
    systemInfo.coordinate_status = pMsg[NM_PROTOCOL_HEADER_LEN + 0];
    // systemInfo.designation_ew = pMsg[NM_PROTOCOL_HEADER_LEN + 1];
    // systemInfo.designation_sn = pMsg[NM_PROTOCOL_HEADER_LEN + 2];
    // systemInfo.reserved_set3 = pMsg[NM_PROTOCOL_HEADER_LEN + 3];
    memcpy(&systemInfo.coordinate_lon, &pMsg[NM_PROTOCOL_HEADER_LEN + 4], 8);
    memcpy(&systemInfo.coordinate_lat, &pMsg[NM_PROTOCOL_HEADER_LEN + 12], 8);
    memcpy(&systemInfo.coordinate_alt, &pMsg[NM_PROTOCOL_HEADER_LEN + 20], 8);
    break;
  case NM_PANEL_SET6_ID:
    if (systemInfo.mcu_operation_flag == 0) {
      systemInfo.record_status = pMsg[NM_PROTOCOL_HEADER_LEN + 0];
      memcpy(&systemInfo.record_RSC, &pMsg[NM_PROTOCOL_HEADER_LEN + 4], 4);
      memcpy(&systemInfo.record_name, &pMsg[NM_PROTOCOL_HEADER_LEN + 8], 16);
      systemInfo.record_type = pMsg[NM_PROTOCOL_HEADER_LEN + 24];
      systemInfo.record_spacetime = pMsg[NM_PROTOCOL_HEADER_LEN + 25];
      systemInfo.linux_operation_flag = pMsg[NM_PROTOCOL_HEADER_LEN + 26];
    }
    break;
  case NM_PANEL_SET7_ID:
    if (systemInfo.mcu_operation_flag == 0) {
      systemInfo.wifi_status = pMsg[NM_PROTOCOL_HEADER_LEN + 0];
      systemInfo.wifi_mode = pMsg[NM_PROTOCOL_HEADER_LEN + 1];
      systemInfo.linux_operation_flag = pMsg[NM_PROTOCOL_HEADER_LEN + 3];
      memcpy(&systemInfo.wifi_ip, &pMsg[NM_PROTOCOL_HEADER_LEN + 4], 4);
      memcpy(&systemInfo.wifi_ssid, &pMsg[NM_PROTOCOL_HEADER_LEN + 8], 16);
    }
    break;
  default:
    break;
  }

  len = NM_PROTOCOL_SET_MSG_LEN;
  msg[0] = NM_PROTOCOL_SYN_BYTE1;
  msg[1] = NM_PROTOCOL_SYN_BYTE2;
  msg[2] = NM_PROTOCOL_SYN_BYTE3;
  msg[3] = NM_PROTOCOL_HEADER_LEN;
  msg[NM_PROTOCOL_MSG_ID_INDEX_L] = pMsg[NM_PROTOCOL_MSG_ID_INDEX_L];
  msg[NM_PROTOCOL_MSG_ID_INDEX_H] = pMsg[NM_PROTOCOL_MSG_ID_INDEX_H];
  msg[NM_PROTOCOL_MSG_LEN_INDEX_L] = (char)(len & 0x00FF);
  msg[NM_PROTOCOL_MSG_LEN_INDEX_H] = (char)((len >> 8) & 0x00FF);
  msg[NM_PROTOCOL_MSG_SENDER_INDEX] = NM_PROTOCOL_MSG_SENDER_PANEL;
  msg[NM_PROTOCOL_MSG_TYPE_INDEX] = NM_MSG_SET_RES_TYPE;

  msg[NM_PROTOCOL_HEADER_LEN + 0] = 1;
  msg[NM_PROTOCOL_HEADER_LEN + 1] = 0;

  crc = Calc_NM_MsgCRC((const char *)msg,
                       (NM_PROTOCOL_SET_MSG_PACK_LEN - NM_PROTOCOL_CRC_LEN));

  msg[NM_PROTOCOL_SET_MSG_PACK_LEN - 1] = (char)((crc >> 24) & 0x000000FF);
  msg[NM_PROTOCOL_SET_MSG_PACK_LEN - 2] = (char)((crc >> 16) & 0x000000FF);
  msg[NM_PROTOCOL_SET_MSG_PACK_LEN - 3] = (char)((crc >> 8) & 0x000000FF);
  msg[NM_PROTOCOL_SET_MSG_PACK_LEN - 4] = (char)(crc & 0x000000FF);

  return NM_PROTOCOL_SET_MSG_PACK_LEN;
}

int NM_Message_Reset_Pack(const char *pMsg) {
  unsigned char *msg = NULL;
  unsigned int crc = 0;
  unsigned short len = 0;

  if (NULL == pMsg)
    return 0;

  memset(u8SlaveI2CTxBuf, 0, SLAVE_I2C_TX_BUF_LEN);
  msg = u8SlaveI2CTxBuf;

  systemInfo.sw_reset_flag = pMsg[NM_PROTOCOL_HEADER_LEN];

  len = NM_PROTOCOL_RST_RESP_MSG_LEN;
  msg[0] = NM_PROTOCOL_SYN_BYTE1;
  msg[1] = NM_PROTOCOL_SYN_BYTE2;
  msg[2] = NM_PROTOCOL_SYN_BYTE3;
  msg[3] = NM_PROTOCOL_HEADER_LEN;
  msg[NM_PROTOCOL_MSG_ID_INDEX_L] = pMsg[NM_PROTOCOL_MSG_ID_INDEX_L];
  msg[NM_PROTOCOL_MSG_ID_INDEX_H] = pMsg[NM_PROTOCOL_MSG_ID_INDEX_H];
  msg[NM_PROTOCOL_MSG_LEN_INDEX_L] = (char)(len & 0x00FF);
  msg[NM_PROTOCOL_MSG_LEN_INDEX_H] = (char)((len >> 8) & 0x00FF);
  msg[NM_PROTOCOL_MSG_SENDER_INDEX] = NM_PROTOCOL_MSG_SENDER_PANEL;
  msg[NM_PROTOCOL_MSG_TYPE_INDEX] = NM_MSG_SET_RES_TYPE;

  msg[NM_PROTOCOL_HEADER_LEN] = 1;

  crc = Calc_NM_MsgCRC((const char *)msg, (NM_PROTOCOL_RST_RESP_MSG_PACK_LEN -
                                           NM_PROTOCOL_CRC_LEN));

  msg[NM_PROTOCOL_RST_RESP_MSG_PACK_LEN - 1] = (char)((crc >> 24) & 0x000000FF);
  msg[NM_PROTOCOL_RST_RESP_MSG_PACK_LEN - 2] = (char)((crc >> 16) & 0x000000FF);
  msg[NM_PROTOCOL_RST_RESP_MSG_PACK_LEN - 3] = (char)((crc >> 8) & 0x000000FF);
  msg[NM_PROTOCOL_RST_RESP_MSG_PACK_LEN - 4] = (char)(crc & 0x000000FF);

  return NM_PROTOCOL_RST_RESP_MSG_PACK_LEN;
}
#else
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
		break;
	case NM_PANNEL_CTRL_ID:
		ret = NM_Message_Ctrl_Pack(pMsg);
		break;
	case NM_PANNEL_HOST_ID:
		ret = NM_Message_Host_Pack(pMsg);
		break;
	case NM_PANNEL_RST_ID:
		ret = NM_Message_Reset_Pack(pMsg);
		break;
	case NM_PANNEL_POWER_ID:
		ret = NM_Message_PowerOff_Pack(pMsg);
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

	memset(u8SlaveI2CTxBuf, 0, SLAVE_I2C_TX_BUF_LEN);
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

	if (batteryState.Percent < 101)
	{
		memcpy(&msg[NM_PROTOCOL_PBL_OFFSET], &batteryState.Percent, 2);
	}
	memcpy(&msg[NM_PROTOCOL_PBT_OFFSET], &batteryState.Temp, 2);
	memcpy(&msg[NM_PROTOCOL_PBV_OFFSET], &batteryState.Voltage, 2);

	msg[NM_PROTOCOL_PSS_OFFSET] = 0x00;

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

	memset(u8SlaveI2CTxBuf, 0, SLAVE_I2C_TX_BUF_LEN);
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

	msg[NM_PROTOCOL_PRE_OFFSET] = systemInfo.sw_reset_flag;
	msg[NM_PROTOCOL_PPC_OFFSET] = systemInfo.power_off_flag;

	if (systemInfo.power_off_flag)
		POWER_OFF_FLAG = 1;
	else
		POWER_OFF_FLAG = 0;

	msg[NM_PROTOCOL_PRC_OFFSET] = systemInfo.record_status;
	msg[NM_PROTOCOL_PRO_OFFSET] = systemInfo.record_op;
	systemInfo.record_op = 0;

	msg[NM_PROTOCOL_PEP_OFFSET] = systemInfo.usb_power_flag;

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

	memset(u8SlaveI2CTxBuf, 0, SLAVE_I2C_TX_BUF_LEN);
	msg = u8SlaveI2CTxBuf;

	state = pMsg[NM_PROTOCOL_HEADER_LEN];

	if (!systemInfo.record_op)
	{
		if (state)
		{
			systemInfo.record_status = 1;
			// FUNC_Led_Switch(1);
		}
		else
		{
			systemInfo.record_status = 0;
			// FUNC_Led_Switch(0);
		}
	}

	state = pMsg[NM_PROTOCOL_HNS_OFFSET];

	// if (state)
	// 	FUNC_Led_Switch(1);
	// else
	// {
	// 	if (!DisplayPannelParameter.record_flag)
	// 		FUNC_Led_Switch(0);
	// 	else
	// 		FUNC_Led_Switch(1);
	// }

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

	memset(u8SlaveI2CTxBuf, 0, SLAVE_I2C_TX_BUF_LEN);
	msg = u8SlaveI2CTxBuf;

	systemInfo.sw_reset_flag = pMsg[NM_PROTOCOL_HEADER_LEN];

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

	memset(u8SlaveI2CTxBuf, 0, SLAVE_I2C_TX_BUF_LEN);
	msg = u8SlaveI2CTxBuf;

	if (pMsg[NM_PROTOCOL_HEADER_LEN])
	{
		systemInfo.power_off_flag = 1;
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
#endif