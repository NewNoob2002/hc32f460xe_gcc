#ifndef __MYSPI_H__
#define __MYSPI_H__

#include <stdint.h>

/* SPI_SCK Port/Pin definition */ // SCL
/* SCK = PA8 */
#define SPI_SCK_PORT (PortB)
#define SPI_SCK_PIN (Pin15)
#define SPI_SCK_FUNC (Func_Spi4_Sck)

/* SPI_NSS Port/Pin definition */
#define SPI_NSS_PORT (PortB)
#define SPI_NSS_PIN (Pin14)
#define SPI_NSS_FUNC (Func_Spi4_Nss0)

/* SPI_MOSI Port/Pin definition */ // SDI
/* MOSI = PB0 */                   /* MOSI = PB0 */
#define SPI_MOSI_PORT (PortB)
#define SPI_MOSI_PIN (Pin13)
#define SPI_MOSI_FUNC (Func_Spi4_Mosi)

/* SPI_MISO Port/Pin definition */
#define SPI_MISO_PORT (PortB)
#define SPI_MISO_PIN (Pin12)
#define SPI_MISO_FUNC (Func_Spi4_Miso)

/* SPI unit and clock definition */
#define SPI_UNIT (M4_SPI4)
#define SPI_UNIT_CLOCK (PWC_FCG1_PERIPH_SPI4)

/* SPI DMA unit and channel definition */
#define SPI_DMA_UNIT (M4_DMA1)
#define SPI_DMA_CLOCK_UNIT (PWC_FCG0_PERIPH_DMA1)

#define SPI_DMA_TX_CHANNEL (DmaCh1)

#define SPI_DMA_TX_TRIG_SOURCE (EVT_SPI4_SPTI)

#ifdef __cplusplus
extern "C"
{
#endif
    void Spi_Config(void);
    void Spi_DmaConfig(void);
    void spi_write_byte(uint8_t byte);
    void tran(uint16_t num);
    void spi_dma_push(const void* data, uint32_t size);

#ifdef __cplusplus
}
#endif

#endif
