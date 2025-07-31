#include "MYSPI.h"
#include "LCD.h"
#include "delay.h"
#include "logo.h"

#define XDP 126
#define YDP 294

uint8_t chunk_buffer[LCD_CHUNK_BUFFER_SIZE] = {0};

void LCD_WR_DATA8(const uint8_t data)
{
    LCD_RS_SET;
    spi_write_byte(data);
}

void LCD_WR_DATA(const uint16_t data)
{
    LCD_WR_DATA8(data >> 8);
    LCD_WR_DATA8(data);
}

void LCD_WR_REG(const uint8_t data)
{
    LCD_RS_CLR;
    spi_write_byte(data);
}

void LCD_Control_Init(void)
{
    stc_port_init_t stcPortInit;

    /* configuration structure initialization */
    MEM_ZERO_STRUCT(stcPortInit);
    stcPortInit.enPinMode = Pin_Mode_Out;

    PORT_Init(LCD_PORT, LCDPin_RS, &stcPortInit);
    PORT_Init(LCD_PORT, LCDPin_RST, &stcPortInit);
    PORT_Init(LCD_PORT, LCDPin_CS, &stcPortInit);

    LCD_CS_CLR;
}

void LCD_Simple_init(void)
{
    delay_ms(50); // DDL_DelayMS(50);
    LCD_RST_SET;
    delay_ms(50); // DDL_DelayMS(50);
    LCD_RST_CLR;
    delay_ms(50); // DDL_DelayMS(50);
    LCD_RST_SET;
    delay_ms(150); // DDL_DelayMS(150);

    LCD_WR_REG(0xfe);
    LCD_WR_DATA8(0x01);
    LCD_WR_REG(0x6A);
    LCD_WR_DATA8(0x21);
    LCD_WR_REG(0xfe);
    LCD_WR_DATA8(0x00); // CMD1 command

    LCD_WR_REG(0xC4);   // NEW ADD
    LCD_WR_DATA8(0x80); // CMD1 command //NEW ADD

    LCD_WR_REG(0x35);
    LCD_WR_DATA8(0x00);

    LCD_WR_REG(0x51);
    LCD_WR_DATA8(0xff);

    LCD_WR_REG(0x3A);
    LCD_WR_DATA8(0x05); // Ĭ��888��ʽ����ƽ̨�����ݸ�ʽ���������888��ʽ����Ҫ��3Ah

    LCD_WR_REG(0x11);
    delay_ms(150);

    LCD_WR_REG(0x29);
}

void LCD_SPI_SetDisplayWindow(u16 add_sx, u16 add_ex, u16 add_sy, u16 add_ey)
{
    u16 sx = add_sx;
    u16 ex = add_ex;
    u16 sy = add_sy;
    u16 ey = add_ey;

    LCD_WR_REG(0x2A); // Column Address Setting
    LCD_WR_DATA8(sx >> 8);
    LCD_WR_DATA8(sx);
    LCD_WR_DATA8(ex >> 8);
    LCD_WR_DATA8(ex); //

    LCD_WR_REG(0x2B); // Row Address Setting
    LCD_WR_DATA8(sy >> 8);
    LCD_WR_DATA8(sy);
    LCD_WR_DATA8(ey >> 8);
    LCD_WR_DATA8(ey); //
    LCD_WR_REG(0x2c);
}

void LCD_Fill(uint16_t x_start, uint16_t y_start, uint16_t x_end, uint16_t y_end, uint16_t color)
{
    uint32_t size = 0, size_remain = 0;

    size = (x_end - x_start + 1) * (y_end - y_start + 1) * 2;

    if (size > LCD_CHUNK_BUFFER_SIZE)
    {
        size_remain = size - LCD_CHUNK_BUFFER_SIZE;
        size = LCD_CHUNK_BUFFER_SIZE;
    }

    LCD_SPI_SetDisplayWindow(x_start, x_end, y_start, y_end);

    while (true)
    {
        for (uint32_t i = 0; i < size / 2; i++)
        {
            chunk_buffer[2 * i] = color >> 8;
            chunk_buffer[2 * i + 1] = color;
        }

        spi_dma_push(chunk_buffer, size);

        if (size_remain == 0)
            break;

        if (size_remain > LCD_CHUNK_BUFFER_SIZE)
        {
            size_remain = size_remain - LCD_CHUNK_BUFFER_SIZE;
        }

        else
        {
            size = size_remain;
            size_remain = 0;
        }
    }
}

void LCD_Clear(uint16_t color)
{
    LCD_Fill(0, 0, XDP - 1, YDP - 1, color);
}

void show_logo()
{

    LCD_SPI_SetDisplayWindow(0, XDP - 1, 0, YDP - 1);

    spi_dma_push(gImage_1, 65535);
    spi_dma_push(gImage_1 + 65535, sizeof(gImage_1) - 65535);

}

uint16_t LCD_DrawPoint(const u32 point)
{
    const u8 rr = (point >> 16);
    const u8 gg = (point >> 8);
    const u8 bb = point;
    const u16 color = ((rr & 0xf8) << 8) + ((gg & 0xe0) << 3);
    const u16 color1 = ((bb & 0xf8) >> 3) + ((gg & 0x1c) << 3);
    return color + color1;
}
