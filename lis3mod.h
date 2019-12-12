#ifndef __LIS3_MODULE_H
#define __LIS3_MODULE_H

#include "main.h"

#define DEVICEADDRESS	0x30
#define SELFADDRESS	0x19
#define I2CSPEED	100000

#define I2C_TIMEOUT	((uint32_t)(10000))
#define I2C_ERR   0  
#define I2C_CRR   1  
#define  I2CDataIn  1  
#define  I2CDataOut 0  

#define I2C_PORT  (GPIOB)  
#define I2CSCL   (GPIO_PIN_4)  
#define I2CSDA   (GPIO_PIN_5)  

#define SDA_H GPIO_WriteHigh(I2C_PORT, I2CSDA)
#define SDA_L GPIO_WriteLow(I2C_PORT, I2CSDA)
#define SCL_H GPIO_WriteHigh(I2C_PORT, I2CSCL)
#define SCL_L GPIO_WriteLow(I2C_PORT, I2CSCL)
#define SDA_read GPIO_ReadInputPin(I2C_PORT,I2CSDA)

enum XYZOUTREG{
	OUT_BASEREG = 0x28,
	OUT_X_L_REG = 0x28,
	OUT_X_H_REG,
	OUT_Y_L_REG,
	OUT_Y_H_REG,
	OUT_Z_L_REG,
	OUT_Z_H_REG
};

enum XYZCTRLREG{
	CTRL_BASEREG = 0x20,
	CTRL_REG1 = 0x20,
	CTRL_REG2,
	CTRL_REG3,
	CTRL_REG4,
	CTRL_REG5,
	CTRL_REG6
};

enum XYZNUM{
	OUT_X_L = 0,
	OUT_X_H,
	OUT_Y_L,
	OUT_Y_H,
	OUT_Z_L,
	OUT_Z_H,
	OUT_COUNT
};

static enum LIS3DATARATE{
	LIS3_POWER_OFF = 0,
	LIS3_POWER_1HZ = 0x10,
	LIS3_POWER_10HZ = 0x20,	
	LIS3_POWER_25HZ = 0x30,
	LIS3_POWER_50HZ = 0x40,
	LIS3_POWER_100HZ = 0x50,
	LIS3_POWER_200HZ = 0x60,
	LIS3_POWER_400HZ = 0x70,
	LIS3_POWER_1600HZ = 0x80
};
static enum LIS3POWERMODE{
	LIS3_LOW_POWER = 0x08,
	LIS3_NORAML_POWER = 0x00
};
static enum LIS3OUTSEL{
	OUT_SOLO_X = 0x01,
	OUT_SOLO_Y = 0x02,
	OUT_SOLO_Z = 0x04,
	OUT_BOTH_XYZ = 0x07
};

void I2C_delay(void);
unsigned char I2C_Start(void);
void Lis3On(void);	
void Lis3Off(void);
void I2C_Stop(void);
void I2C_Ack(void);
void I2C_NoAck(void);

unsigned char I2C_WaitAck(void);

void I2C_SendByte(unsigned char SendByte);
unsigned char I2C_ReadByte(uint8_t ack);
uint8_t Write_Byte(uint8_t DevAddr,uint8_t reg,uint8_t data);
uint8_t Read_Byte(uint8_t DevAddr,uint8_t reg);
uint8_t Mult_Write(uint8_t addr,uint8_t reg,uint8_t len,uint8_t *buf);
uint8_t Mult_Read(uint8_t addr,uint8_t reg,uint8_t len,uint8_t *buf);



/* bad code
void XyzInit(void);
int XyzUpdate(void);
int XyzPower(void);
//INTERRUPT void ADC1_IRQHandler(void); /* ADC1 */

#endif