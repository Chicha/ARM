// nRF24L01.h
#ifndef _NRF_24L01_
#define _NRF_24L01_

#include "Arduino.h"          

/* nRF24L01 Pin Map */
#define nRF24L01_SCK_PIN        13  //sD13
#define nRF24L01_CE_PIN         8   //sD8
#define nRF24L01_CSN_PIN        7   //sD7
#define nRF24L01_MOSI_PIN       11  //sD11
#define nRF24L01_MISO_PIN       12  //sD12
#define nRF24L01_IRQ_PIN        2   //sD2

#define nRF24L01_SCK_SET        digitalWrite(nRF24L01_SCK_PIN, 1)
#define nRF24L01_SCK_CLR        digitalWrite(nRF24L01_SCK_PIN, 0)

#define nRF24L01_CE_SET         digitalWrite(nRF24L01_CE_PIN, 1)
#define nRF24L01_CE_CLR         digitalWrite(nRF24L01_CE_PIN, 0)

#define nRF24L01_CSN_SET        digitalWrite(nRF24L01_CSN_PIN, 1)
#define nRF24L01_CSN_CLR        digitalWrite(nRF24L01_CSN_PIN, 0)

#define nRF24L01_MOSI_SET       digitalWrite(nRF24L01_MOSI_PIN, 1)
#define nRF24L01_MOSI_CLR       digitalWrite(nRF24L01_MOSI_PIN, 0)

#define nRF24L01_MISO_ReadDat   digitalRead(nRF24L01_MISO_PIN)
#define nRF24L01_IRQ_ReadDat    digitalRead(nRF24L01_IRQ_PIN)


#define TX_ADR_WIDTH    5     // 5 bytes TX address width
#define RX_ADR_WIDTH    5     // 5 bytes RX address width
#define TX_PLOAD_WIDTH  32    // 32 bytes TX payload
#define RX_PLOAD_WIDTH  32    // 32 bytes TX payload


/* nRF24L01 Instruction Mnemonics */
#define READ_REG        0x00  // Define read command to register
#define WRITE_REG       0x20  // Define write command to register
#define RD_RX_PLOAD     0x61  // Define RX payload register address
#define WR_TX_PLOAD     0xA0  // Define TX payload register address
#define FLUSH_TX        0xE1  // Define flush TX register command
#define FLUSH_RX        0xE2  // Define flush RX register command
#define REUSE_TX_PL     0xE3  // Define reuse TX payload register command
#define NOP             0xFF  // Define No Operation, might be used to read status register


/* nRF24L01 Memory Map */
#define CONFIG          0x00  // 'Config' register address
#define EN_AA           0x01  // 'Enable Auto Acknowledgment' register address
#define EN_RXADDR       0x02  // 'Enabled RX addresses' register address
#define SETUP_AW        0x03  // 'Setup address width' register address
#define SETUP_RETR      0x04  // 'Setup Auto. Retrans' register address
#define RF_CH           0x05  // 'RF channel' register address
#define RF_SETUP        0x06  // 'RF setup' register address
#define STATUS          0x07  // 'Status' register address
#define OBSERVE_TX      0x08  // 'Observe TX' register address
#define CD              0x09  // 'Carrier Detect' register address
#define RX_ADDR_P0      0x0A  // 'RX address pipe0' register address
#define RX_ADDR_P1      0x0B  // 'RX address pipe1' register address
#define RX_ADDR_P2      0x0C  // 'RX address pipe2' register address
#define RX_ADDR_P3      0x0D  // 'RX address pipe3' register address
#define RX_ADDR_P4      0x0E  // 'RX address pipe4' register address
#define RX_ADDR_P5      0x0F  // 'RX address pipe5' register address
#define TX_ADDR         0x10  // 'TX address' register address
#define RX_PW_P0        0x11  // 'RX payload width, pipe0' register address
#define RX_PW_P1        0x12  // 'RX payload width, pipe1' register address
#define RX_PW_P2        0x13  // 'RX payload width, pipe2' register address
#define RX_PW_P3        0x14  // 'RX payload width, pipe3' register address
#define RX_PW_P4        0x15  // 'RX payload width, pipe4' register address
#define RX_PW_P5        0x16  // 'RX payload width, pipe5' register address
#define FIFO_STATUS     0x17  // 'FIFO Status Register' register address
#define DYNPD	        0x1C
#define FEATURE	        0x1D

//***************************************************************//
//                   FUNCTION's PROTOTYPES                       //
/****************************************************************/
unsigned char SPI_RW(unsigned char byte);                                // Single SPI read/write
unsigned char SPI_Read(unsigned char reg);                               // Read one byte from nRF24L01
unsigned char SPI_RW_Reg(unsigned char reg, unsigned char byte);                  // Write one byte to register 'reg'
unsigned char SPI_Write_Buf(unsigned char reg, unsigned char *pBuf, unsigned char bytes);  // Writes multiply bytes to one register
unsigned char SPI_Read_Buf(unsigned char reg, unsigned char *pBuf, unsigned char bytes);   // Read multiply bytes from one register
//*****************************************************************/


void nRF24L01_Init(void) ;
void SetRX_Mode(void);
unsigned char nRF24L01_RxPacket(unsigned char* rx_buf);
void nRF24L01_TxPacket(unsigned char * tx_buf);
unsigned char nRF24L01_debug(void);

extern unsigned char TX_ADDRESS[TX_ADR_WIDTH];//TX address
extern unsigned char RX_ADDRESS[RX_ADR_WIDTH];//RX address


#endif   //_NRF_24L01_
