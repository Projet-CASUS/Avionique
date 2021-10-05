#include "PinNames.h"
#include "mbed.h"
#include "FATFileSystem.h"
#include "components/storage/blockdevice/COMPONENT_SD/SDBlockDevice.cpp"
#include "components/storage/blockdevice/COMPONENT_SD/SDBlockDevice.h"
#include <SPI.h>
#include "platform/mbed_thread.h"
#include <cstdint>
#include <cstdio>
#include <ctime>
#include "MPU6050.h"
#include "BMP280.h"
#include <vector>

#define SPI_CS D9

std::vector<vector<int>> fileBlock;
FILE* instantFile;

SDBlockDevice s(SPI_MOSI, SPI_MISO, SPI_SCK,SPI_CS);
FATFileSystem sd("sd", &s);

Timer mainTimer;
Timer smallTimer;
Timer backUpTimer;

void writeToSD(vector<int> values,FILE* fp) {
    if(fp != NULL) {
        for(int i=0;i<values.size();i++) {
            fprintf(fp,"%d",values[i]);
            if(i < values.size()-1) {
                fprintf(fp, ";");
            }
        }
        fprintf(fp, "\n");
    }  
}
 
bool openFile(const char* filename,const char* filemode,FILE *f) {
    f = fopen(filename,filemode);
    return f != NULL;
}

void closeFile(FILE* file) {
    fclose(file);
}

void writeVectorValues() {
    openFile("/sd/logs.csv", "a", instantFile);
    for(int i=0;i<fileBlock.size();i++) {
        writeToSD(fileBlock[i], instantFile);
    }
    closeFile(instantFile);
    fileBlock.clear();
    fileBlock.resize(0);
    mainTimer.reset();
}

int main()
{
    int timeInterval = 0;
    MPU6050 *mpuSensor = new MPU6050(I2C_SDA,I2C_SCL,0x68);
    BMP280 *bmpSensor = new BMP280(I2C_SDA,I2C_SCL,0x76);
    smallTimer.start();
    mainTimer.start();

    while(1) {
        if(mainTimer.read_ms() >= 1000) {
            writeVectorValues();
        }

        if(smallTimer.read_us() >= 8333) {
            mpuSensor->getAllReadings();
            std::vector<int> tempdata{mpuSensor->getAccel(0),mpuSensor->getAccel(1),mpuSensor->getAccel(2),
            mpuSensor->getGyro(0),mpuSensor->getGyro(1),mpuSensor->getGyro(2),bmpSensor->getTemperature(),bmpSensor->getTemperature(),timeInterval};
            fileBlock.push_back(tempdata);
            timeInterval++;
        }
    }

    delete mpuSensor;
    delete bmpSensor;
}