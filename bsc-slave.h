#define I2C_SLAVE				0x0703  /* Use this slave address */
#define I2C_SLAVE_FORCE			0x0706  /* Use this slave address, even if it*/
#define I2C_SLAVE_BSC_RSR 		0x0707
#define I2C_SLAVE_BSC_SLV		0x0708
#define I2C_SLAVE_BSC_CR		0x0709
#define I2C_SLAVE_BSC_FR	   	0x070a
#define I2C_SLAVE_BSC_IFLS		0x070b
#define I2C_SLAVE_BSC_IMSC		0x070c
#define I2C_SLAVE_BSC_RIS		0x070d
#define I2C_SLAVE_BSC_MIS		0x070e
#define I2C_SLAVE_BSC_ICR		0x070f
#define I2C_SLAVE_CLEAR_FIFOS	0x0710

/* BSC SLAVE register offsets */
#define BSC_DR			0x00
#define BSC_RSR 		0x04
#define BSC_SLV			0x08
#define BSC_CR  		0x0c
#define BSC_FR	        	0x10
#define BSC_IFLS		0x14
#define BSC_IMSC		0x18
#define BSC_RIS			0x1c
#define BSC_MIS			0x20
#define BSC_ICR			0x24
#define BSC_DMACR		0x28
#define BSC_TDR                 0x2c
#define BSC_GPUSTAT		0x30
#define BSC_HCTRL		0x34
#define BSC_DEBUG		0x3c

/* Bitfields in DR */
#define BSC_DR_RXFLEVEL_MASK	0xF8000000
#define BSC_DR_TXFLEVEL_MASK	0x07c00000
#define BSC_DR_RXBUSY		0x00200000
#define BSC_DR_TXFE		0x00100000
#define BSC_DR_RXFF		0x00080000
#define BSC_DR_TXFF		0x00040000
#define BSC_DR_RXFE		0x00020000
#define BSC_DR_TXBUSY		0x00010000
#define BSC_DR_DATA_MASK	0x000000FF

/*Bitfields in RSR*/
#define BSC_RSR_UE 0x00000002
#define BSC_RSR_OE 0x00000001

/* Bitfields in CR */
#define BSC_CR_INV_TXF		0x00002000
#define BSC_CR_TESTFIFO		0x00000800
#define BSC_CR_RXE          0x00000200
#define BSC_CR_TXE          0x00000100
#define BSC_CR_BRK          0x00000080
#define BSC_CR_CPOL         0x00000010
#define BSC_CR_CPHA         0x00000008
#define BSC_CR_I2C          0x00000004
#define BSC_CR_SPI	        0x00000002
#define BSC_CR_EN	        0x00000001

/* Bitfields in IMSC */
#define BSC_IMSC_TXIM           0x00000002
#define BSC_IMSC_RXIM		0x00000001

/* Bitfields in MIS */
#define BSC_MIS_TXMIS           0x00000002
#define BSC_MIS_RXMIS		0x00000001

/* Bitfields in ICR */
#define BSC_ICR_TXIC		0x00000002

/*Bitfields in IFLS*/
#define BSC_IFLS_ONE_EIGHTS	0x00000000
#define BSC_IFLS_ONE_QUART      0x00000024
#define BSC_IFLS_ONE_HALF       0x00000012
#define BSC_IFLS_TX2_RX4BYTE    0x00000008

/* Bitfields in FR */
#define BSC_FR_TXFE				0x00000010
#define BSC_FR_RXFF				0x00000008
#define BSC_FR_TXFF				0x00000004
#define BSC_FR_RXFE             0x00000002
#define BSC_FR_TXBUSY           0x00000001