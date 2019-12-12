#include "lis3mod.h"


static enum XYZNUM outcount;
static uint8_t outflag;
volatile uint8_t xyzdata[OUT_COUNT];

	
void Lis3On(void){
	GPIO_Init(I2C_PORT,I2CSCL,GPIO_MODE_OUT_OD_LOW_FAST);;
	GPIO_Init(I2C_PORT,I2CSDA,GPIO_MODE_OUT_OD_LOW_FAST);
	GPIO_WriteHigh(I2C_PORT, I2CSCL); 
	GPIO_WriteHigh(I2C_PORT, I2CSDA); 
	Write_Byte(DEVICEADDRESS,CTRL_REG1,LIS3_LOW_POWER|LIS3_POWER_10HZ|OUT_BOTH_XYZ);
	I2C_delay();
}
	
void Lis3Off(void){
	Write_Byte(DEVICEADDRESS,CTRL_REG1,LIS3_POWER_OFF);
	I2C_delay();I2C_delay();
	GPIO_Init(I2C_PORT,I2CSCL,GPIO_MODE_OUT_OD_LOW_FAST);;
	GPIO_Init(I2C_PORT,I2CSDA,GPIO_MODE_OUT_OD_LOW_FAST);
	GPIO_WriteHigh(I2C_PORT, I2CSCL); 
	GPIO_WriteHigh(I2C_PORT, I2CSDA); 
	
}

	
void I2C_delay(void)
{	
   unsigned char i=1; //这里可以优化速度	，经测试最低到5还能写入
   while(i) 
   { 
     i--; 
   } 
}



unsigned char I2C_Start(void)
{
	SDA_H;
	SCL_H;
	I2C_delay();
	if(!SDA_read)return 0;	//SDA线为低电平则总线忙,退出
	SDA_L;
	I2C_delay();
	if(SDA_read) return 0;	//SDA线为高电平则总线出错,退出
	SDA_L;
	I2C_delay();
	return 1;
}

void I2C_Stop(void)
{
	SCL_L;
	I2C_delay();
	SDA_L;
	I2C_delay();
	SCL_H;
	I2C_delay();
	SDA_H;
	I2C_delay();
}

void I2C_Ack(void)
{	
	SCL_L;
	I2C_delay();
	SDA_L;
	I2C_delay();
	SCL_H;
	I2C_delay();
	SCL_L;
	I2C_delay();
}

void I2C_NoAck(void)
{	
	SCL_L;
	I2C_delay();
	SDA_H;
	I2C_delay();
	SCL_H;
	I2C_delay();
	SCL_L;
	I2C_delay();
}

unsigned char I2C_WaitAck(void) 	 //返回为:=0有ACK,=1无ACK
{
	SCL_L;
	I2C_delay();
	SDA_H;			
	I2C_delay();
	SCL_H;
	I2C_delay();	
	if(SDA_read)
	{
      SCL_L;
      return 1;
	}
	
	SCL_L;
	return 0;
}

void I2C_SendByte(unsigned char SendByte) //数据从高位到低位//
{
    unsigned char i=8;
    while(i--)
    {
			SCL_L;
			I2C_delay();
			if(SendByte&0x80)
				SDA_H;
			else
				SDA_L;
			SendByte<<=1;
			I2C_delay();
			SCL_H;
			I2C_delay();
    }
    SCL_L;
}

unsigned char I2C_ReadByte(uint8_t ack)
{
	unsigned char i=8;
	unsigned char ReceiveByte=0;

	SDA_H;
	while(i--)
	{
		ReceiveByte<<=1;
		SCL_L;
		I2C_delay();
		SCL_H;
		I2C_delay();
		if(SDA_read)
		{
			ReceiveByte|=0x01;
		}
	}
	if(!ack) I2C_NoAck();
	else I2C_Ack();
	SCL_L;
	return ReceiveByte;
}

uint8_t Write_Byte(uint8_t DevAddr,uint8_t reg,uint8_t data)
{
	I2C_Start();
	I2C_SendByte((DevAddr)|0);//发送器件地址+写命令
	if(I2C_WaitAck())	//等待应答
	{
		I2C_Stop();	
		return 1;		
	}
	I2C_SendByte(reg);	//写寄存器地址
	I2C_WaitAck();		//等待应答
	I2C_SendByte(data);//发送数据
	if(I2C_WaitAck())	//等待ACK
	{
		I2C_Stop();	 
		return 1;		 
	}		 
    I2C_Stop();	 
	return 0;
}

uint8_t Read_Byte(uint8_t DevAddr,uint8_t reg)
{

	uint8_t res;
	I2C_Start();
	I2C_SendByte((DevAddr)|0);//发送器件地址+写命令
	if(I2C_WaitAck())//等待应答
	{
		I2C_Stop();
		return 0;
	}
	I2C_SendByte(reg);//写寄存器地址
	if(I2C_WaitAck())//等待应答
	{
		I2C_Stop();
		return 0;
	}
	I2C_Start();
	I2C_SendByte((DevAddr)|1);//发送期间地址+读命令
	if(I2C_WaitAck())//等待应答
	{
		I2C_Stop();
		return 0;
	}
	res=I2C_ReadByte(0);//读取数据，发送nACK
	I2C_Stop();//产生一个停止条件
	return res;
}  

//I2C连续写
//addr:器件地址
//reg: 寄存器地址
//len: 写入长度
//buf: 数据区
//返回值: 0,正常
//  			其他，错误代码
uint8_t Mult_Write(uint8_t addr,uint8_t reg,uint8_t len,uint8_t *buf)
{
	uint8_t i;
	I2C_Start();
	I2C_SendByte((addr<<1)|0);//发送器件地址+写命令
	if(I2C_WaitAck())//等待应答
	{
		I2C_Stop();
		return 1;
	}
	I2C_SendByte(reg);//写寄存器地址
	I2C_WaitAck();//等待应答
	for(i=0;i<len;i++)
	{
		I2C_SendByte(buf[i]);//发送数据
		if(I2C_WaitAck())//等待ACK
		{
			I2C_Stop();
			return 1;
		}
	}
	I2C_Stop();
	return 0;
}
//I2C连续读
//addr:器件地址
//reg:要读取的寄存器地址
//len:要读取得长度
//buf:读取到的数据存储区
//返回值: 0,正常
//				其他，错误代码
uint8_t Mult_Read(uint8_t addr,uint8_t reg,uint8_t len,uint8_t *buf)
{
	I2C_Start();
	I2C_SendByte((addr)|0);//发送器件地址+写命令
	if(I2C_WaitAck())//等待应答
	{
		I2C_Stop();
		return 1;
	}
	I2C_SendByte(reg|0x80);//写寄存器地址
	I2C_WaitAck();//等待应答
	I2C_Start();
	I2C_SendByte((addr)|1);//发送器件地址+读命令
	I2C_WaitAck();//等待应答
	while(len)
	{
		if(len==1) *buf=I2C_ReadByte(0);//读数据，发送nACK
		else *buf=I2C_ReadByte(1);//读数据，发送ACK
		len--;
		buf++;
	}
	I2C_Stop();//产生一个停止条件
	return 0;
}


/* bad code
__IO uint32_t	timeout = I2C_TIMEOUT;

void XyzInit(void){
	outflag = 0;
	outcount = OUT_X_L;
	for(outcount = 0; outcount < OUT_COUNT; outcount ++)
		xyzdata[outcount] = 0;
	CLK_HSIPrescalerConfig(CLK_PRESCALER_HSIDIV2);
	I2C_Init(I2CSPEED,SELFADDRESS,I2C_DUTYCYCLE_2,I2C_ACK_CURR,I2C_ADDMODE_7BIT,16);

}

int XyzUpdate(){
	timeout = I2C_TIMEOUT;
  while(I2C_GetFlagStatus(I2C_FLAG_BUSBUSY)){
    if((timeout--) == 0) 
		{
			I2C_GenerateSTOP( ENABLE);
			return 1;
		}
  }
  I2C_GenerateSTART(ENABLE);			// 1: ST
  timeout = I2C_TIMEOUT;
  while(!I2C_CheckEvent( I2C_EVENT_MASTER_MODE_SELECT)){
    if((timeout--) == 0) return 2;
  }
  I2C_Send7bitAddress( (uint8_t)DEVICEADDRESS, I2C_DIRECTION_TX); // 2: SAD+W
  timeout = I2C_TIMEOUT;
  while(!I2C_CheckEvent( I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED)){
    if((timeout--) == 0) return 3;
  }
	I2C_SendData((0x0F)); 	// 3: SUB
  timeout = I2C_TIMEOUT;
  while(I2C_GetFlagStatus(I2C_FLAG_TRANSFERFINISHED) == RESET){
    if((timeout--) == 0) return 4;
  }

  I2C_GenerateSTART( ENABLE);	// 4: SR

  timeout = I2C_TIMEOUT;
  while(!I2C_CheckEvent( I2C_EVENT_MASTER_MODE_SELECT))
  {
    if((timeout--) == 0) return 5;
  }

  I2C_Send7bitAddress((uint8_t)DEVICEADDRESS, I2C_DIRECTION_RX);	// 5:SAD+R
	
	timeout = I2C_TIMEOUT;
	while(I2C_GetFlagStatus( I2C_FLAG_ADDRESSSENTMATCHED) == RESET)
	{
    if((timeout--) == 0) return 6;
  }
	
	I2C_AcknowledgeConfig(I2C_ACK_NONE);
	
	(void)I2C->SR3;
	
	I2C_GenerateSTOP( ENABLE);
	
	I2C_AcknowledgeConfig( I2C_ACK_CURR);

  while(!I2C_CheckEvent( I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED))
  {
    if((timeout--) == 0) return 6;
  }

	
	timeout = I2C_TIMEOUT;	
	while(!I2C_CheckEvent( I2C_EVENT_MASTER_BYTE_RECEIVED))
	{
		if((timeout--) == 0) return 7;
	}

	
	xyzdata[0] = I2C_ReceiveData();
	
	I2C_AcknowledgeConfig(I2C_ACK_NONE);
	
	I2C_GenerateSTOP(ENABLE);
	//I2C_ClearFlag(I2C_FLAG_BUSBUSY);	
	
	return 0;
}
	
	
int XyzPower(void){
	timeout = I2C_TIMEOUT;
  while(I2C_GetFlagStatus( I2C_FLAG_BUSBUSY)){
		if((timeout--) == 0) return 1;
  }
  I2C_GenerateSTART(ENABLE);			// 1: ST
  timeout = I2C_TIMEOUT;
  while(!I2C_CheckEvent( I2C_EVENT_MASTER_MODE_SELECT)){
    if((timeout--) == 0) return 2;
  }
  I2C_Send7bitAddress( (uint8_t)DEVICEADDRESS, I2C_DIRECTION_TX); // 2: SAD+W
  timeout = I2C_TIMEOUT;
  while(!I2C_CheckEvent( I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED)){
    if((timeout--) == 0) return 3;
  }
	I2C_SendData(CTRL_REG1); 	// 3: SUB
  timeout = I2C_TIMEOUT;
  while(I2C_GetFlagStatus(I2C_FLAG_TRANSFERFINISHED) == RESET){
    if((timeout--) == 0) return 4;
  }

	I2C_SendData(0x77); 	// 4: DATA
	
	while(I2C_GetFlagStatus(I2C_FLAG_TRANSFERFINISHED) == RESET){
    if((timeout--) == 0) return 5;
  }
	
	I2C_GenerateSTOP(ENABLE);
	//I2C_ClearFlag(I2C_FLAG_BUSBUSY);
	return 0;
}
*/


