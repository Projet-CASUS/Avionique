#include "PinNames.h"
#include "Timer.h"
#include "mbed.h"
#include "FATFileSystem.h"
#include "components/storage/blockdevice/COMPONENT_SD/SDBlockDevice.cpp"
#include "components/storage/blockdevice/COMPONENT_SD/SDBlockDevice.h"
#include <SPI.h>
#include "platform/mbed_thread.h"
#include "BMP280.h"
#include <cstdint>
#include <cstdio>
#include <ctime>

#define SPI_CS D9

SDBlockDevice s(SPI_MOSI, SPI_MISO, SPI_SCK,SPI_CS);
FATFileSystem sd("sd", &s);

Timer Timer_file;
Timer timeHistory;

void writeToSD(int a, int b, int c,FILE* fp) {
    if(fp != NULL)
        fprintf(fp, "%d;%d;%d\n",a,b,c);   
}
 
FILE* openFile() {
    FILE *fp = fopen("/sd/logs.csv", "a");
    return fp;
}

void closeFile(FILE* file) {
    fclose(file);
}

int main()
{
    Timer_file.start();
    int timeInterval = 0;
    BMP280 bmp280_sensor(I2C_SDA,I2C_SCL,0x77);
    bmp280_sensor.begin();
    FILE *logs = openFile();
    timeHistory.start();
    while(timeInterval < 4800) { // this limit the dataset
        if(Timer_file.read_ms() > 16) {
            int32_t press = bmp280_sensor.getPressure();
            int32_t temp = bmp280_sensor.getTemperature();
            writeToSD(press,temp,timeHistory.read_ms(),logs);
            timeInterval++;
            Timer_file.reset();
        }
    }
    if(logs != NULL) closeFile(logs);
}