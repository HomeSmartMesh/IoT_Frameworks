/** @file nRF_SPI.h
 *
 * @author Wassim FILALI taken over from  Runar Kjellhaug out of nRF24L01P-EK
 *
 * @compiler IAR STM8
 *
 *
 * $Date: 20.09.2015
 * $Revision:
 *
 */

//for BYTE
#include "commonTypes.h"


// Macro to read SPI Interrupt flag
#define WAIT_SPIF (!(SPI0CN & 0x80))  // SPI interrupt flag(µC platform dependent)

// Define nRF24L01 interrupt flag's
#define IDLE            0x00  // Idle, no interrupt pending
#define MAX_RT          0x10  // Max #of TX retrans interrupt
#define TX_DS           0x20  // TX data sent interrupt
#define RX_DR           0x40  // RX data received

#define SPI_CFG         0x40  // SPI Configuration register value
#define SPI_CTR         0x01  // SPI Control register values
#define SPI_CLK         0x00  // SYSCLK/2*(SPI_CLK+1) == > 12MHz / 2 = 6MHz
#define SPI0E           0x02  // SPI Enable in XBR0 register

//********************************************************************************************************************//
// SPI(nRF24L01) commands
//These SPI addresses are not registers addresses, they are quick access functionnalities
#define READ_REG        0x00  // Define read command to register
#define WRITE_REG       0x20  // Define write command to register
#define RD_RX_PLOAD     0x61  // Define RX payload register address
#define WR_TX_PLOAD     0xA0  // Define TX payload register address
#define FLUSH_TX        0xE1  // Define flush TX register command
#define FLUSH_RX        0xE2  // Define flush RX register command
#define REUSE_TX_PL     0xE3  // Define reuse TX payload register command
#define NOP             0xFF  // Define No Operation, might be used to read status register

//********************************************************************************************************************//
// SPI(nRF24L01) registers(addresses)
//These addresse have to be combined with the READ_REG or WRITE_REG
// (READ_REG | CONFIG) which is equal to (CONFIG) as READ_REG is 0
// (WRITE_REG | CONFIG) to write to the register
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
#define DYNPD		    0x1C  // Enable Dynamic Payload Length
#define FEATURE		    0x1D  // Feature Register
//--------------registers bits definition
//CONFIG bits
#define bit_Mask_Reserved	0x7F
#define	bit_PWR_UP			0x02
#define	bit_neg_PWR_UP		(~0x02)
#define	bit_PRIM_RX			0x01
#define	bit_neg_PRIM_RX		(~0x01)
#define bit_MASK_RX_DR		0x40
#define bit_MASK_TX_DS		0x20
#define bit_MASK_MAX_RT		0x10
#define bit_neg_EN_CRC		(~0x08)
//STATUS bits	
#define bit_RX_DR			0x40
#define bit_TX_DS			0x20
#define bit_MAX_RT			0x10
#define bit_Mask_RXPNO		0x0E
#define bit_RXPNO_NotUsed	0x0C
#define bit_RXPNO_RxFFEmpty	0x0E
#define bit_TX_FULL			0x01
	
//FIFO_STATUS bits	
#define	bit_TX_EMPTY		0x10

extern const unsigned char nRF24L01pRegNumber;

extern const char* nRF24L01pRegNames[];

//********************************************************************************************************************//
//                                        FUNCTION's PROTOTYPES                                                       //
//********************************************************************************************************************//

void SPI_Init();                                       // Init HW SPI
BYTE SPI_RW(BYTE byte);                              // Single SPI read/write

//SPI commands
BYTE SPI_Read_Register(BYTE reg);                      // Uses the command READ_REG
BYTE SPI_Write_Register(BYTE reg, BYTE value);         // Uses the command WRITE_REG

BYTE SPI_Write_Buf(BYTE reg, BYTE *pBuf, BYTE bytes);  // Writes multiply bytes to one register
BYTE SPI_Read_Buf(BYTE reg, BYTE *pBuf, BYTE bytes);   // Read multiply bytes from one register

BYTE SPI_Command(BYTE reg, BYTE value);					//generic command out of nRF24L01P Page 51 revision 1.0

//********************************************************************************************************************//
