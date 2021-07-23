#ifndef MPU6050_H
#define MPU6050_H

#include "I2C.h"
#include "PinNames.h"
#include "mbed.h"
#include <cstdint>

#define MPU_DEFAULT_ADRESS 0x68
#define SMPRT_DIV 0x19 // enum mpu6050_clock_select_t
#define CONFIG 0x1A // enum mpu6050_bandwidth_t
#define GYRO_CONFIG 0x1B
#define ACCEL_CONFIG 0x1C

#define ACCEL_XOUT_H 0x3B
#define ACCEL_XOUT_L 0x3C
#define ACCEL_YOUT_H 0x3D
#define ACCEL_YOUT_L 0x3E
#define ACCEL_ZOUT_H 0x3F
#define ACCEL_ZOUT_L 0x3F

#define TEMP_OUT_H 0x41
#define TEMP_OUT_L 0x42

#define GYRO_XOUT_H 0x43
#define GYRO_XOUT_L 0x44
#define GYRO_YOUT_H 0x45
#define GYRO_YOUT_L 0x46
#define GYRO_ZOUT_H 0x47
#define GYRO_ZOUT_L 0x48

#define PWR_MGMT_1 0x6B
#define PWR_MGMT_2 0x6C
#define MPU_WHO_AM_I 0x75

typedef enum {
    MPU6050_RANGE_2_G = 0,
    MPU6050_RANGE_4_G = 1,
    MPU6050_RANGE_8_G = 2,
    MPU6050_RANGE_16_G = 3,
} mpu6050_accel_range;

typedef enum {
    MPU6050_RANGE_250_DEG = 0,
    MPU6050_RANGE_500_DEG = 1,
    MPU6050_RANGE_1000_DEG = 2,
    MPU6050_RANGE_2000_DEG = 3,
} mpu6050_gyro_range;

typedef enum {
  MPU6050_BAND_260_HZ = 0,
  MPU6050_BAND_184_HZ = 1,
  MPU6050_BAND_94_HZ = 2,
  MPU6050_BAND_44_HZ = 3,
  MPU6050_BAND_21_HZ = 4,
  MPU6050_BAND_10_HZ = 5,
  MPU6050_BAND_5_HZ = 6,
} mpu6050_bandwidth_t;

typedef enum {
  MPU6050_DIV_0 = 0,
  MPU6050_DIV_1 = 1,
  MPU6050_DIV_2 = 2,
  MPU6050_DIV_3 = 3,
  MPU6050_DIV_4 = 4,
  MPU6050_DIV_5 = 5,
  MPU6050_DIV_6 = 6,
  MPU6050_STOP = 7,
} mpu6050_clock_select_t;

struct configurations {
    uint8_t a_range;
    uint8_t g_range;
};

class MPU6050
{
public:
    MPU6050(PinName sda_pin,PinName scl_pin,int address,uint8_t ii2_speed=3);
    void set_iic_Speed(uint8_t new_speed);
    int32_t operator[] (int);
    bool begin(uint8_t c_range = MPU6050_DIV_0,uint8_t f_range = MPU6050_BAND_21_HZ,uint8_t g_range = MPU6050_RANGE_250_DEG,uint8_t a_range=MPU6050_RANGE_2_G);
    int32_t* calibration(int axe);
    int32_t getTemperature();
    int32_t getAccel(int index=0);
    int32_t getGyro(int index=0);
private:
    void activate();
    I2C i2cBus;
    int address_MPU6050;
    uint8_t iic_speed_;
    int16_t rawAccX, rawAccY, rawAccZ, rawTemp, rawGyroX, rawGyroY, rawGyroZ;
    int32_t temperature, AccelX, AccelY, AccelZ, GyroX, GyroY, GyroZ;
    configurations config_ranges;
    void writeThenReadRegister(int register_To_Read,char* data,int size);
    void writeRegister(char* data,int size,bool stop = false);
    void setConfiguration(char c_range,char f_range,char g_range,char a_range);
    void getAllReadings();
    void setConfigRegister(char registerCode,char* code);
    int32_t average(int32_t* values,int length);
};

#endif
