#ifndef BMP280_H
#define BMP280_H

#include "I2C.h"
#include "PinNames.h"
#include "mbed.h"
#include <cstdint>

const int DEFAULT_ADDRESS_BMP280 = 0x76;
const uint8_t SENSOR_ID = 0x58;

enum {
    BMP280_REISTER_DIGT1 = 0x88,
    BMP280_REISTER_DIGT2 = 0x8A,
    BMP280_REISTER_DIGT3 = 0x8C,
    BMP280_REISTER_DIGP1 = 0x8E,
    BMP280_REISTER_DIGP2 = 0x90,
    BMP280_REISTER_DIGP3 = 0x92,
    BMP280_REISTER_DIGP4 = 0x94,
    BMP280_REISTER_DIGP5 = 0x96,
    BMP280_REISTER_DIGP6 = 0x98,
    BMP280_REISTER_DIGP7 = 0x9A,
    BMP280_REISTER_DIGP8 = 0x9C,
    BMP280_REISTER_DIGP9 = 0x9E,
    BMP280_RESET_REGISTER = 0xE0,
    BMP280_ID_REGISTER = 0xD0,
    BMP280_STATUS_REGISTER = 0xF3,
    BMP280_CTRL_MES_REGISTER = 0xF4,
    BMP280_CONFIG_REGISTER = 0xF5,
    BMP280_TEMP_REGISTER = 0xFA,
    BMP280_PRESS_REGISTER = 0xF7
};

typedef struct {
    uint16_t dig_t1;
    int16_t dig_t2;
    int16_t dig_t3;
    uint16_t dig_p1;
    int16_t dig_p2;
    int16_t dig_p3;
    int16_t dig_p4;
    int16_t dig_p5;
    int16_t dig_p6;
    int16_t dig_p7;
    int16_t dig_p8;
    int16_t dig_p9;

} calibration_values;
//B7 et //28
class BMP280 {
public:
    BMP280(PinName sda_pin,PinName scl_pin,int address,uint8_t ii2_speed=3);
    int32_t getTemperature();
    int32_t getPressure();
    void activate();
    bool begin();
private: 
    calibration_values _bmp280_calibration_values;
    I2C i2c_bus;
    int32_t actual_temp,actual_press;
    int address_BMP280;
    uint8_t iic_speed_;
    int32_t t_fine;
    void readTemperature();
    void readPressure();
    uint8_t read_8bits(char register_to_read);
    uint16_t read_16bits(char register_to_read);
    int16_t readS_16bits(char register_to_read);
    uint16_t read_16bits_LE(char register_to_read);
    int16_t readS_16bits_LE(char register_to_read);
    uint32_t read_24bits(char register_to_read);
    void write_register8bits(char register_to_write,char* value,bool stop=false);
    uint8_t getChipId();
    uint8_t getStatus();
    void resetChip();
    void readCoefficient();
    void setConfig();
};

#endif