#ifndef MCU_DEFINE_H
#define MCU_DEFINE_H

#ifdef __cplusplus
#include <cstdint>
#include <cstdbool>
#else
#include <stdint.h>
#include <stdbool.h>
#endif
#include "CommonMacro.h"

#define PANEL_MSG_MAX_LEN   512

/* Define Write and read data length for the example */
#define SLAVE_I2C_RX_BUF_LEN 512
#define SLAVE_I2C_TX_BUF_LEN 256

typedef enum key_action_t{
  KEY_ACTION_NONE = 0,
  KEY_ACTION_SINGLE_CLICK, // 单击
  KEY_ACTION_DOUBLE_CLICK, // 双击
  KEY_ACTION_LONG_PRESS, // 长按
  KEY_ACTION_LONG_PRESS_REPEAT,
} key_action_t;

typedef enum chargeStatus_t
{
	notCharge = 0,
	normalCharge,
	fastCharge,
}chargeStatus_t;

typedef struct present_device
{
    bool i2c;
    bool bq40z50;
    bool mp2762;
}present_device;

typedef struct online_device
{
    bool i2c;
    bool bq40z50;
    bool mp2762;
}online_device;

typedef enum tag_work_mode_t {
	rover_mode = 0,
	base_mode,
}work_mode_t;

typedef enum tag_turn_on_off_t {
	interface_off = 0,
	interface_on,
}turn_on_off_t;

typedef struct tag_SystemInfo_t {
	unsigned char sw_reset_flag;
	unsigned char power_off_flag;

	unsigned char record_status;
	float record_RSC;
	char record_name[16];
	uint8_t record_type;
	uint8_t record_spacetime;
	unsigned char record_op;

	// unsigned char voice_flag;
	unsigned char usb_power_flag;

	work_mode_t work_mode;

	uint8_t coordinate_status;// 1-
	double coordinate_lon;
	double coordinate_lat;
	double coordinate_alt;

	uint8_t satellite_number_used;
	uint8_t satellite_number_track;

	turn_on_off_t gprs_status;

	turn_on_off_t NtripServer_status;
	uint8_t NtripServer_IP[4];
	uint8_t NtripServer_Mountpoint[32];

	turn_on_off_t NtripClient_status;
	uint8_t NtripClient_IP[4];
	uint8_t NtripClient_Mountpoint[32];

	turn_on_off_t radio_status; // 0-rx; 1-tx
	uint8_t radio_mode;
	uint8_t radio_protocol;
	uint8_t radio_channel;

	turn_on_off_t wifi_status;
	uint8_t wifi_mode;
	uint8_t wifi_ip[4];
	uint8_t wifi_ssid[16];

	uint8_t ExternalPower;
	uint8_t mcu_operation_flag;
	uint8_t linux_operation_flag;
	bool sync_flag;
}SystemInfo_t;

typedef enum {
	MD_TX = 0U,
	MD_RX = 1U,
	MD_NONE = 2U,
} stc_i2c_com_mode_t;

/**
 * @brief I2c communication status enum
 */
typedef enum {
	SLAVE_I2C_COM_BUSY = 0U,
	SLAVE_I2C_COM_IDLE = 1U,
} stc_i2c_com_status_t;

/**
 * @brief I2c communication structure
 */
typedef struct {
	stc_i2c_com_mode_t    enMode;         /*!< I2C communication mode*/
	uint32_t              u32RxLen;         /*!< I2C communication data length*/
	uint32_t              u32TxLen;
	uint8_t              *pRxBuf;           /*!< I2C communication data buffer pointer*/
	uint8_t              *pTxBuf;
	volatile uint32_t         u32RecvDataWriteIndex;   /*!< I2C communication data transfer index*/
	volatile uint32_t         u32RecvDataReadIndex;
	volatile uint32_t         u32TransDataWriteIndex;
	volatile uint32_t         u32TransDataReadIndex;
	volatile stc_i2c_com_status_t  enComStatus;    /*!< I2C communication status*/
} stc_i2c_communication_t;

typedef struct tag_PanelMessage_s
{
	int w_index;
	unsigned char syn_head[4];
	unsigned int msg_len;
	unsigned char msg[PANEL_MSG_MAX_LEN+1];
	unsigned char syn_state;
	unsigned char available;
}PanelMessage, *pPanelMessage;

typedef struct BatteryState
{
	uint16_t Percent;
	uint16_t Temp;
	uint16_t Voltage;
	chargeStatus_t chargeStatus;
	bool charge_time_record;
	uint32_t  charge_start_time;
} BatteryState, *pBatteryState;

typedef struct ledState{
    bool isOn;
    uint32_t lastToggleTime;
    uint32_t currentRate;
} ledState;

extern SystemInfo_t systemInfo;
extern BatteryState batteryState;
extern online_device online_devices;

extern PanelMessage PMessage;

extern uint8_t u8SlaveI2CRxBuf[SLAVE_I2C_RX_BUF_LEN];
extern uint8_t u8SlaveI2CTxBuf[SLAVE_I2C_TX_BUF_LEN];
extern stc_i2c_communication_t stcI2cCom;
extern stc_i2c_com_mode_t Slave_I2C_Mode;
extern unsigned int Slave_I2C_TX_Len;

extern volatile uint32_t IIC_ERROR_COUNT;

/*
 * GLOBAL FLAG
 */
extern uint8_t POWER_OFF_FLAG;
#endif