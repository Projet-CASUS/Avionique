#include "mbed.h"
#include "FATFileSystem.h"
#include "components/storage/blockdevice/COMPONENT_SD/SDBlockDevice.cpp"
#include "platform/mbed_thread.h"


#define SPI_CS D9

SDBlockDevice s(SPI_MOSI, SPI_MISO, SPI_SCK,SPI_CS);
FATFileSystem sd("sd", &s); // the pinout on the mbed Cool Components workshop board
 
int ecrire(double a, double b, double c) {
   
    FILE *fp = fopen("/sd/Altimeter.txt", "a");
    if (fp==NULL)
    {
        printf("le fichier n'a pas pu etre ouvert \n");
        return 0;
    }

    fprintf(fp, "%.2f\t %.2f\t, %.2f\n",a,b,c);

    fclose(fp); 

return 1;    
}


