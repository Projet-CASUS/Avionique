# Micro-SD card
https://cdn-learn.adafruit.com/downloads/pdf/adafruit-micro-sd-breakout-board-card-tutorial.pdf?timestamp=1613847927

# Components to add 
In order to be able to compile the sd program in mbed, you have include the SD et SPIF in the targets.json.
The compnents shuold be add only for the platform you working on (ex: MCU_STM32L496xG)

Add in targets.json
"components_add": [
            "FLASHIAP",
            "SD",
            "SPIF"
        ],

# Format SD
in this directory the file named formatProgram format the sd in the good file system. This program solve the problem if the file won't open on the main program.