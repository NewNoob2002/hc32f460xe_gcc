#ifndef MCU_DEFINE_H
#define MCU_DEFINE_H

#include <stdint.h>
#include <stdbool.h>

typedef enum {
  KEY_ACTION_NONE = 0,
  KEY_ACTION_SINGLE_CLICK, // 单击
  KEY_ACTION_DOUBLE_CLICK, // 双击
  KEY_ACTION_LONG_PRESS, // 长按
	KEY_ACTION_LONG_PRESS_REPEAT,
} key_action_t;

typedef struct _present_device
{
    bool i2c = true;
    bool bq40z50 = true;
    bool mp2762 = true;
}present_device;

typedef struct _online_device
{
    bool i2c = false;
    bool i2c_slave = false;
    bool bq40z50 = false;
    bool mp2762 = false;
}online_device;

typedef struct tag_SystemParameter_s
{
	  unsigned char hw_version[8];
	  unsigned char sw_version[8];
	  unsigned short sound_status;
	  unsigned char reset_flag;
	  unsigned char poweroff_flag;
	  unsigned char record_flag;
	  unsigned char record_op;
	  unsigned char voice_flag;
	  unsigned char usb_power_flag;
}SystemParameter;

enum chargeStatus
{
    notCharge = 0,
    normalCharge,
    fastCharge,
};

typedef struct _BatteryState
{
    float batteryLevelPercent = 50.0; 
    // SOC measured from fuel gauge, in %. Used in multiple places (display, serial debug, log)
    float batteryVoltage = 0.0;
    float batteryChargingPercentPerHour = 50.0;
    float batteryTempC = 0.0;
    uint8_t chargeStatus = notCharge;
    bool isCharging = false;
} BatteryState, *pBatteryState;

typedef struct _ledState{
    bool isOn;
    uint32_t lastToggleTime;
    uint32_t currentRate;
} ledState;

#endif