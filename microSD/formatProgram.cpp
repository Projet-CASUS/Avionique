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

#define SPI_CS D9

SDBlockDevice s(SPI_MOSI, SPI_MISO, SPI_SCK,SPI_CS);
FATFileSystem sd("sd", &s); // the pinout on the mbed Cool Components workshop 

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
    sd.reformat(&s);
}
