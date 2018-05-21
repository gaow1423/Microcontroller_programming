	The Code is intended to enable the microcontroller to read and write data at particular pins. Besides, it 
will specific all relevant parameters such as clock speed, baud rate and etc, inorder to match the peripheral devices. 
The code will be written in C to configure I2C protocol, UART communication SPI.

Note: The compile should include AVR library in order to run main.c. The Makefile is referenced from public domain. 

Follow the instruction below:

1. Type "make all" in command line followed by Enter Key to run main.c using makefile. All relevant files including ".hex" file will be generated in current folder.
2. Connect the Teensy 2.0 with the computer by USB.
3. Open the Teensy Loader Application, and write the main.hex file into the Teensy 2.0
4. Type "make clean" to clean up all generated files and leave only source codes in the folder. 
