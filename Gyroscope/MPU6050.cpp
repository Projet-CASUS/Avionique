#include "MPU6050.h"
#include "PinNames.h"
#include "mstd_iterator"
#include <cstdint>

MPU6050::MPU6050(PinName sda_pin,PinName scl_pin,int address) : i2cBus(sda_pin,scl_pin), addres_MPU6050(address<<1) {
    activate();
}

void MPU6050::activate() {
    i2cBus.frequency(100000);
    uint8_t activation_cmd[2];
    activation_cmd[0] = PWR_MGMT_1;
    activation_cmd[1] = 0;
    writeRegister(activation_cmd, 2);
}

void MPU6050::writeThenReadRegister(int register_To_Read,uint8_t* data,int size) {
    char cmd[1] = {0};
    cmd[0] = register_To_Read;
    i2cBus.write(addres_MPU6050,cmd,1,true);
    i2cBus.read(addres_MPU6050,(char*)data,1);
}

void MPU6050::writeRegister(uint8_t* data,int size,bool stop) {
    i2cBus.write(addres_MPU6050,(char*)data,size,stop);
}

void MPU6050::setCongiguration(uint8_t c_range,uint8_t f_range,uint8_t g_range,uint8_t a_range) {
    // first set sample rate
    setSampleRate(&c_range);
    // set filter bandwidth
    setFilterBandWidth(&f_range);
    // set gyro range
    setGyroRange(&g_range);
    // set accel range
    setAccelRange(&a_range);
    config_ranges.a_range = a_range;
    config_ranges.g_range = g_range;
}

void MPU6050::setSampleRate(uint8_t* smp_div) {
    uint8_t* data_to_send[2];
    writeThenReadRegister(SMPRT_DIV,smp_div,1);
    data_to_send[0] = (uint8_t*)SMPRT_DIV;
    data_to_send[1] = smp_div;
    writeRegister(*data_to_send, 2);
}

void MPU6050::setFilterBandWidth(uint8_t* filter) {
    uint8_t* data_to_send[2];
    writeThenReadRegister(CONFIG, filter, 1);
    data_to_send[0] = (uint8_t*)CONFIG;
    data_to_send[1] = filter;
    writeRegister(*data_to_send, 2);
}

void MPU6050::setGyroRange(uint8_t* gyroRange) {
    uint8_t* data_to_send[2];
    writeThenReadRegister(GYRO_CONFIG, gyroRange, 1);
    data_to_send[0] = (uint8_t*)GYRO_CONFIG;
    data_to_send[1] = gyroRange;
    writeRegister(*data_to_send, 2);
}

void MPU6050::setAccelRange(uint8_t* accelRange) {
    uint8_t* data_to_send[2];
    writeThenReadRegister(ACCEL_CONFIG, accelRange, 1);
    data_to_send[0] = (uint8_t*)ACCEL_CONFIG;
    data_to_send[1] = accelRange;
    writeRegister(*data_to_send, 2);
}

bool MPU6050::begin(uint8_t c_range,uint8_t f_range,uint8_t g_range,uint8_t a_range) {
    uint8_t data[1];
    writeThenReadRegister(MPU_WHO_AM_I,data,1);
    if(data[0] == 0x68) {
        setCongiguration(c_range,f_range,g_range,a_range);
        return true;
    }
    return false;
}   

Readings MPU6050::getAllReadings() {
    uint8_t datareturned[14];
    Readings allReandings;

    writeThenReadRegister(ACCEL_XOUT_H, datareturned, 14);
    //rawAccel
    rawAccX = datareturned[0] << 8 || datareturned[1];
    rawAccY = datareturned[2] << 8 || datareturned[3];
    rawAccZ = datareturned[4] << 8 || datareturned[5];

    //rawGyro
    rawGyroX = datareturned[8] << 8 || datareturned[9];
    rawGyroY = datareturned[10] << 8 || datareturned[11];
    rawGyroZ = datareturned[12] << 8 || datareturned[13];

    // rawTemp
    rawTemp = datareturned[6] << 8 || datareturned[7];

    allReandings.temperature = (rawTemp / 340) + 36.53;
    
    float accel_scale = 1;
    if(config_ranges.a_range == MPU6050_RANGE_2_G)
        accel_scale = 16384;
    if(config_ranges.a_range == MPU6050_RANGE_4_G)
        accel_scale = 8192;
    if(config_ranges.a_range == MPU6050_RANGE_8_G)
        accel_scale = 4096;
    if(config_ranges.a_range == MPU6050_RANGE_16_G)
        accel_scale = 2048;

    allReandings.acceleration[0] = ((float)rawAccX) / accel_scale;
    allReandings.acceleration[1] = ((float)rawAccY) / accel_scale;
    allReandings.acceleration[2] = ((float)rawAccZ) / accel_scale;

    float gyro_scale = 1;
    if (config_ranges.g_range == MPU6050_RANGE_250_DEG)
        gyro_scale = 131;
    if (config_ranges.g_range == MPU6050_RANGE_500_DEG)
        gyro_scale = 65.5;
    if (config_ranges.g_range == MPU6050_RANGE_1000_DEG)
        gyro_scale = 32.8;
    if (config_ranges.g_range == MPU6050_RANGE_2000_DEG)
        gyro_scale = 16.4;

    allReandings.gyroscope[0] = ((float)rawGyroX) / gyro_scale;
    allReandings.gyroscope[1] = ((float)rawGyroY) / gyro_scale;
    allReandings.gyroscope[2] = ((float)rawGyroZ) / gyro_scale;

    return allReandings;
}