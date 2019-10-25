#include "mt9v034.h"
#define CAM_ADDRESS 0x48<<1
extern I2C_HandleTypeDef hi2c1;

void mtWriteReg(uint8_t address,uint16_t val){
	uint8_t data[3];
	data[2]=val&0xff;
	data[1]=(val>>8)&0xff;
	data[0]=address;
  HAL_I2C_Master_Transmit(&hi2c1,CAM_ADDRESS,data,3,100);
}
uint16_t mtReadReg(uint8_t address){
  uint8_t data[2]={0,0};
	uint8_t addr=address;
  HAL_I2C_Master_Transmit(&hi2c1,CAM_ADDRESS,&addr,1,100);
	HAL_I2C_Master_Receive(&hi2c1,CAM_ADDRESS,data,2,100);
	return (data[0]<<8)|(data[1]);
}

void LVDS_Enable(void){
	uint16_t reg_val=0;
	reg_val=mtReadReg(0xb3);
	reg_val&=~(1<<4);
	mtWriteReg(0xb3,reg_val);
	HAL_Delay(1);
  reg_val=mtReadReg(0xb1);
	reg_val&=~(1<<1);
	mtWriteReg(0xb1,reg_val);
	HAL_Delay(1);
	reg_val=mtReadReg(0x0c);
	reg_val&=~(1<<4);
	mtWriteReg(0x0c,reg_val);
	HAL_Delay(10);
	reg_val|=(1<<4);
	mtWriteReg(0x0c,reg_val);
	HAL_Delay(10);
}

void SLAVE_Enable(void){
	uint16_t reg_val=0;
	reg_val=0x380;
	mtWriteReg(0x07,reg_val);
	HAL_Delay(10);
}
void SNAPSHOT_MODE_Enable(void){
	uint16_t reg_val=0;
	reg_val=0x190;
	mtWriteReg(0x07,reg_val);
	HAL_Delay(10);
}
void AGC_AEC_DISABLE(void){
	mtWriteReg(0xAF,0); 	//Disable AEC and AGC
	HAL_Delay(10);
}

void SET_EXPOSURE(int val)
{
	if(val>32767)
		val=32767;
  else if(val<0)
		val=0;
	mtWriteReg(0x0B,(unsigned int) val);
	//HAL_Delay(1);		
}
void SET_ANALOG_GAIN(int val)
{
	if(val>60)
		val=60;
  else if(val<0)
		val=0;
	mtWriteReg(0x35,(unsigned int) val);
	//HAL_Delay(1);		
}
void SET_DIGITAL_GAIN(int val)
{
	if(val>0xF)
		val=0xf;
  else if(val<0)
		val=0;
	for(int i=0;i<25;i++)
		mtWriteReg(0x80+i,0xF0|(unsigned int)val);
		//HAL_Delay(1);		
}
void LVDS_SYNC_ENABLE(void){
	mtWriteReg(0xb5,1);
	HAL_Delay(1);
}

void LVDS_SYNC_DESABLE(void){
	mtWriteReg(0xb5,0);
	HAL_Delay(1);
}

void resetSlave(void)
{
		HAL_GPIO_WritePin(GPIOA,GPIO_PIN_4,GPIO_PIN_RESET);
		HAL_Delay(3000);
		HAL_GPIO_WritePin(GPIOA,GPIO_PIN_4,GPIO_PIN_SET);

}