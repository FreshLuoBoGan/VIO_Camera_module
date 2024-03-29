
#include "MPU9250_SPI.h"

const uint8_t SPI_READ = 0x80;
int _SPI_HS = 0;
int16_t _axcounts, _aycounts, _azcounts;
int16_t _gxcounts, _gycounts, _gzcounts;
int16_t _hxcounts, _hycounts, _hzcounts;
int16_t _tcounts;
float _accelScale;
float _gyroScale;
float _magScaleX, _magScaleY, _magScaleZ;

const int16_t tX[3] = {0,  1,  0};
const int16_t tY[3] = {1,  0,  0};
const int16_t tZ[3] = {0,  0, -1};
// constants
const float G = 9.807f;
const float _d2r = 3.14159265359f / 180.0f;
// MPU9250 registers
const uint8_t ACCEL_OUT = 0x3B;
const uint8_t GYRO_OUT = 0x43;
const uint8_t TEMP_OUT = 0x41;
const uint8_t EXT_SENS_DATA_00 = 0x49;
const uint8_t ACCEL_CONFIG = 0x1C;
const uint8_t ACCEL_FS_SEL_2G = 0x00;
const uint8_t ACCEL_FS_SEL_4G = 0x08;
const uint8_t ACCEL_FS_SEL_8G = 0x10;
const uint8_t ACCEL_FS_SEL_16G = 0x18;
const uint8_t GYRO_CONFIG = 0x1B;
const uint8_t GYRO_FS_SEL_250DPS = 0x00;
const uint8_t GYRO_FS_SEL_500DPS = 0x08;
const uint8_t GYRO_FS_SEL_1000DPS = 0x10;
const uint8_t GYRO_FS_SEL_2000DPS = 0x18;
const uint8_t ACCEL_CONFIG2 = 0x1D;
const uint8_t ACCEL_DLPF_184 = 0x01;
const uint8_t ACCEL_DLPF_92 = 0x02;
const uint8_t ACCEL_DLPF_41 = 0x03;
const uint8_t ACCEL_DLPF_20 = 0x04;
const uint8_t ACCEL_DLPF_10 = 0x05;
const uint8_t ACCEL_DLPF_5 = 0x06;
const uint8_t CONFIG = 0x1A;
const uint8_t GYRO_DLPF_184 = 0x01;
const uint8_t GYRO_DLPF_92 = 0x02;
const uint8_t GYRO_DLPF_41 = 0x03;
const uint8_t GYRO_DLPF_20 = 0x04;
const uint8_t GYRO_DLPF_10 = 0x05;
const uint8_t GYRO_DLPF_5 = 0x06;
const uint8_t SMPDIV = 0x19;
const uint8_t INT_PIN_CFG = 0x37;
const uint8_t INT_ENABLE = 0x38;
const uint8_t INT_DISABLE = 0x00;
const uint8_t INT_PULSE_50US = 0x00;
const uint8_t INT_WOM_EN = 0x40;
const uint8_t INT_RAW_RDY_EN = 0x01;
const uint8_t PWR_MGMNT_1 = 0x6B;
const uint8_t PWR_CYCLE = 0x20;
const uint8_t PWR_RESET = 0x80;
const uint8_t CLOCK_SEL_PLL = 0x01;
const uint8_t PWR_MGMNT_2 = 0x6C;
const uint8_t SEN_ENABLE = 0x00;
const uint8_t DIS_GYRO = 0x07;
const uint8_t USER_CTRL = 0x6A;
const uint8_t I2C_MST_EN = 0x20;
const uint8_t I2C_MST_CLK = 0x0D;
const uint8_t I2C_MST_CTRL = 0x24;
const uint8_t I2C_SLV0_ADDR = 0x25;
const uint8_t I2C_SLV0_REG = 0x26;
const uint8_t I2C_SLV0_DO = 0x63;
const uint8_t I2C_SLV0_CTRL = 0x27;
const uint8_t I2C_SLV0_EN = 0x80;
const uint8_t I2C_READ_FLAG = 0x80;
const uint8_t MOT_DETECT_CTRL = 0x69;
const uint8_t ACCEL_INTEL_EN = 0x80;
const uint8_t ACCEL_INTEL_MODE = 0x40;
const uint8_t LP_ACCEL_ODR = 0x1E;
const uint8_t WOM_THR = 0x1F;
const uint8_t WHO_AM_I = 0x75;
const uint8_t FIFO_EN = 0x23;
const uint8_t FIFO_TEMP = 0x80;
const uint8_t FIFO_GYRO = 0x70;
const uint8_t FIFO_ACCEL = 0x08;
const uint8_t FIFO_MAG = 0x01;
const uint8_t FIFO_COUNT = 0x72;
const uint8_t FIFO_READ = 0x74;
// AK8963 registers
const uint8_t AK8963_I2C_ADDR = 0x0C;
const uint8_t AK8963_HXL = 0x03;
const uint8_t AK8963_CNTL1 = 0x0A;
const uint8_t AK8963_PWR_DOWN = 0x00;
const uint8_t AK8963_CNT_MEAS1 = 0x12;
const uint8_t AK8963_CNT_MEAS2 = 0x16;
const uint8_t AK8963_FUSE_ROM = 0x0F;
const uint8_t AK8963_CNTL2 = 0x0B;
const uint8_t AK8963_RESET = 0x01;
const uint8_t AK8963_ASA = 0x10;
const uint8_t AK8963_WHO_AM_I = 0x00;


uint8_t _buffer[21];

int mpu9250WriteReg(uint8_t regAddr, uint8_t regVal) {

  uint8_t data[2] ;
  data[0] = regAddr;
  data[1] = regVal;
  if (hspi1.Init.BaudRatePrescaler != SPI_BAUDRATEPRESCALER_128) {
    hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_128;
    HAL_SPI_Init(&hspi1);
  }
  NSS_LOW;
  HAL_SPI_Transmit(&hspi1, data, 2, 100);
  NSS_HIGH;
  return 1;
}

int mpu9250ReadReg( uint8_t regAddr, uint8_t *regVal , uint16_t count) {
  if (_SPI_HS) {
    if (hspi1.Init.BaudRatePrescaler != SPI_BAUDRATEPRESCALER_4) {
      hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_4;
      HAL_SPI_Init(&hspi1);
    }
  } else {
    if (hspi1.Init.BaudRatePrescaler != SPI_BAUDRATEPRESCALER_128) {
      hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_128;
      HAL_SPI_Init(&hspi1);
    }
  }
  uint8_t val;
  val = regAddr | SPI_READ;
  NSS_LOW;
  HAL_SPI_Transmit(&hspi1, &val, 1, 100);
  HAL_SPI_Receive(&hspi1, regVal, count, 100);
  NSS_HIGH;
  return 1;
}

int readRegisters(uint8_t subAddress, uint8_t count, uint8_t* dest) {

  mpu9250ReadReg( subAddress,  dest , count);

  return 1;
}

int writeRegister(uint8_t subAddress, uint8_t data) {
  /* write data to device */
  mpu9250WriteReg(subAddress, data);

  HAL_Delay(10);

  /* read back the register */
  readRegisters(subAddress, 1, _buffer);

  /* check the read back register against the written register */
  if (_buffer[0] == data) {
    return 1;
  }
  else {
    return -1;
  }
}

int writeAK8963Register(uint8_t subAddress, uint8_t data) {
  // set slave 0 to the AK8963 and set for write
  if (writeRegister(I2C_SLV0_ADDR, AK8963_I2C_ADDR) < 0) {
    return -1;
  }
  // set the register to the desired AK8963 sub address
  if (writeRegister(I2C_SLV0_REG, subAddress) < 0) {
    return -2;
  }
  // store the data for write
  if (writeRegister(I2C_SLV0_DO, data) < 0) {
    return -3;
  }
  // enable I2C and send 1 byte
  if (writeRegister(I2C_SLV0_CTRL, I2C_SLV0_EN | (uint8_t)1) < 0) {
    return -4;
  }
  // read the register and confirm
  if (readAK8963Registers(subAddress, 1, _buffer) < 0) {
    return -5;
  }
  if (_buffer[0] == data) {
    return 1;
  } else {
    return -6;
  }
}

int readAK8963Registers(uint8_t subAddress, uint8_t count, uint8_t* dest) {
  // set slave 0 to the AK8963 and set for read
  int _status;
  if (writeRegister(I2C_SLV0_ADDR, AK8963_I2C_ADDR | I2C_READ_FLAG) < 0) {
    return -1;
  }
  // set the register to the desired AK8963 sub address
  if (writeRegister(I2C_SLV0_REG, subAddress) < 0) {
    return -2;
  }
  // enable I2C and request the bytes
  if (writeRegister(I2C_SLV0_CTRL, I2C_SLV0_EN | count) < 0) {
    return -3;
  }
  HAL_Delay(1); // takes some time for these registers to fill
  // read the bytes off the MPU9250 EXT_SENS_DATA registers
  _status = readRegisters(EXT_SENS_DATA_00, count, dest);
  return _status;
}

int whoAmI(void) {
  // read the WHO AM I register
  if (readRegisters(WHO_AM_I, 1, _buffer) < 0) {
    return -1;
  }
  // return the register value
  return _buffer[0];
}

/* gets the AK8963 WHO_AM_I register value, expected to be 0x48 */
int whoAmIAK8963(void) {
  // read the WHO AM I register
  if (readAK8963Registers(AK8963_WHO_AM_I, 1, _buffer) < 0) {
    return -1;
  }
  // return the register value
  return _buffer[0];
}


int mpuBegin(void) {

  // select clock source to gyro
  if (writeRegister(PWR_MGMNT_1, CLOCK_SEL_PLL) < 0) {
    return -1;
  }
  // enable I2C master mode
  if (writeRegister(USER_CTRL, I2C_MST_EN) < 0) {
    return -2;
  }
  // set the I2C bus speed to 400 kHz
  if (writeRegister(I2C_MST_CTRL, I2C_MST_CLK) < 0) {
    return -3;
  }
  // set AK8963 to Power Down
  writeAK8963Register(AK8963_CNTL1, AK8963_PWR_DOWN);
  // reset the MPU9250
  writeRegister(PWR_MGMNT_1, PWR_RESET);
  // wait for MPU-9250 to come back up
  HAL_Delay(1);
  // reset the AK8963
  writeAK8963Register(AK8963_CNTL2, AK8963_RESET);
  // select clock source to gyro
  if (writeRegister(PWR_MGMNT_1, CLOCK_SEL_PLL) < 0) {
    return -4;
  }
  // check the WHO AM I byte, expected value is 0x71 (decimal 113) or 0x73 (decimal 115)
  if ((whoAmI() != 113) && (whoAmI() != 115)) {
    return -5;
  }
  // enable accelerometer and gyro
  if (writeRegister(PWR_MGMNT_2, SEN_ENABLE) < 0) {
    return -6;
  }
  // setting accel range to 16G as default
  if (writeRegister(ACCEL_CONFIG, ACCEL_FS_SEL_4G) < 0) {
    return -7;
  }

  // setting the gyro range to 2000DPS as default
  if (writeRegister(GYRO_CONFIG, GYRO_FS_SEL_1000DPS) < 0) {
    return -8;
  }

  // setting bandwidth to 184Hz as default
  if (writeRegister(ACCEL_CONFIG2, ACCEL_DLPF_184) < 0) {
    return -9;
  }
  if (writeRegister(CONFIG, GYRO_DLPF_184) < 0) { // setting gyro bandwidth to 184Hz
    return -10;
  }
  // setting the sample rate divider to 0 as default
  if (writeRegister(SMPDIV, 0x00) < 0) {
    return -11;
  }
  // enable I2C master mode
  if (writeRegister(USER_CTRL, I2C_MST_EN) < 0) {
    return -12;
  }
  // set the I2C bus speed to 400 kHz
  if ( writeRegister(I2C_MST_CTRL, I2C_MST_CLK) < 0) {
    return -13;
  }
  // check AK8963 WHO AM I register, expected value is 0x48 (decimal 72)
  if ( whoAmIAK8963() != 72 ) {
    return -14;
  }

  // set AK8963 to 16 bit resolution, 100 Hz update rate
  if (writeAK8963Register(AK8963_CNTL1, AK8963_CNT_MEAS2) < 0) {
    return -18;
  }
  HAL_Delay(100); // long wait between AK8963 mode changes
  // select clock source to gyro
  if (writeRegister(PWR_MGMNT_1, CLOCK_SEL_PLL) < 0) {
    return -19;
  }
  // instruct the MPU9250 to get 7 bytes of data from the AK8963 at the sample rate
  readAK8963Registers(AK8963_HXL, 7, _buffer);
  // successful init, return 1
  return 1;
}


int mpuReadSensor(struct mpuSensorData *data) {
  // grab the data from the MPU9250
  _SPI_HS = 1;
  if (readRegisters(ACCEL_OUT, 21, _buffer) < 0) {
    return -1;
  }

  // combine into 16 bit values
  data->ax = (((int16_t)_buffer[0]) << 8)  | _buffer[1];
  data->ay = (((int16_t)_buffer[2]) << 8)  | _buffer[3];
  data->az = (((int16_t)_buffer[4]) << 8)  | _buffer[5];
  data->t =  (((int16_t)_buffer[6]) << 8)  | _buffer[7];
  data->gx = (((int16_t)_buffer[8]) << 8)  | _buffer[9];
  data->gy = (((int16_t)_buffer[10]) << 8) | _buffer[11];
  data->gz = (((int16_t)_buffer[12]) << 8) | _buffer[13];
  data->hx = (((int16_t)_buffer[15]) << 8) | _buffer[14];
  data->hy = (((int16_t)_buffer[17]) << 8) | _buffer[16];
  data->hz = (((int16_t)_buffer[19]) << 8) | _buffer[18];

  return 1;
}
/* enables the data ready interrupt */

int enableDataReadyInterrupt(void) {
  // use low speed SPI for register setting
  _SPI_HS = 0;
  /* setting the interrupt */
  if (writeRegister(INT_PIN_CFG, INT_PULSE_50US) < 0) { // setup interrupt, 50 us pulse
    return -1;
  }
  if (writeRegister(INT_ENABLE, INT_RAW_RDY_EN) < 0) { // set to data ready
    return -2;
  }
  return 1;
}

/* disables the data ready interrupt */
int disableDataReadyInterrupt(void) {
  // use low speed SPI for register setting
  _SPI_HS = 0;
  if (writeRegister(INT_ENABLE, INT_DISABLE) < 0) { // disable interrupt
    return -1;
  }
  return 1;
}

/* sets the accelerometer full scale range to values other than default */
int setAccelRange(enum AccelRange range) {
  // use low speed SPI for register setting
  _SPI_HS = 0;
  switch (range) {
    case ACCEL_RANGE_2G: {
        // setting the accel range to 2G
        if (writeRegister(ACCEL_CONFIG, ACCEL_FS_SEL_2G) < 0) {
          return -1;
        }
        _accelScale = G * 2.0f / 32767.5f; // setting the accel scale to 2G
        break;
      }
    case ACCEL_RANGE_4G: {
        // setting the accel range to 4G
        if (writeRegister(ACCEL_CONFIG, ACCEL_FS_SEL_4G) < 0) {
          return -1;
        }
        _accelScale = G * 4.0f / 32767.5f; // setting the accel scale to 4G
        break;
      }
    case ACCEL_RANGE_8G: {
        // setting the accel range to 8G
        if (writeRegister(ACCEL_CONFIG, ACCEL_FS_SEL_8G) < 0) {
          return -1;
        }
        _accelScale = G * 8.0f / 32767.5f; // setting the accel scale to 8G
        break;
      }
    case ACCEL_RANGE_16G: {
        // setting the accel range to 16G
        if (writeRegister(ACCEL_CONFIG, ACCEL_FS_SEL_16G) < 0) {
          return -1;
        }
        _accelScale = G * 16.0f / 32767.5f; // setting the accel scale to 16G
        break;
      }
  }
  return 1;
}

/* sets the gyro full scale range to values other than default */
int setGyroRange(enum GyroRange range) {
  // use low speed SPI for register setting
  _SPI_HS = 0;
  switch (range) {
    case GYRO_RANGE_250DPS: {
        // setting the gyro range to 250DPS
        if (writeRegister(GYRO_CONFIG, GYRO_FS_SEL_250DPS) < 0) {
          return -1;
        }
        _gyroScale = 250.0f / 32767.5f * _d2r; // setting the gyro scale to 250DPS
        break;
      }
    case GYRO_RANGE_500DPS: {
        // setting the gyro range to 500DPS
        if (writeRegister(GYRO_CONFIG, GYRO_FS_SEL_500DPS) < 0) {
          return -1;
        }
        _gyroScale = 500.0f / 32767.5f * _d2r; // setting the gyro scale to 500DPS
        break;
      }
    case GYRO_RANGE_1000DPS: {
        // setting the gyro range to 1000DPS
        if (writeRegister(GYRO_CONFIG, GYRO_FS_SEL_1000DPS) < 0) {
          return -1;
        }
        _gyroScale = 1000.0f / 32767.5f * _d2r; // setting the gyro scale to 1000DPS
        break;
      }
    case GYRO_RANGE_2000DPS: {
        // setting the gyro range to 2000DPS
        if (writeRegister(GYRO_CONFIG, GYRO_FS_SEL_2000DPS) < 0) {
          return -1;
        }
        _gyroScale = 2000.0f / 32767.5f * _d2r; // setting the gyro scale to 2000DPS
        break;
      }
  }
  return 1;
}

/* sets the DLPF bandwidth to values other than default */
int setDlpfBandwidth(enum DlpfBandwidth bandwidth) {
  // use low speed SPI for register setting
  _SPI_HS = 0;
  switch (bandwidth) {
    case DLPF_BANDWIDTH_184HZ: {
        if (writeRegister(ACCEL_CONFIG2, ACCEL_DLPF_184) < 0) { // setting accel bandwidth to 184Hz
          return -1;
        }
        if (writeRegister(CONFIG, GYRO_DLPF_184) < 0) { // setting gyro bandwidth to 184Hz
          return -2;
        }
        break;
      }
    case DLPF_BANDWIDTH_92HZ: {
        if (writeRegister(ACCEL_CONFIG2, ACCEL_DLPF_92) < 0) { // setting accel bandwidth to 92Hz
          return -1;
        }
        if (writeRegister(CONFIG, GYRO_DLPF_92) < 0) { // setting gyro bandwidth to 92Hz
          return -2;
        }
        break;
      }
    case DLPF_BANDWIDTH_41HZ: {
        if (writeRegister(ACCEL_CONFIG2, ACCEL_DLPF_41) < 0) { // setting accel bandwidth to 41Hz
          return -1;
        }
        if (writeRegister(CONFIG, GYRO_DLPF_41) < 0) { // setting gyro bandwidth to 41Hz
          return -2;
        }
        break;
      }
    case DLPF_BANDWIDTH_20HZ: {
        if (writeRegister(ACCEL_CONFIG2, ACCEL_DLPF_20) < 0) { // setting accel bandwidth to 20Hz
          return -1;
        }
        if (writeRegister(CONFIG, GYRO_DLPF_20) < 0) { // setting gyro bandwidth to 20Hz
          return -2;
        }
        break;
      }
    case DLPF_BANDWIDTH_10HZ: {
        if (writeRegister(ACCEL_CONFIG2, ACCEL_DLPF_10) < 0) { // setting accel bandwidth to 10Hz
          return -1;
        }
        if (writeRegister(CONFIG, GYRO_DLPF_10) < 0) { // setting gyro bandwidth to 10Hz
          return -2;
        }
        break;
      }
    case DLPF_BANDWIDTH_5HZ: {
        if (writeRegister(ACCEL_CONFIG2, ACCEL_DLPF_5) < 0) { // setting accel bandwidth to 5Hz
          return -1;
        }
        if (writeRegister(CONFIG, GYRO_DLPF_5) < 0) { // setting gyro bandwidth to 5Hz
          return -2;
        }
        break;
      }
  }
  return 1;
}

/* sets the sample rate divider to values other than default */
int setSrd(uint8_t srd) {
  // use low speed SPI for register setting
  _SPI_HS = 0;
  /* setting the sample rate divider to 19 to facilitate setting up magnetometer */
  if (writeRegister(SMPDIV, 19) < 0) { // setting the sample rate divider
    return -1;
  }
  if (srd > 9) {
    // set AK8963 to Power Down
    if (writeAK8963Register(AK8963_CNTL1, AK8963_PWR_DOWN) < 0) {
      return -2;
    }
    HAL_Delay(100); // long wait between AK8963 mode changes
    // set AK8963 to 16 bit resolution, 8 Hz update rate
    if (writeAK8963Register(AK8963_CNTL1, AK8963_CNT_MEAS1) < 0) {
      return -3;
    }
    HAL_Delay(100); // long wait between AK8963 mode changes
    // instruct the MPU9250 to get 7 bytes of data from the AK8963 at the sample rate
    readAK8963Registers(AK8963_HXL, 7, _buffer);
  } else {
    // set AK8963 to Power Down
    if (writeAK8963Register(AK8963_CNTL1, AK8963_PWR_DOWN) < 0) {
      return -2;
    }
    HAL_Delay(100); // long wait between AK8963 mode changes
    // set AK8963 to 16 bit resolution, 100 Hz update rate
    if (writeAK8963Register(AK8963_CNTL1, AK8963_CNT_MEAS2) < 0) {
      return -3;
    }
    HAL_Delay(100); // long wait between AK8963 mode changes
    // instruct the MPU9250 to get 7 bytes of data from the AK8963 at the sample rate
    readAK8963Registers(AK8963_HXL, 7, _buffer);
  }
  /* setting the sample rate divider */
  if (writeRegister(SMPDIV, srd) < 0) { // setting the sample rate divider
    return -4;
  }
  return 1;
}
int mpuMyCustomConfig(void){
	writeRegister(ACCEL_CONFIG2,(1<<3));//Bypass The Filter
	writeRegister(GYRO_CONFIG,1);//Bypass the filter , 250 deg/s	
}