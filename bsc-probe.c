#include<stdio.h>
#include<unistd.h>
#include<fcntl.h>
#include <stdint.h>
#include "bsc-slave.h"


#define SLV_ADDR  	 0x33
#define TX_BUF_SIZE      5

int main(void){

	char buffer[20];
	char tx_buffer[TX_BUF_SIZE];
	int fd;
	int length, tx_length = 2;
	int count, value_count, transfered;
	char *pointer;
	int output_end = 0;

	if((fd = open("/dev/i2c-slave", O_RDWR)) == -1){
		printf("could not open i2c-slave\n");
	}

	unsigned long reg;

	reg = ioctl(fd, I2C_SLAVE_BSC_RSR, 0);
	printf("RSR: %x\n", reg);

	if ((reg&BSC_RSR_UE) != 0)
		printf("TX Underrun Error;\n");
	if ((reg&BSC_RSR_OE) != 0)
		printf("RX Overrun Error;\n");

	reg = ioctl(fd, I2C_SLAVE_BSC_SLV, 0);
	printf("Slave Address: %d\n", reg);

	reg = ioctl(fd, I2C_SLAVE_BSC_CR, 0);
	printf("CR: %x\n", reg);

	if ((reg&BSC_CR_BRK) != 0)
		printf("Break Enabled (I2C TX functions disabled)\n");
	else
		printf("Break Disabled (I2C TX Fucntions enabled)\n");

	if ((reg&BSC_CR_RXE) != 0)
		printf("Receive Mode Enabled (Only affects SPI?)\n");
	else
		printf("Receive Mode Disabled (Only affects SPI?)\n");

	if ((reg&BSC_CR_TXE) != 0)
		printf("Transmit mode enabled\n");
	else
		printf("Transmit mode disabled\n");

	if ((reg&BSC_CR_I2C) != 0)
		printf("I2C Mode enabled\n");
	else
		printf("I2C mode disabled\n");

	if ((reg&BSC_CR_SPI) != 0)
		printf("SPI Mode enabled\n");
	else
		printf("SPI mode disabled\n");

	if ((reg&BSC_CR_EN) != 0)
		printf("Device Enabled\n");
	else
		printf("Device Disabled\n");

	reg = ioctl(fd, I2C_SLAVE_BSC_FR, 0);
	printf("FR: %x\n", reg);

	printf("RX FIFO Level: %d\n", (reg & 0xf800) / 2048);
	printf("TX FIFO Level: %d\n", (reg & 0x7c0) / 64);

	if ((reg&BSC_FR_TXFE) != 0)
		printf("TX Fifo Empty\n");
	if ((reg&BSC_FR_RXFF) != 0)
		printf("RX Fifo full\n");
	if ((reg&BSC_FR_TXFF) != 0)
		printf("TX Fifo full\n");
	if ((reg&BSC_FR_RXFE) != 0)
		printf("RX Fifo Empty\n");
	if ((reg&BSC_FR_TXBUSY) != 0)
		printf("Transmit operation in progress.\n");
	else
		printf("Transmit inactive.\n");

	reg = ioctl(fd, I2C_SLAVE_BSC_IFLS, 0);
	printf("IFLS: %x\n", reg);

	printf("RX FIFO Interrupt trigger: %d\n", (reg & 0x0038) / 8);
	printf("TX FIFO interrupt trigger: %d\n", (reg & 0x7));

	reg = ioctl(fd, I2C_SLAVE_BSC_IMSC, 0);
	printf("IMSC: %x\n", reg);

	reg = ioctl(fd, I2C_SLAVE_BSC_RIS, 0);
	printf("RIS: %x\n", reg);

	reg = ioctl(fd, I2C_SLAVE_BSC_MIS, 0);
	printf("MIS: %x\n", reg);

	reg = ioctl(fd, I2C_SLAVE_BSC_ICR, 0);
	printf("ICR: %x\n", reg);


	close(fd);
	return 0;
}
