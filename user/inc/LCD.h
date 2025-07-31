#ifndef LCD_H
#define LCD_H
/* Includes ------------------------------------------------------------------*/
#include <cstdint>

/* LCD color */
#define rgb565(r, g, b) (((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3))

#define RED    rgb565(255,   0,   0) // 0xf800
#define GREEN  rgb565(  0, 255,   0) // 0x07e0
#define BLUE   rgb565(  0,   0, 255) // 0x001f
#define BLACK  rgb565(  0,   0,   0) // 0x0000
#define WHITE  rgb565(255, 255, 255) // 0xffff
#define GRAY   rgb565(128, 128, 128) // 0x8410
#define YELLOW rgb565(255, 255,   0) // 0xFFE0
#define CYAN   rgb565(  0, 156, 209) // 0x04FA
#define PURPLE rgb565(128,   0, 128) // 0x8010
/* Exported types ------------------------------------------------------------*/
typedef unsigned long u32;
typedef unsigned short u16;
typedef volatile unsigned long vu32;
typedef volatile unsigned short vu16;
typedef unsigned char u8;
typedef unsigned short int uint16_t;

#define LCD_PORT PortB
#define LCDPin_RS Pin01
#define LCDPin_RST Pin02
#define LCDPin_CS Pin14

#define LCD_RST_CLR PORT_ResetBits(LCD_PORT, LCDPin_RST)
#define LCD_RST_SET PORT_SetBits(LCD_PORT, LCDPin_RST)
#define LCD_RS_CLR PORT_ResetBits(LCD_PORT, LCDPin_RS)
#define LCD_RS_SET PORT_SetBits(LCD_PORT, LCDPin_RS)
#define LCD_CS_CLR PORT_ResetBits(LCD_PORT, LCDPin_CS)
#define LCD_CS_SET PORT_SetBits(LCD_PORT, LCDPin_CS)

#define LCD_CHUNK_BUFFER_SIZE (126*294/2)
extern uint8_t chunk_buffer[LCD_CHUNK_BUFFER_SIZE];

#ifdef __cplusplus
extern "C"
{
#endif
    void LCD_Control_Init(void);
    void LCD_Simple_init(void);
    void LCD_SPI_SetDisplayWindow(u16 add_sx, u16 add_ex, u16 add_sy, u16 add_ey);
    void LCD_SPI_WriteRAM_Prepare(void);

    void show_logo();
    void LCD_WR_DATA(uint16_t data);
    uint16_t LCD_DrawPoint(u32 point);
    void LCD_Fill(uint16_t x_start, uint16_t y_start, uint16_t x_end, uint16_t y_end, uint16_t color);
    void LCD_Clear(uint16_t color);

#ifdef __cplusplus
}
#endif

#endif /* LCD_H */
