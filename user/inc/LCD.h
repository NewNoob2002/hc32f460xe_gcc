#ifndef __LCD_H
#define __LCD_H
/* Includes ------------------------------------------------------------------*/
#include <stdint.h>

/* LCD color */
#define Black 0x0000   /* 黑色：    0,   0,   0 */
#define BLUE 0x001F    /* 蓝色：    0,   0, 255 */
#define GREEN 0x07E0   /* 绿色：    0, 255,   0 */
#define CYAN 0x07FF    /* 青色：    0, 255, 255 */
#define RED 0xF800     /* 红色：  255,   0,   0 */
#define MAGENTA 0xF81F /* 品红：  255,   0, 255 */
#define YELLOW 0xFFE0  /* 黄色：  255, 255, 0   */
#define White 0xFFFF   /* 白色：  255, 255, 255 */
#define NAVY 0x000F    /* 深蓝色：  0,   0, 128 */
#define DGREEN 0x03E0  /* 深绿色：  0, 128,   0 */
#define DCYAN 0x03EF   /* 深青色：  0, 128, 128 */
#define MAROON 0x7800  /* 深红色：128,   0,   0 */
#define PURPLE 0x780F  /* 紫色：  128,   0, 128 */
#define OLIVE 0x7BE0   /* 橄榄绿：128, 128,   0 */
#define LGRAY 0xC618   /* 灰白色：192, 192, 192 */
#define DGRAY 0x7BEF   /* 深灰色：128, 128, 128 */
/* Exported types ------------------------------------------------------------*/
typedef unsigned long u32;
typedef unsigned short u16;
typedef volatile unsigned long vu32;
typedef volatile unsigned short vu16;
typedef unsigned char u8;
typedef unsigned short int uint16_t;

extern const unsigned char gImage_1[74088];

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
    void LCD_Simple_inition(void);
    void LCD_SPI_SetDisplayWindow(u16 add_sx, u16 add_ex, u16 add_sy, u16 add_ey);
    void LCD_SPI_WriteRAM_Prepare(void);

    void showpic_a();
    void LCD_WR_DATA(uint16_t data);
    uint16_t LCD_DrawPoint(u32 point);
    void LCD_Fill(uint16_t x_start, uint16_t y_start, uint16_t x_end, uint16_t y_end, uint16_t color);
    void LCD_Clear(uint16_t color);

#ifdef __cplusplus
}
#endif

#endif /* __LCD_H */
