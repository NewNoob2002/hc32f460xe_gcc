#include <Arduino.h>
#include "init/init.h"
#include "MillisTaskManager.h"
#include "soft_i2c.h"

static MillisTaskManager taskManager;

// 定义I2C引脚 - 根据你的硬件连接修改
#define SOFT_I2C_SDA_PIN    PA8   // SDA引脚
#define SOFT_I2C_SCL_PIN    PA9   // SCL引脚

// 创建软件I2C对象
SoftWire softWire(SOFT_I2C_SDA_PIN, SOFT_I2C_SCL_PIN);

void loopTask()
{
    digitalToggle(PC13);
    softWire.beginTransmission(0x0B);
    softWire.write(0x16);
    softWire.endTransmission();
    softWire.requestFrom(0x0B, 1, true);
    uint8_t data = softWire.read();
    Serial.printf("data: 0x%02X\n", data);
}

void demonstrateBusScan() {
    Serial.println("\n=== I2C Bus Scan Demo ===");
    
    uint8_t found_devices[16];
    uint8_t device_count = softWire.scanBus(found_devices, 16);
    
    Serial.printf("Found %d I2C devices:\n", device_count);
    for (uint8_t i = 0; i < device_count; i++) {
        Serial.printf("  Device %d: 0x%02X\n", i + 1, found_devices[i]);
    }
}

int main()
{
    core_init();

    if (softWire.begin()) {
        Serial.println("SoftWire initialized successfully!");
    } else {
        Serial.println("SoftWire initialization failed!");
        return 0;
    }
    
    // 设置I2C时钟频率为100kHz
    softWire.setClock(100000);
    Serial.println("I2C clock set to 100kHz");

    demonstrateBusScan();

    pinMode(PC13, OUTPUT);
    taskManager.Register(loopTask, 1000);

    while (true) {
        taskManager.Running(millis());
    }
    return 0;
}