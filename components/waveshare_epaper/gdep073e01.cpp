#include "waveshare_epaper.h"
#include "esphome/core/log.h"
#include "esphome/core/application.h"
#include "Arduino.h"
#include "SPI.h"
#include "image_copy.h"

#define isEPD_W21_BUSY digitalRead(A6)      // BUSY
#define EPD_W21_RST_0 digitalWrite(A3, LOW) // RES
#define EPD_W21_RST_1 digitalWrite(A3, HIGH)
#define EPD_W21_DC_0 digitalWrite(A4, LOW) // DC
#define EPD_W21_DC_1 digitalWrite(A4, HIGH)
#define EPD_W21_CS_0 digitalWrite(A5, LOW) // CS
#define EPD_W21_CS_1 digitalWrite(A5, HIGH)

// 4bit

namespace esphome
{
  namespace waveshare_epaper
  {

    static const char *const TAG = "gdep073e01";

    uint32_t GoodDisplayGdep073e01::idle_timeout_() { return 10000; }

    void GoodDisplayGdep073e01::initialize()
    {
      ESP_LOGD("TAG", "initializing GoodDisplayGdep073e01");
      init_display_();
      for (int i = 0; i < get_buffer_length_(); i++)
      {
        this->buffer_[i] = 0x00;
      }
      ESP_LOGD("TAG", "sample buffer = %d", this->buffer_[342]);
    }

    bool GoodDisplayGdep073e01::wait_until_idle_()
    {
      if (this->busy_pin_ == nullptr)
      {
        return true;
      }

      const uint32_t start = millis();
      while (this->busy_pin_->digital_read())
      {
        this->command(0x71);
        if (millis() - start > this->idle_timeout_())
        {
          ESP_LOGI(TAG, "Timeout while displaying image!");
          return false;
        }
        App.feed_wdt();
        delay(10);
      }
      delay(200); // NOLINT
      return true;
    };

    void GoodDisplayGdep073e01::init_display_()
    {
      this->reset_();

      this->command(0xAA); // CMDH
      this->data(0x49);
      this->data(0x55);
      this->data(0x20);
      this->data(0x08);
      this->data(0x09);
      this->data(0x18);

      this->command(0x01); //
      this->data(0x3F);

      this->command(0x00);
      this->data(0x5F);
      this->data(0x69);

      this->command(0x03);
      this->data(0x00);
      this->data(0x54);
      this->data(0x00);
      this->data(0x44);

      this->command(0x05);
      this->data(0x40);
      this->data(0x1F);
      this->data(0x1F);
      this->data(0x2C);

      this->command(0x06);
      this->data(0x6F);
      this->data(0x1F);
      this->data(0x17);
      this->data(0x49);

      this->command(0x08);
      this->data(0x6F);
      this->data(0x1F);
      this->data(0x1F);
      this->data(0x22);
      this->command(0x30);
      this->data(0x08);
      this->command(0x50);
      this->data(0x3F);

      this->command(0x60);
      this->data(0x02);
      this->data(0x00);

      this->command(0x61);
      this->data(0x03);
      this->data(0x20);
      this->data(0x01);
      this->data(0xE0);

      this->command(0x84);
      this->data(0x01);

      this->command(0xE3);
      this->data(0x2F);

      this->command(0x04); // PWR on
      ESP_LOGD("TAG", "init end reached");

      wait_until_idle_();
    };

    void GoodDisplayGdep073e01::init_display_fast_()
    {
      this->reset_();

      command(0xAA); // CMDH
      command(0x49);
      command(0x55);
      command(0x20);
      command(0x08);
      command(0x09);
      command(0x18);

      command(0x01);
      command(0x3F);
      command(0x00);
      command(0x32);
      command(0x2A);
      command(0x0E);
      command(0x2A);

      command(0x00);
      command(0x5F);
      command(0x69);

      command(0x03);
      command(0x00);
      command(0x54);
      command(0x00);
      command(0x44);

      command(0x05);
      command(0x40);
      command(0x1F);
      command(0x1F);
      command(0x2C);

      command(0x06);
      command(0x6F);
      command(0x1F);
      command(0x16);
      command(0x25);

      command(0x08);
      command(0x6F);
      command(0x1F);
      command(0x1F);
      command(0x22);

      command(0x13); // IPC
      command(0x00);
      command(0x04);

      command(0x30);
      command(0x02);

      command(0x41); // TSE
      command(0x00);

      command(0x50);
      command(0x3F);

      command(0x60);
      command(0x02);
      command(0x00);

      command(0x61);
      command(0x03);
      command(0x20);
      command(0x01);
      command(0xE0);

      command(0x82);
      command(0x1E);

      command(0x84);
      command(0x01);

      command(0x86); // AGID
      command(0x00);

      command(0xE3);
      command(0x2F);

      command(0xE0); // CCSET
      command(0x00);

      command(0xE6); // TSSET
      command(0x00);

      command(0x04); // PWR on
      wait_until_idle_();
    };

    unsigned char GoodDisplayGdep073e01::get_color(Color color)
    {
      int16_t red = color.r;
      int16_t green = color.g;
      int16_t blue = color.b;
      unsigned char cv7 = 0x01;

      if ((red < 0x80) && (green < 0x80) && (blue < 0x80))
        cv7 = 0x00; // black
      else if ((red >= 0x80) && (green >= 0x80) && (blue >= 0x80))
        cv7 = 0x01; // white
      else if ((green >= 0x80) && (blue >= 0x80))
        cv7 = green > blue ? 0x06 : 0x05; // green, blue
      else if ((red >= 0x80) && (green >= 0x80))
        cv7 = green > red ? 0x06 : 0x03; // yellow, orange
      else if (red >= 0x80)
        cv7 = 0x03; // red
      else if (green >= 0x80)
        cv7 = 0x06; // green
      else
        cv7 = 0x05; // blue

      return cv7;
    }

    void HOT GoodDisplayGdep073e01::draw_absolute_pixel_internal(int x, int y, Color color)
    {

      if (x >= this->get_width_internal() || y >= this->get_height_internal() || x < 0 || y < 0)
        return;


      // ESP_LOGD("TAG", "x = %d", x);

      const uint32_t pos = (x + (y * (this->get_width_internal()))) / 2u;
      // ESP_LOGD("TAG", "pos = %d", pos);
      unsigned char cv = this->get_color(color);

      unsigned char temp_byte = this->buffer_[pos];
      // ESP_LOGD("TAG", "temp_byte = %02X", temp_byte);

      if (temp_byte & 0xF0 == 0xF0) temp_byte &= 0x0F;
      if (temp_byte & 0x0F == 0x0F) temp_byte &= 0xF0;

      if (x & 0x01)
      {
        temp_byte &= 0x0F;
        temp_byte |= (cv << 4);
      }
      else
      {
        temp_byte &= 0xF0;
        temp_byte |= cv;
      }
      // ESP_LOGD("TAG", "byte = %02X", temp_byte);

      this->buffer_[pos] = temp_byte;

      // ESP_LOGD("TAG", "sample buffer_ byte = %02X", this->buffer_[pos]);
    }

    uint32_t GoodDisplayGdep073e01::get_buffer_length_()
    {
      uint32_t len = get_height_internal() * get_width_internal() / 2;
      return len;
    }

    void GoodDisplayGdep073e01::display_bytes_(const unsigned char *bytesData)
    {
      unsigned int i, j, k;
      unsigned char temp1, temp2;
      unsigned char data;

      // Acep_color(White); //Each refresh must be cleaned first
      this->command(0x10);
      for (i = 0; i < sizeof(bytesData); i++)
      {
        this->data(data);
      }
      this->command(0x12); // DISPLAY REFRESH
      this->data(0x00);
      delay(1);        //!!!The delay here is necessary, 200uS at least!!!
      wait_until_idle_(); // waiting for the electronic paper IC to release the idle signal
    }

    void GoodDisplayGdep073e01::display_pic_(const unsigned char *picData)
    {
      unsigned int i, j, k;
      unsigned char temp1, temp2;
      unsigned char data;

      // Acep_color(White); //Each refresh must be cleaned first
      this->command(0x10);
      for (i = 0; i < sizeof(picData) / 2; i++)
      {

        temp1 = picData[i++] << 4;
        temp2 = picData[i++];
        data = temp1 | temp2;
        this->data(data);
      }
      this->command(0x12); // DISPLAY REFRESH
      this->data(0x00);
      delay(1);        //!!!The delay here is necessary, 200uS at least!!!
      wait_until_idle_(); 
    }

    void GoodDisplayGdep073e01::display_fill_color_(const unsigned char color)
    {
      this->command(0x10);
      {
        for (unsigned long i = 0; i < 192000; i++)
        {
          this->data(color);
        }
      }
      this->command(0x04);
      wait_until_idle_();

      // 20211212
      // Second setting
      this->command(0x06);
      this->data(0x6F);
      this->data(0x1F);
      this->data(0x17);
      this->data(0x49);

      this->command(0x12);
      this->data(0x00);
      wait_until_idle_();

      this->command(0x02);
      this->data(0x00);
      wait_until_idle_();
    }

    void HOT GoodDisplayGdep073e01::display()
    {
      EPD_init_fast();
      // ESP_LOGD("TAG", "display");
      // unsigned char c = 0x03;
      // ESP_LOGD("TAG", "c &= 0x0F = %02X", c &= 0x0F);
      // unsigned char d = 0x02;
      // ESP_LOGD("TAG", "d = %02X", d);
      // ESP_LOGD("TAG", "d<<4 | c = %02X", d<<4|c);
      // unsigned char e = 0x01;
      // unsigned char f = 0x02;
      // unsigned char g = e | f;
      // ESP_LOGD("TAG", "g = %02X", g);

      // unsigned char h = e<<4 | f;

      // ESP_LOGD("TAG", "h = %02X", h);
      ESP_LOGD("TAG", "sample buffer_ byte = %02X", this->buffer_[1704]);
      ESP_LOGD("TAG", "sample buffer_ byte = %02X", this->buffer_[1505]);

      // EPD_init();
      // EPD_Display_Red();
      // delay(5000);
      // BYTES_display(this->buffer_);
      // EPD_display_Byte_Color(0x11);
      // delay(10000);
      PIC_display(stock_image);
      ESP_LOGD("TAG", "sample buffer_ byte = %02X", this->buffer_[1704]);
      ESP_LOGD("TAG", "sample buffer_ byte = %02X", this->buffer_[1505]);
      EPD_sleep();
    }

    int GoodDisplayGdep073e01::get_width_internal() { return 200; }

    int GoodDisplayGdep073e01::get_height_internal() { return 480; }

    void GoodDisplayGdep073e01::dump_config()
    {
      LOG_DISPLAY("", "GoodDisplay 6-color E-Paper", this);
      ESP_LOGCONFIG(TAG, "  Model: gdep073e01");
      LOG_PIN("  Reset Pin: ", this->reset_pin_);
      LOG_PIN("  DC Pin: ", this->dc_pin_);
      LOG_PIN("  Busy Pin: ", this->busy_pin_);
      LOG_UPDATE_INTERVAL(this);
    }
    
    // void GoodDisplayGdep073e01::EPD_W21_Init(void)
    // {
    //   EPD_W21_RST_0; // Module reset
    //   delay(10);     // At least 10ms delay
    //   EPD_W21_RST_1;
    //   delay(10); // At least 10ms delay
    // }
    //
    // void GoodDisplayGdep073e01::lcd_chkstatus()
    // {
    //   while (isEPD_W21_BUSY)
    //     ;
    // }
    //
    // // slow
    // void GoodDisplayGdep073e01::EPD_init()
    // {

    //   EPD_W21_Init(); // Electronic paper IC reset

    //   EPD_W21_WriteCMD(0xAA); // CMDH
    //   EPD_W21_WriteDATA(0x49);
    //   EPD_W21_WriteDATA(0x55);
    //   EPD_W21_WriteDATA(0x20);
    //   EPD_W21_WriteDATA(0x08);
    //   EPD_W21_WriteDATA(0x09);
    //   EPD_W21_WriteDATA(0x18);

    //   EPD_W21_WriteCMD(0x01); //
    //   EPD_W21_WriteDATA(0x3F);

    //   EPD_W21_WriteCMD(0x00);
    //   EPD_W21_WriteDATA(0x5F);
    //   EPD_W21_WriteDATA(0x69);

    //   EPD_W21_WriteCMD(0x03);
    //   EPD_W21_WriteDATA(0x00);
    //   EPD_W21_WriteDATA(0x54);
    //   EPD_W21_WriteDATA(0x00);
    //   EPD_W21_WriteDATA(0x44);

    //   EPD_W21_WriteCMD(0x05);
    //   EPD_W21_WriteDATA(0x40);
    //   EPD_W21_WriteDATA(0x1F);
    //   EPD_W21_WriteDATA(0x1F);
    //   EPD_W21_WriteDATA(0x2C);

    //   EPD_W21_WriteCMD(0x06);
    //   EPD_W21_WriteDATA(0x6F);
    //   EPD_W21_WriteDATA(0x1F);
    //   EPD_W21_WriteDATA(0x17);
    //   EPD_W21_WriteDATA(0x49);

    //   EPD_W21_WriteCMD(0x08);
    //   EPD_W21_WriteDATA(0x6F);
    //   EPD_W21_WriteDATA(0x1F);
    //   EPD_W21_WriteDATA(0x1F);
    //   EPD_W21_WriteDATA(0x22);
    //   EPD_W21_WriteCMD(0x30);
    //   EPD_W21_WriteDATA(0x08);
    //   EPD_W21_WriteCMD(0x50);
    //   EPD_W21_WriteDATA(0x3F);

    //   EPD_W21_WriteCMD(0x60);
    //   EPD_W21_WriteDATA(0x02);
    //   EPD_W21_WriteDATA(0x00);

    //   EPD_W21_WriteCMD(0x61);
    //   EPD_W21_WriteDATA(0x03);
    //   EPD_W21_WriteDATA(0x20);
    //   EPD_W21_WriteDATA(0x01);
    //   EPD_W21_WriteDATA(0xE0);

    //   EPD_W21_WriteCMD(0x84);
    //   EPD_W21_WriteDATA(0x01);

    //   EPD_W21_WriteCMD(0xE3);
    //   EPD_W21_WriteDATA(0x2F);

    //   EPD_W21_WriteCMD(0x04); // PWR on
    //   lcd_chkstatus();        // waiting for the electronic paper IC to release the idle signal
    //   ESP_LOGD("TAG", "init end reached");
    // }
    // // fast
    // void GoodDisplayGdep073e01::EPD_init_fast(void)
    // {

    //   EPD_W21_Init(); // Electronic paper IC reset

    //   EPD_W21_WriteCMD(0xAA); // CMDH
    //   EPD_W21_WriteDATA(0x49);
    //   EPD_W21_WriteDATA(0x55);
    //   EPD_W21_WriteDATA(0x20);
    //   EPD_W21_WriteDATA(0x08);
    //   EPD_W21_WriteDATA(0x09);
    //   EPD_W21_WriteDATA(0x18);

    //   EPD_W21_WriteCMD(0x01);
    //   EPD_W21_WriteDATA(0x3F);
    //   EPD_W21_WriteDATA(0x00);
    //   EPD_W21_WriteDATA(0x32);
    //   EPD_W21_WriteDATA(0x2A);
    //   EPD_W21_WriteDATA(0x0E);
    //   EPD_W21_WriteDATA(0x2A);

    //   EPD_W21_WriteCMD(0x00);
    //   EPD_W21_WriteDATA(0x5F);
    //   EPD_W21_WriteDATA(0x69);

    //   EPD_W21_WriteCMD(0x03);
    //   EPD_W21_WriteDATA(0x00);
    //   EPD_W21_WriteDATA(0x54);
    //   EPD_W21_WriteDATA(0x00);
    //   EPD_W21_WriteDATA(0x44);

    //   EPD_W21_WriteCMD(0x05);
    //   EPD_W21_WriteDATA(0x40);
    //   EPD_W21_WriteDATA(0x1F);
    //   EPD_W21_WriteDATA(0x1F);
    //   EPD_W21_WriteDATA(0x2C);

    //   EPD_W21_WriteCMD(0x06);
    //   EPD_W21_WriteDATA(0x6F);
    //   EPD_W21_WriteDATA(0x1F);
    //   EPD_W21_WriteDATA(0x16);
    //   EPD_W21_WriteDATA(0x25);

    //   EPD_W21_WriteCMD(0x08);
    //   EPD_W21_WriteDATA(0x6F);
    //   EPD_W21_WriteDATA(0x1F);
    //   EPD_W21_WriteDATA(0x1F);
    //   EPD_W21_WriteDATA(0x22);

    //   EPD_W21_WriteCMD(0x13); // IPC
    //   EPD_W21_WriteDATA(0x00);
    //   EPD_W21_WriteDATA(0x04);

    //   EPD_W21_WriteCMD(0x30);
    //   EPD_W21_WriteDATA(0x02);

    //   EPD_W21_WriteCMD(0x41); // TSE
    //   EPD_W21_WriteDATA(0x00);

    //   EPD_W21_WriteCMD(0x50);
    //   EPD_W21_WriteDATA(0x3F);

    //   EPD_W21_WriteCMD(0x60);
    //   EPD_W21_WriteDATA(0x02);
    //   EPD_W21_WriteDATA(0x00);

    //   EPD_W21_WriteCMD(0x61);
    //   EPD_W21_WriteDATA(0x03);
    //   EPD_W21_WriteDATA(0x20);
    //   EPD_W21_WriteDATA(0x01);
    //   EPD_W21_WriteDATA(0xE0);

    //   EPD_W21_WriteCMD(0x82);
    //   EPD_W21_WriteDATA(0x1E);

    //   EPD_W21_WriteCMD(0x84);
    //   EPD_W21_WriteDATA(0x01);

    //   EPD_W21_WriteCMD(0x86); // AGID
    //   EPD_W21_WriteDATA(0x00);

    //   EPD_W21_WriteCMD(0xE3);
    //   EPD_W21_WriteDATA(0x2F);

    //   EPD_W21_WriteCMD(0xE0); // CCSET
    //   EPD_W21_WriteDATA(0x00);

    //   EPD_W21_WriteCMD(0xE6); // TSSET
    //   EPD_W21_WriteDATA(0x00);

    //   EPD_W21_WriteCMD(0x04); // PWR on
    //   lcd_chkstatus();        // waiting for the electronic paper IC to release the idle signal
    // }

    // void GoodDisplayGdep073e01::EPD_sleep(void)
    // {
    //   EPD_W21_WriteCMD(0X02); // power off
    //   EPD_W21_WriteDATA(0x00);
    //   lcd_chkstatus(); // waiting for the electronic paper IC to release the idle signal
    //   /*EPD_W21_WriteCMD(0X07);   //deep sleep
    //   EPD_W21_WriteDATA(0xA5);*/
    // }
    


    // void GoodDisplayGdep073e01::EPD_BYTES_display(const unsigned char *bytesData)
    // {
    //   unsigned int i, j, k;
    //   unsigned char temp1, temp2;
    //   unsigned char data;

    //   // Acep_color(White); //Each refresh must be cleaned first
    //   EPD_W21_WriteCMD(0x10);
    //   for (i = 0; i < sizeof(bytesData); i++)
    //   {
    //     EPD_W21_WriteDATA(data);
    //   }
    //   EPD_W21_WriteCMD(0x12); // DISPLAY REFRESH
    //   EPD_W21_WriteDATA(0x00);
    //   delay(1);        //!!!The delay here is necessary, 200uS at least!!!
    //   lcd_chkstatus(); // waiting for the electronic paper IC to release the idle signal
    // }




    // void GoodDisplayGdep073e01::PIC_display(const unsigned char *picData)
    // {
    //   unsigned int i, j, k;
    //   unsigned char temp1, temp2;
    //   unsigned char data;

    //   // Acep_color(White); //Each refresh must be cleaned first
    //   EPD_W21_WriteCMD(0x10);
    //   for (i = 0; i < sizeof(picData) / 2; i++)
    //   {

    //     temp1 = picData[i++] << 4;
    //     temp2 = picData[i++];
    //     data = temp1 | temp2;
    //     EPD_W21_WriteDATA(data);
    //   }

    //   // Refresh
    //   EPD_W21_WriteCMD(0x12); // DISPLAY REFRESH
    //   EPD_W21_WriteDATA(0x00);
    //   delay(1);        //!!!The delay here is necessary, 200uS at least!!!
    //   lcd_chkstatus(); // waiting for the electronic paper IC to release the idle signal
    // }

    // void GoodDisplayGdep073e01::PIC_display_Clear(void)
    // {
    //   unsigned int i, j;
    //   // Acep_color(Clean); //Each refresh must be cleaned first
    //   EPD_W21_WriteCMD(0x10);
    //   for (i = 0; i < 480; i++)
    //   {
    //     for (j = 0; j < 800 / 2; j++)
    //     {
    //       EPD_W21_WriteDATA(0x00);
    //     }
    //   }
    //   // Refresh
    //   EPD_W21_WriteCMD(0x12); // DISPLAY REFRESH
    //   EPD_W21_WriteDATA(0x00);
    //   delay(1);        //!!!The delay here is necessary, 200uS at least!!!
    //   lcd_chkstatus(); // waiting for the electronic paper IC to release the idle signal
    // }

    // void GoodDisplayGdep073e01::EPD_display_Byte_Color(const unsigned char color)
    // {
    //   EPD_W21_WriteCMD(0x10);
    //   {
    //     for (unsigned long i = 0; i < 192000; i++)
    //     {
    //       EPD_W21_WriteDATA(color);
    //     }
    //   }
    //   EPD_W21_WriteCMD(0x04);
    //   lcd_chkstatus();

    //   // 20211212
    //   // Second setting
    //   EPD_W21_WriteCMD(0x06);
    //   EPD_W21_WriteDATA(0x6F);
    //   EPD_W21_WriteDATA(0x1F);
    //   EPD_W21_WriteDATA(0x17);
    //   EPD_W21_WriteDATA(0x49);

    //   EPD_W21_WriteCMD(0x12);
    //   EPD_W21_WriteDATA(0x00);
    //   lcd_chkstatus();

    //   EPD_W21_WriteCMD(0x02);
    //   EPD_W21_WriteDATA(0x00);
    //   lcd_chkstatus();
    // }

    // void GoodDisplayGdep073e01::SPI_Write(unsigned char value)
    // {
    //   SPI.transfer(value);
    // }

    // // SPI write command
    // void GoodDisplayGdep073e01::EPD_W21_WriteCMD(unsigned char command)
    // {
    //   EPD_W21_CS_0;
    //   EPD_W21_DC_0; // D/C#   0:command  1:data
    //   SPI_Write(command);
    //   EPD_W21_CS_1;
    // }
    // // SPI write data
    // void GoodDisplayGdep073e01::EPD_W21_WriteDATA(unsigned char datas)
    // {
    //   EPD_W21_CS_0;
    //   EPD_W21_DC_1; // D/C#   0:command  1:data
    //   SPI_Write(datas);
    //   EPD_W21_CS_1;
    // }
  }
}