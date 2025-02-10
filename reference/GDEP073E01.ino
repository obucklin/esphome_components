#include <SPI.h>
#include "Display_EPD_W21_spi.h"
#include "Display_EPD_W21.h"
#include "image.h"
void setup() {
  pinMode(D0, INPUT);   //BUSY
  pinMode(D1, OUTPUT);  //RES
  pinMode(D2, OUTPUT);  //DC
  pinMode(D3, OUTPUT);  //CS
  //SPI       MOSI: A18(D10), SCK: A19(D8)
  SPI.beginTransaction(SPISettings(10000000, MSBFIRST, SPI_MODE0));
  SPI.begin();
  Serial.begin(9600);
}


//Tips//
/*
1.Flickering is normal when EPD is performing a full screen update to clear ghosting from the previous image so to ensure better clarity and legibility for the new image.
2.There will be no flicker when EPD performs a partial refresh.
3.Please make sue that EPD enters sleep mode when refresh is completed and always leave the sleep mode command. Otherwise, this may result in a reduced lifespan of EPD.
4.Please refrain from inserting EPD to the FPC socket or unplugging it when the MCU is being powered to prevent potential damage.)
5.Re-initialization is required for every full screen update.
6.When porting the program, set the BUSY pin to input mode and other pins to output mode.
*/
void loop() {
Serial.println("hello_screen");
#if 1  //Full screen refresh demostration.

  /************Full display*******************/
  EPD_init_fast();        //Full screen refresh initialization.
  Serial.println("init done");
  delay(5000);
  PIC_display(gImage_1);  //To Display one image using full screen refresh.
  EPD_sleep();            //Enter the sleep mode and please do not delete it, otherwise it will reduce the lifespan of the screen.
  Serial.println("render done");
  delay(5000);            //Delay for 5s.


#if 0          //Demonstration of Dispaly colored stripes, to enable this feature, please change 0 to 1.
               /************Full display*******************/
  EPD_init();  //Full screen refresh initialization.
  EPD_Display_White();
  delay(5000);  //Delay for 5s.
  EPD_Display_Black();
  delay(5000);  //Delay for 5s.
  EPD_Display_Yellow();
  delay(5000);  //Delay for 5s.
  EPD_Display_blue();
  delay(5000);  //Delay for 5s.
  EPD_Display_Green();
  delay(5000);  //Delay for 5s.
  EPD_Display_red();
  delay(5000);  //Delay for 5s.

#endif
  // EPD_init();  //Full screen refresh initialization.

  // PIC_display_Clear();  //Clear screen function.

  // EPD_sleep();  //Enter the sleep mode and please do not delete it, otherwise it will reduce the lifespan of the screen..
  // delay(2000);
#endif
  while (1)
    ;  // The program stops here
}
