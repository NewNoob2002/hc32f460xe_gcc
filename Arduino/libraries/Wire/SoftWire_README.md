# SoftWire - 软件I2C库

基于HC32F460的Arduino架构的软件I2C库，提供完整的I2C通信功能，支持8位和16位数据读写。

## 特性

- ✅ 完全的软件I2C实现，不依赖硬件I2C外设
- ✅ 支持8位和16位数据读写
- ✅ 支持大端序(BE)和小端序(LE)16位数据格式
- ✅ 兼容Arduino Wire库接口
- ✅ 支持时钟拉伸处理
- ✅ 内置设备检测和总线扫描功能
- ✅ 详细的错误状态报告
- ✅ 支持10kHz-400kHz时钟频率

## 快速开始

### 1. 基础使用

```cpp
#include "soft_i2c.h"

// 创建软件I2C对象
SoftWire softWire(PA8, PA9);  // SDA引脚, SCL引脚

void setup() {
    Serial.begin(115200);
    
    // 初始化软件I2C
    if (softWire.begin()) {
        Serial.println("SoftWire initialized!");
    }
    
    // 设置时钟频率为100kHz
    softWire.setClock(100000);
}
```

### 2. 8位数据读写

```cpp
uint8_t device_addr = 0x48;  // 设备地址
uint8_t reg_addr = 0x01;     // 寄存器地址

// 写8位数据
uint8_t write_data = 0xAB;
SoftI2CStatus status = softWire.writeRegister8(device_addr, reg_addr, write_data);

// 读8位数据
uint8_t read_data;
status = softWire.readRegister8(device_addr, reg_addr, &read_data);

if (status == SOFT_I2C_SUCCESS) {
    Serial.printf("Read data: 0x%02X\n", read_data);
}
```

### 3. 16位数据读写

```cpp
// 写16位数据（大端序）
uint16_t write_data = 0x1234;
status = softWire.writeRegister16BE(device_addr, reg_addr, write_data);

// 读16位数据（大端序）
uint16_t read_data;
status = softWire.readRegister16BE(device_addr, reg_addr, &read_data);

// 小端序操作
status = softWire.writeRegister16LE(device_addr, reg_addr, write_data);
status = softWire.readRegister16LE(device_addr, reg_addr, &read_data);
```

### 4. 多字节数据操作

```cpp
// 写多个字节
uint8_t write_buffer[] = {0x12, 0x34, 0x56, 0x78};
status = softWire.writeBytes(device_addr, reg_addr, write_buffer, 4);

// 读多个字节
uint8_t read_buffer[4];
status = softWire.readBytes(device_addr, reg_addr, read_buffer, 4);
```

### 5. Wire兼容接口

```cpp
// 使用Wire风格的API
softWire.beginTransmission(device_addr);
softWire.write(reg_addr);
softWire.write(data1);
softWire.write(data2);
uint8_t result = softWire.endTransmission();

// 读取数据
uint8_t bytes_received = softWire.requestFrom(device_addr, 2);
while (softWire.available()) {
    uint8_t data = softWire.read();
    Serial.printf("0x%02X ", data);
}
```

## API参考

### 构造函数

```cpp
SoftWire(gpio_pin_t sda_pin, gpio_pin_t scl_pin, uint32_t delay_us = 5)
```

### 基础函数

| 函数 | 说明 |
|------|------|
| `begin()` | 初始化软件I2C |
| `end()` | 结束软件I2C，释放引脚 |
| `setClock(frequency)` | 设置时钟频率(10kHz-400kHz) |
| `isDeviceOnline(address)` | 检测设备是否在线 |

### 8位数据操作

| 函数 | 说明 |
|------|------|
| `writeRegister8(addr, reg, data)` | 写8位寄存器 |
| `readRegister8(addr, reg, *data)` | 读8位寄存器 |
| `writeBytes(addr, reg, *data, len)` | 写多字节数据 |
| `readBytes(addr, reg, *data, len)` | 读多字节数据 |

### 16位数据操作

| 函数 | 说明 |
|------|------|
| `writeRegister16BE(addr, reg, data)` | 写16位寄存器(大端序) |
| `readRegister16BE(addr, reg, *data)` | 读16位寄存器(大端序) |
| `writeRegister16LE(addr, reg, data)` | 写16位寄存器(小端序) |
| `readRegister16LE(addr, reg, *data)` | 读16位寄存器(小端序) |

### Wire兼容接口

| 函数 | 说明 |
|------|------|
| `beginTransmission(address)` | 开始传输 |
| `write(data)` | 写数据到缓冲区 |
| `endTransmission(sendStop)` | 结束传输 |
| `requestFrom(addr, quantity)` | 请求数据 |
| `available()` | 检查可用数据 |
| `read()` | 读取一个字节 |

### 工具函数

| 函数 | 说明 |
|------|------|
| `getStatusString(status)` | 获取状态描述字符串 |
| `scanBus(*devices, max_count)` | 扫描总线上的设备 |

## 状态码

| 状态码 | 说明 |
|--------|------|
| `SOFT_I2C_SUCCESS` | 成功 |
| `SOFT_I2C_NACK_ADDR` | 地址NACK |
| `SOFT_I2C_NACK_DATA` | 数据NACK |
| `SOFT_I2C_TIMEOUT_ERROR` | 超时错误 |
| `SOFT_I2C_BUS_ERROR` | 总线错误 |

## 性能优化

经过优化的SoftWire库具有以下性能特点：

- **高速传输**: 支持高达200kHz的实际传输速度
- **智能GPIO控制**: 减少不必要的pinMode调用，提高效率
- **精确时序**: 考虑GPIO操作开销的时序计算
- **低延迟**: 优化的半延时控制，减少传输时间

### 性能测试

使用 `SoftWire_Performance_Test` 示例可以测试实际性能：

```cpp
// 运行性能测试
softWire.setClock(100000);  // 设置目标100kHz
// 查看实际达到的传输速度和效率
```

### 速度优化建议

1. **选择合适的频率**: 50-100kHz通常是速度和稳定性的最佳平衡点
2. **短线连接**: 减少I2C线缆长度可以支持更高频率
3. **适当的上拉电阻**: 4.7kΩ适合大多数应用，高速时可用2.2kΩ
4. **批量传输**: 使用 `writeBytes()` / `readBytes()` 比单字节操作更高效

## 使用注意事项

1. **引脚选择**: 确保选择的GPIO引脚支持输入上拉模式
2. **时钟频率**: 建议使用50-100kHz，高频时需要考虑线路负载
3. **上拉电阻**: I2C总线需要外部上拉电阻(通常4.7kΩ)
4. **错误处理**: 始终检查函数返回的状态码
5. **总线共享**: 可以与硬件I2C共存，使用不同引脚
6. **性能监控**: 使用性能测试示例验证实际传输速度

## 示例代码

- **基础使用**: `examples/SoftWire_Demo/SoftWire_Demo.ino`
- **性能测试**: `examples/SoftWire_Performance_Test/SoftWire_Performance_Test.ino`

## 兼容性

- ✅ HC32F460系列微控制器
- ✅ Arduino IDE开发环境
- ✅ 与现有Wire库代码兼容

## 技术支持

如有问题或建议，请查看示例代码或调试输出信息。 