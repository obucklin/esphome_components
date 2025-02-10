#pragma once
#include "esphome/core/component.h"
#include "esphome/components/spi/spi.h"
#include "esphome/components/display/display_buffer.h"
// #include <SPI.h>

class GDEP073E01 : public esphome::Component, public esphome::display::DisplayBuffer {
   public:
  void setup() override;
  void update() override;
  void dump_config() override;
  
  void set_busy_pin(GPIOPin *pin) { busy_pin_ = pin; }
  void set_reset_pin(GPIOPin *pin) { reset_pin_ = pin; }
  void set_dc_pin(GPIOPin *pin) { dc_pin_ = pin; }
  void set_cs_pin(GPIOPin *pin) { cs_pin_ = pin; }

 protected:
  void initialize_display();
  void wait_until_idle();
  void send_command(uint8_t command);
  void send_data(uint8_t data);
  
  GPIOPin *busy_pin_{nullptr};
  GPIOPin *reset_pin_{nullptr};
  GPIOPin *dc_pin_{nullptr};
  GPIOPin *cs_pin_{nullptr};
  SPIClass *spi_{nullptr};

  uint16_t get_width_internal() const override;
  uint16_t get_height_internal() const override;

#ifndef _GDEP073E01_H_
#define _GDEP073E01_H_


#define EPD_WIDTH   800
#define EPD_HEIGHT  480

//8bit
#define Black   0x00  
#define White   0x11  
#define Green   0x66  
#define Blue    0x55  
#define Red     0x33  
#define Yellow  0x22 
#define Clean   0x77  

//4bit
#define black   0x00  
#define white   0x01  
#define green   0x06  
#define blue    0x05  
#define red     0x03  
#define yellow  0x02 
#define clean   0x07  

#define PSR         0x00
#define PWRR         0x01
#define POF         0x02
#define POFS        0x03
#define PON         0x04
#define BTST1       0x05
#define BTST2       0x06
#define DSLP        0x07
#define BTST3       0x08
#define DTM         0x10
#define DRF         0x12
#define PLL         0x30
#define CDI         0x50
#define TCON        0x60
#define TRES        0x61
#define REV         0x70
#define VDCS        0x82
#define T_VDCS      0x84
#define PWS         0xE3

#endif
/***********************************************************
            end file
***********************************************************/

};

