//
// Created by gtc on 25-7-31.
//

#ifndef NM_MESSAGE_H
#define NM_MESSAGE_H

#include "hc32_ddl.h"
#include "mcu_define.h"

#if 0
#define NM_PROTOCOL_MSG_SENDER_DEV    0x01
#define NM_PROTOCOL_MSG_SENDER_APP    0x00
#define NM_PROTOCOL_MSG_SENDER_PANEL  0x03

#define NM_MSG_QUERY_TYPE            0x00
#define NM_MSG_QUERY_RES_TYPE        0x01
#define NM_MSG_SET_TYPE              0x02
#define NM_MSG_SET_RES_TYPE          0x03
#define NM_MSG_ERR_REP_TYPE          0x05

#define NM_MSG_CRC_ERR_REP_ID        0x01
#define NM_MSG_MSG_ID_ERR_REP_ID     0x02
#define NM_MSG_MODULE_ID_ERR_REP_ID  0x03



#define NM_PROTOCOL_SYN_BYTE1    0xaa
#define NM_PROTOCOL_SYN_BYTE2    0x44
#define NM_PROTOCOL_SYN_BYTE3    0x18
#define NM_PROTOCOL_HEADER_LEN   20    //֡ͷ³¤¶ȍ
#define NM_PROTOCOL_MSG_ID_INDEX_L   4   //MSG_IDλփ
#define NM_PROTOCOL_MSG_ID_INDEX_H   5   //MSG_IDλփ
#define NM_PROTOCOL_MSG_LEN_INDEX_L       12
#define NM_PROTOCOL_MSG_LEN_INDEX_H       13
#define NM_PROTOCOL_MSG_SENDER_INDEX      16
#define NM_PROTOCOL_MSG_TYPE_INDEX        17
#define NM_PROTOCOL_MSG_INTERVAL_INDEX    19
#define NM_PROTOCOL_CRC_LEN      4     //CRC³¤¶ȍ



#define NM_PANEL_INFO1_ID                 0x01
#define NM_PROTOCOL_INFO1_MSG_LEN         36///32    //ʽ¾ݳ¤¶ȍ
#define NM_PROTOCOL_INFO1_MSG_PACK_LEN    60///56    //ʽ¾ݰü³¤¶ȍ



#define NM_PANEL_INFO2_ID                 0x02
#define NM_PROTOCOL_INFO2_MSG_LEN          8
#define NM_PROTOCOL_PINFO2_MSG_PACK_LEN    32


#define NM_PANEL_INFO3_ID                 0x07
#define NM_PROTOCOL_INFO3_MSG_LEN          88
#define NM_PROTOCOL_PINFO3_MSG_PACK_LEN    112


#define NM_PANEL_INFO4_ID                 0x0A
#define NM_PROTOCOL_INFO4_MSG_LEN          28
#define NM_PROTOCOL_PINFO4_MSG_PACK_LEN    52



#define NM_PANEL_RST_ID                       0x04
#define NM_PROTOCOL_RST_RESP_MSG_LEN           2
#define NM_PROTOCOL_RST_RESP_MSG_PACK_LEN      26



#define NM_PANEL_SET1_ID                           0x06
#define NM_PANEL_SET3_ID                           0x08
#define NM_PANEL_SET6_ID                           0x09
#define NM_PANEL_SET7_ID                           0x0C

#define NM_PANEL_SET3_1_ID                           0x03

#define NM_PROTOCOL_SET_MSG_LEN                    2
#define NM_PROTOCOL_SET_MSG_PACK_LEN               26
#else
#define NM_PROTOCOL_SYN_BYTE1 0xaa
#define NM_PROTOCOL_SYN_BYTE2 0x44
#define NM_PROTOCOL_SYN_BYTE3 0x18
#define NM_PROTOCOL_HEADER_LEN 20
#define NM_PROTOCOL_CRC_LEN 4
#define NM_PROTOCOL_MSG_ID_INDEX_L 4
#define NM_PROTOCOL_MSG_ID_INDEX_H 5
#define NM_PROTOCOL_MSG_LEN_INDEX_L 12
#define NM_PROTOCOL_MSG_LEN_INDEX_H 13
#define NM_PROTOCOL_MSG_SENDER_INDEX 16
#define NM_PROTOCOL_MSG_TYPE_INDEX 17
#define NM_PROTOCOL_MSG_INTERVAL_INDEX 19

#define NM_PROTOCOL_MSG_SENDER_DEV 0x01
#define NM_PROTOCOL_MSG_SENDER_APP 0x00
#define NM_PROTOCOL_MSG_SENDER_PANNEL 0x03

#define NM_MSG_QUERY_TYPE 0x00
#define NM_MSG_QUERY_RES_TYPE 0x01
#define NM_MSG_SET_TYPE 0x02
#define NM_MSG_SET_RES_TYPE 0x03
#define NM_MSG_ERR_REP_TYPE 0x05

#define NM_MSG_CRC_ERR_REP_ID 0x01
#define NM_MSG_MSG_ID_ERR_REP_ID 0x02
#define NM_MSG_MODULE_ID_ERR_REP_ID 0x03

#define NM_PANNEL_INFO_ID 0x01
#define NM_PROTOCOL_PINFO_MSG_LEN 36	  /// 32
#define NM_PROTOCOL_PINFO_MSG_PACK_LEN 60 /// 56
#define NM_PROTOCOL_PHV_OFFSET (NM_PROTOCOL_HEADER_LEN + 0)
#define NM_PROTOCOL_PFV_OFFSET (NM_PROTOCOL_HEADER_LEN + 8)
#define NM_PROTOCOL_PBR_OFFSET (NM_PROTOCOL_HEADER_LEN + 16)
#define NM_PROTOCOL_PBL_OFFSET (NM_PROTOCOL_HEADER_LEN + 18)
#define NM_PROTOCOL_PBT_OFFSET (NM_PROTOCOL_HEADER_LEN + 20)
#define NM_PROTOCOL_PBV_OFFSET (NM_PROTOCOL_HEADER_LEN + 22)
#define NM_PROTOCOL_PSS_OFFSET (NM_PROTOCOL_HEADER_LEN + 26)

#define NM_PANNEL_CTRL_ID 0x02
#define NM_PROTOCOL_PCTRL_MSG_LEN 8
#define NM_PROTOCOL_PCTRL_MSG_PACK_LEN 32
#define NM_PROTOCOL_PRE_OFFSET (NM_PROTOCOL_HEADER_LEN + 0)
#define NM_PROTOCOL_PPC_OFFSET (NM_PROTOCOL_HEADER_LEN + 1)
#define NM_PROTOCOL_PRC_OFFSET (NM_PROTOCOL_HEADER_LEN + 2)
#define NM_PROTOCOL_PRO_OFFSET (NM_PROTOCOL_HEADER_LEN + 3)
#define NM_PROTOCOL_PEP_OFFSET (NM_PROTOCOL_HEADER_LEN + 4)

#define NM_PANNEL_HOST_ID 0x03
#define NM_PROTOCOL_HOST_MSG_LEN 2
#define NM_PROTOCOL_HOST_MSG_PACK_LEN 26
#define NM_PROTOCOL_HR1_OFFSET (NM_PROTOCOL_HEADER_LEN + 0)
#define NM_PROTOCOL_HR2_OFFSET (NM_PROTOCOL_HEADER_LEN + 1)
#define NM_PROTOCOL_HNS_OFFSET (NM_PROTOCOL_HEADER_LEN + 4)

#define NM_PANNEL_RST_ID 0x04
#define NM_PROTOCOL_RST_RESP_MSG_LEN 2
#define NM_PROTOCOL_RST_RESP_MSG_PACK_LEN 26
#define NM_PROTOCOL_RRST_OFFSET (NM_PROTOCOL_HEADER_LEN + 0)

#define NM_PANNEL_POWER_ID 0x05
#define NM_PROTOCOL_POWER_RESP_MSG_LEN 2
#define NM_PROTOCOL_POWER_RESP_MSG_PACK_LEN 26
#endif
#ifdef __cplusplus
extern "C"
{
#endif
int Panel_DecodeNMMessage(stc_i2c_communication_t *pSlaveI2CInfo, pPanelMessage pPM);
int Panel_FindNMMessage(char ch, pPanelMessage pPM, const unsigned char *syn);
int NM_Message_Decode(const char *pMsg);

// int NM_Message_Info_Pack(const char *pMsg , char NM_PANEL_INFO_ID);
// int NM_Message_SET_Pack(const char *pMsg , char NM_PANEL_SET_ID);
// int NM_Message_Reset_Pack(const char *pMsg);
    int NM_Message_Info_Pack(const char *pMsg);
    int NM_Message_Ctrl_Pack(const char *pMsg);
    int NM_Message_Host_Pack(const char *pMsg);
    int NM_Message_Reset_Pack(const char *pMsg);
    int NM_Message_PowerOff_Pack(const char *pMsg);
#ifdef __cplusplus
}
#endif

#endif //NM_MESSAGE_H
