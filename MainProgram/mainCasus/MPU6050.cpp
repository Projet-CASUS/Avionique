#include "DigitalIn.h"
#include "DigitalOut.h"
#include "MPU6050.h"
#include "PinNames.h"
#include "mstd_iterator"
#include <cstdint>
#include <cstdio>

MPU6050::MPU6050(PinName sda_pin,PinName scl_pin,int address, uint8_t ii2_speed) : i2cBus(sda_pin,scl_pin), address_MPU6050(address<<1),  iic_speed_(ii2_speed) {
    activate();
}


void MPU6050::activate() {
    set_iic_Speed(iic_speed_);
    char activation_cmd[2];
    activation_cmd[0] = PWR_MGMT_1;
    activation_cmd[1] = 0;
    writeRegister(activation_cmd, 2);
}

/// <summary>
///     changes the i2c speed
/// </summary>
/// <param> none </param>
/// <return> the i2c speed </return>
void MPU6050::set_iic_Speed(uint8_t new_speed) {
    i2cBus.frequency(400000 / (1 + new_speed));
}

void MPU6050::writeThenReadRegister(int register_To_Read,char* data,int size) {
    char cmd[1] = {0};
    cmd[0] = register_To_Read;
    i2cBus.write(address_MPU6050,cmd,1,true);
    i2cBus.read(address_MPU6050,data,size);
}

void MPU6050::writeRegister(char* data,int size,bool stop) {
    i2cBus.write(address_MPU6050,data,size,stop);
}

/// <summary>
///     the function sets all the registers to the values pass by   ///     paramaters
/// </summary>
/// <param>the sample rate division, the bandpass filter cutoff frequency, the accelerometer range ns the gyroscope range </param>
/// <return> none </return>
void MPU6050::setConfiguration(char c_range,char f_range,char g_range,char a_range) {
    // first set sample rate
    setConfigRegister(SMPRT_DIV, &c_range);
    // set filter bandwidth
    setConfigRegister(CONFIG, &f_range);
    // set gyro range
    setConfigRegister(GYRO_CONFIG, &g_range);
    // set accel range
    setConfigRegister(ACCEL_CONFIG, &a_range);

    config_ranges.a_range = a_range;
    config_ranges.g_range = g_range;
}

/// <summary>
///     the function is used to send the config data to the sensor
/// </summary>
/// <param> none </param>
/// <return> the register to write to and the code to write </return>
void MPU6050::setConfigRegister(char registerCode,char* code) {
    char data_to_send[2];
    char readValue = 0;
    char* reading = &readValue;
    writeThenReadRegister(registerCode, reading, 1);
    data_to_send[0] = registerCode;
    data_to_send[1] = *code;
    writeRegister(data_to_send, 2);
}

bool MPU6050::begin(uint8_t c_range,uint8_t f_range,uint8_t g_range,uint8_t a_range) {
    char data[1];
    writeThenReadRegister(MPU_WHO_AM_I,data,1);
    if(data[0] == 0x68) {
        setConfiguration(c_range,f_range,g_range,a_range);
        return true;
    }
    return false;
}


/// <summary>
///     getAllReadings read every registers ans then store the values in the    ///     different variables.
/// </summary>
/// <param> void </param>
/// <return> void </return>
void MPU6050::getAllReadings() {
    char datareturned[14];

    writeThenReadRegister(ACCEL_XOUT_H, datareturned, 14);
    //rawAccel
    rawAccX = datareturned[0] << 8 | datareturned[1];
    rawAccY = datareturned[2] << 8 | datareturned[3];
    rawAccZ = datareturned[4] << 8 | datareturned[5];
    //rawGyro
    rawGyroX = datareturned[8] << 8 | datareturned[9];
    rawGyroY = datareturned[10] << 8 | datareturned[11];
    rawGyroZ = datareturned[12] << 8 | datareturned[13];

    // rawTemp
    rawTemp = (datareturned[6] << 8 | datareturned[7]);

    temperature = (rawTemp*10 / 34) + 3653;
    
    int32_t accel_scale;
    if(config_ranges.a_range == MPU6050_RANGE_2_G)
        accel_scale = 16384;
    if(config_ranges.a_range == MPU6050_RANGE_4_G)
        accel_scale = 8192;
    if(config_ranges.a_range == MPU6050_RANGE_8_G)
        accel_scale = 4096;
    if(config_ranges.a_range == MPU6050_RANGE_16_G)
        accel_scale = 2048;

    AccelX = rawAccX*1000 / accel_scale;
    AccelY = rawAccY*1000 / accel_scale;
    AccelZ = rawAccZ*1000 / accel_scale;

    int32_t gyro_scale;
    if (config_ranges.g_range == MPU6050_RANGE_250_DEG)
        gyro_scale = 1310;
    if (config_ranges.g_range == MPU6050_RANGE_500_DEG)
        gyro_scale = 655;
    if (config_ranges.g_range == MPU6050_RANGE_1000_DEG)
        gyro_scale = 328;
    if (config_ranges.g_range == MPU6050_RANGE_2000_DEG)
        gyro_scale = 164;

    GyroX = (rawGyroX*1000) / gyro_scale;
    GyroY = (rawGyroY*1000) / gyro_scale;
    GyroZ = (rawGyroZ*1000) / gyro_scale;
}


/// <summary>
///     return the value of the temperature.
/// </summary>
/// <param> void </param>
/// <return> temperature </return>
int32_t MPU6050::getTemperature() {
    return temperature;
}

/// <summary>
///     return the value of one the the acceleration values
/// </summary>
/// <param> the index the axe </param>
/// <return> the acceleration value of the selected axe </return>
int32_t MPU6050::getAccel(int index) {
    float returnValue = 0.0f;
    switch(index) {
        case 0:
            returnValue = AccelX;
            break;
        case 1:
            returnValue = AccelY;
            break;
        case 2:
            returnValue = AccelZ;
            break;
    }
    return returnValue;
}

/// <summary>
///     return the value of one the the gyro values
/// </summary>
/// <param> the index the axe </param>
/// <return> the gyro value of the selected axe </return>
int32_t MPU6050::getGyro(int index) {
    float returnValue = 0.0f;
    switch(index) {
        case 0:
            returnValue = GyroX;
            break;
        case 1:
            returnValue = GyroY;
            break;
        case 2:
            returnValue = GyroZ;
            break;
    }
    return returnValue;
}

int32_t MPU6050::operator[]( int index ) {
    if(index < 3)
        return getAccel(index);
    else if(index == 3)
        return getTemperature();
    else if(index >= 4 && index < 7)
        return getGyro(index - 4);
    return -1;
}

/// <summary>
///     calibrates the sensors accel and gyro
/// </summary>
/// <param> the axe to calibrate </param>
/// <return> the offset value</return>
int32_t* MPU6050::calibration(int axe) {
    DigitalOut signal(LED1); // may be map to a different pin
    DigitalIn button(D8);

    int32_t accels[12],gyros[12];
    int32_t averAccel, averGyro;
    int iteration = 0;
    if(button) {
        while(iteration < 12) {
            getAllReadings();

            accels[iteration] = getAccel(axe);

            gyros[iteration] = getGyro(axe);
            iteration++;
        }
        int32_t averAccel = average(accels, 12);
        int32_t averGyro = average(gyros, 12);
    }
    int32_t averages[2] = {averAccel, averGyro};
    return averages;
}

int32_t MPU6050::average(int32_t* values,int length) {
    int32_t sum = 0;
    for(int i=0;i<length;i++) {
        sum += values[i];
    }
    return (sum / length);
}
