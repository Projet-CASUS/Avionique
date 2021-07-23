# MPU-6050 (Gyroscope)
https://invensense.tdk.com/wp-content/uploads/2015/02/MPU-6000-Datasheet1.pdf

# Function map
## MPU6050(PinName sda_pin,PinName scl_pin,int address,uint8_t ii2_speed) (constructor)
The constructor creates an instance of MPU6050 class with the I2c pins, the device address and the I2C speed. The construtor accepts 4 different values for the speed: {0,1,2,3}. This number is used to divide the speed ratio of 400000 / (1+ii2_speed). 400000 Hz represents the maximum I2C for the MPU6050. The address must the 8 bits address of the device.

## set_iic_Speed(uint8_t new_speed)
The function change the actual I2C speed and replace it with the new_speed.

## begin(uint8_t c_range = MPU6050_DIV_0,uint8_t f_range = MPU6050_BAND_21_HZ,uint8_t g_range = MPU6050_RANGE_250_DEG,uint8_t a_range=MPU6050_RANGE_2_G)
This function configures the MPU6050 for measurement. By default every config register is set to their maximum value in terms of precision. See the dataset for more info about each register values.


### ***Every mesure return by the next functions are int32_t type***
## getTemperature()
the function returns value of temperature sensor on the chip. To get the real temperature value divide the return value by 100. Units are celsius degre.

## getAccel(int index=0)
the function returns the value of one of the accelerometer. The axe is passed by parameter to the function. To get the real accel value divide the return value by 1000. Units are G.

## getGyro(int index=0);
the function returns the value of one of the gyroscope. The axe is passed by parameter to the function. To get the real accel value divide the return value by 10. Units are deg/s^2.

## [] operator overloading
You can also use the subscript operator to get the value of temperature, accelerometer or gyroscope. In order to get the correct value refer to the table 1.
### Table 1
|Sensor name   | Index  |
|---|---|
| AccelX  | 0  |
| AccelY  |  1 |
| AccelZ  |  2 |
| Temperature  |  3 |
|  GyroX |  4 |
| GyroY  |  5 |
| GyroZ  |  6 |