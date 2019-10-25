
#ifndef __MT9V034
#define __MT9V034
#include "main.h"

void mtWriteReg(uint8_t address,uint16_t val);
uint16_t mtReadReg(uint8_t address);
void LVDS_Enable(void);
void SLAVE_Enable(void);
void SNAPSHOT_MODE_Enable(void);
void AGC_AEC_DISABLE(void);
void SET_EXPOSURE(int val);
void SET_ANALOG_GAIN(int val);
void SET_DIGITAL_GAIN(int val);
void LVDS_SYNC_ENABLE(void);
void LVDS_SYNC_DESABLE(void);
void resetSlave(void);

#endif