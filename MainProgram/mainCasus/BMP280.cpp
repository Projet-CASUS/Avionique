#include "BMP280.h"
#include "PinNames.h"
#include "mstd_iterator"
#include <cstdint>
#include <cstdio>

/**
 * @brief Construct a new BMP280::BMP280 object
 * 
 * @param sda_pin 
 * @param scl_pin 
 * @param address 
 * @param ii2_speed select the speed divider - Defaut to 3 -> 400000 / (speed + 1) -> 100000kHz
 */
BMP280::BMP280(PinName sda_pin,PinName scl_pin,int address,uint8_t ii2_speed) : i2c_bus(sda_pin,scl_pin), address_BMP280(address<<1), iic_speed_(ii2_speed) {
}

/**
 * @brief Check if the sensor is connected and then read the coefficients.
 * 
 */
void BMP280::activate() {
    uint8_t sensorId = getChipId();
    if(sensorId == SENSOR_ID) {
        readCoefficient();
    }
}

/**
 * @brief set all the config for the measurement.
 * 
 */
void BMP280::setConfig() {
    char config_sampling[1] = {0xB7};
    write_register8bits(BMP280_CTRL_MES_REGISTER, config_sampling);

    config_sampling[0] = 0x28;
    write_register8bits(BMP280_CONFIG_REGISTER, config_sampling);
}

/**
 * @brief setup all the config and activate the sensor.
 * 
 * @return true if the sensor is ok to go.
 * @return false if a error occured.
 */
bool BMP280::begin() {
    activate();

    if(getChipId() == SENSOR_ID) {
        setConfig();
    }
    else return false;
    return true;
}

/**
 * @brief read 8 bits from the sensor.
 * 
 * @param register_to_read the register on the sensor to read som data.
 * @return uint8_t the data read in int 8 bits
 */
uint8_t BMP280::read_8bits(char register_to_read) {
    char cmd[1];
    char data[1];
    cmd[0] = register_to_read;
    i2c_bus.write(address_BMP280,cmd,1,false);
    i2c_bus.read(address_BMP280,data,1);
    uint16_t value = data[0];
    return value;
}

/**
 * @brief read 16 bits unsigned from the sensor
 * 
 * @param register_to_read the register on the sensor to read som data.
 * @return uint16_t the data read in int 16 unsigned
 */
uint16_t BMP280::read_16bits(char register_to_read) {
    char cmd[1];
    char data[2];
    cmd[0] = register_to_read;
    i2c_bus.write(address_BMP280,cmd,1,true);// true
    i2c_bus.read(address_BMP280,data,2,false);
    uint16_t value = data[0] << 8 | data[1];
    return value;
}

/**
 * @brief read 16 bits signed from the sensor
 * 
 * @param register_to_read the register on the sensor to read som data.
 * @return int16_t the data read in int 16 signed
 */
int16_t BMP280::readS_16bits(char register_to_read) {
    char cmd[1];
    char data[2];
    cmd[0] = register_to_read;
    i2c_bus.write(address_BMP280,cmd,1,false); // true
    i2c_bus.read(address_BMP280,data,2,false);
    int16_t value = data[0] << 8 | data[1];
    return value;
}

/**
 * @brief read 16 bits inverted and unsigned from the sensor
 * 
 * @param register_to_read the register on the sensor to read som data.
 * @return uint16_t the data read in int 16 unsigned
 */
uint16_t BMP280::read_16bits_LE(char register_to_read) {
    uint16_t value = read_16bits(register_to_read);
    return (value >> 8) | (value << 8);
}

/**
 * @brief read 16 bits inverted signedfrom the sensor
 * 
 * @param register_to_read the register on the sensor to read som data.
 * @return int16_t the data read in int 16 signed
 */
int16_t BMP280::readS_16bits_LE(char register_to_read) {
    int16_t value = (int16_t)read_16bits_LE(register_to_read);
    return value;
}

/**
 * @brief read 24 bits from the sensor
 * 
 * @param register_to_read 
 * @return uint32_t the data read in int 32 unsigned
 */
uint32_t BMP280::read_24bits(char register_to_read) {
    char cmd[1];
    char data[3];
    cmd[0] = register_to_read;
    i2c_bus.write(address_BMP280,cmd,1,true);
    i2c_bus.read(address_BMP280,data,3,false);
    uint32_t value = (data[0] << 16 | data[1] << 8 | data[2]);
    return value;
}

/**
 * @brief write 8 bits to the sensor
 * 
 * @param register_to_write 
 * @param value 
 * @param stop 
 */
void BMP280::write_register8bits(char register_to_write,char* value,bool stop) {
    char data[2];
    data[0] = register_to_write;
    data[1] = *value;
    i2c_bus.write(address_BMP280,data,2,stop);
}

/**
 * @brief get the chip ID of the connected sensor
 * 
 * @return uint8_t chipID
 */
uint8_t BMP280::getChipId() {
    uint8_t id = read_8bits(BMP280_ID_REGISTER);
    return id;
}

/**
 * @brief read the sensor's status
 * 
 * @return uint8_t sensor's status
 */
uint8_t BMP280::getStatus() {
    uint8_t status = read_8bits(BMP280_STATUS_REGISTER);
    return status;
}

/**
 * @brief read all the sensor's coefficients
 * 
 */
void BMP280::readCoefficient() {
    _bmp280_calibration_values.dig_t1 = read_16bits_LE(BMP280_REISTER_DIGT1);
    _bmp280_calibration_values.dig_t2 = readS_16bits_LE(BMP280_REISTER_DIGT2);
    _bmp280_calibration_values.dig_t3 = readS_16bits_LE(BMP280_REISTER_DIGT3);

    _bmp280_calibration_values.dig_p1 = read_16bits_LE(BMP280_REISTER_DIGP1);
    _bmp280_calibration_values.dig_p2 = readS_16bits_LE(BMP280_REISTER_DIGP2);
    _bmp280_calibration_values.dig_p3 = readS_16bits_LE(BMP280_REISTER_DIGP3);
    _bmp280_calibration_values.dig_p4 = readS_16bits_LE(BMP280_REISTER_DIGP4);
    _bmp280_calibration_values.dig_p5 = readS_16bits_LE(BMP280_REISTER_DIGP5);
    _bmp280_calibration_values.dig_p6 = readS_16bits_LE(BMP280_REISTER_DIGP6);
    _bmp280_calibration_values.dig_p7 = readS_16bits_LE(BMP280_REISTER_DIGP7);
    _bmp280_calibration_values.dig_p8 = readS_16bits_LE(BMP280_REISTER_DIGP8);
    _bmp280_calibration_values.dig_p9 = readS_16bits_LE(BMP280_REISTER_DIGP9);
}

/**
 * @brief reset the chip
 * 
 */
void BMP280::resetChip() {
    char RESET_VALUE = 0xB6;
    write_register8bits(BMP280_RESET_REGISTER, &RESET_VALUE);
}

/**
 * @brief read the temperature from the sensor and store it into a variable.
 * the formula comes from the BMP280 datasheet
 */
void BMP280::readTemperature() {
    int32_t t1,t2;
    int32_t readings_T = read_24bits(BMP280_TEMP_REGISTER);
    
    readings_T >>= 4;

    t1 = ((((readings_T>>3) - ((int32_t)_bmp280_calibration_values.dig_t1<<1))) * ((int32_t)_bmp280_calibration_values.dig_t2)) >> 11;

    t2 = (((((readings_T >> 4) - ((int32_t)_bmp280_calibration_values.dig_t1)) *
            ((readings_T >> 4) - ((int32_t)_bmp280_calibration_values.dig_t1))) >>
           12) *
          ((int32_t)_bmp280_calibration_values.dig_t3)) >>
         14;
    t_fine = t1 + t2;
    actual_temp = (t_fine * 5 + 128) >> 8; 
}

/**
 * @brief read the pressure from the sensor and store it into a variable.
 * the formula comes from the BMP280 datasheet
 */
void BMP280::readPressure() {
    int64_t p1,p2,P;

    readTemperature();

    int32_t readings_P = read_24bits(BMP280_PRESS_REGISTER);
    readings_P >>= 4;

    p1 = ((int64_t)t_fine) - 128000;
    p2 = p1 * p1 * (int64_t)_bmp280_calibration_values.dig_p6;
    p2 = p2 + ((p1 * (int64_t)_bmp280_calibration_values.dig_p5) << 17);
    p2 = p2 + (((int64_t)_bmp280_calibration_values.dig_p4) << 35);
    p1 = ((p1 * p1 * (int64_t)_bmp280_calibration_values.dig_p3) >> 8) +
            ((p1 * (int64_t)_bmp280_calibration_values.dig_p2) << 12);
    p1 =
        (((((int64_t)1) << 47) + p1)) * ((int64_t)_bmp280_calibration_values.dig_p1) >> 33;

    if (p1 == 0) {
        actual_press = -1;
        return;
    }
    P = 1048576 - readings_P;
    P = (((P << 31) - p2) * 3125) / p1;
    p1 = (((int64_t)_bmp280_calibration_values.dig_p9) * (P >> 13) * (P >> 13)) >> 25;
    p2 = (((int64_t)_bmp280_calibration_values.dig_p8) * P) >> 19;

    P = ((P + p1 + p2) >> 8) + (((int64_t)_bmp280_calibration_values.dig_p7) << 4);
    actual_press = (P / 256);
}

/**
 * @brief return the value of the temperature *100
 * 
 * @return int32_t 
 */
int32_t BMP280::getTemperature() {
    readTemperature();
    return actual_temp;
}

/**
 * @brief return the value of the pressure *100
 * 
 * @return int32_t 
 */
int32_t BMP280::getPressure() {
    readPressure();
    return actual_press;
}